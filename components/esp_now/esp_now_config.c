// Copyright 2022 PWrInSpace, Kuba
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_now_config.h"
#include "esp_log.h"
#include "esp_now_api.h"
#include "rocket_data.h"
#include "state_machine.h"
#include "errors_config.h"

#define TAG "ENC"

static struct {
    TimerHandle_t broadcast_timer;
} gb = {
    .broadcast_timer = NULL,
};

static void callback_pitot(uint8_t *data, size_t size);
static void callback_vent_valve(uint8_t *data, size_t size);
static void callback_main_valve(uint8_t *data, size_t size);
static void callback_tanwa(uint8_t *data, size_t size);

const ENA_device_t esp_now_broadcast = {
    .peer = {.peer_addr = BROADCAST_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = NULL,
};

const ENA_device_t esp_now_pitot = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_pitot,
};

const ENA_device_t esp_now_vent_valve = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_vent_valve,
};

const ENA_device_t esp_now_main_valve = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_main_valve,
};

const ENA_device_t esp_now_tanwa = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_tanwa,
};

// static void on_err(ENA_ERROR err) {
//     ESP_LOGE(TAG, "ERROR :C %d", err);
// }

static void callback_pitot(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Pitot receive, size %d", size);
}

static void callback_vent_valve(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Vent receive, size %d", size);
    if (size == sizeof(vent_valve_data_t)) {
        rocket_data_update_vent_valve((vent_valve_data_t *) data);
    }
}

static void callback_main_valve(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Main receive, size %d", size);
    if (size == sizeof(main_valve_data_t)) {
        rocket_data_update_main_valve((main_valve_data_t *) data);
    }
}

static void callback_tanwa(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Tanwa receive, size %d", size);
}

static void on_broadcast_timer(TimerHandle_t timer) {
    uint8_t state = SM_get_current_state();
    ENA_send(&esp_now_broadcast, &state, sizeof(state), 0);
}

bool esp_now_broadcast_timer_start(uint32_t period_ms) {
    gb.broadcast_timer = xTimerCreate("broadcast timer", pdMS_TO_TICKS(period_ms),
                                        pdTRUE, NULL, on_broadcast_timer);
    if (gb.broadcast_timer == NULL) {
        return false;
    }

    if (xTimerStart(gb.broadcast_timer, pdMS_TO_TICKS(100)) == pdFAIL){
        return false;
    }

    return true;
}

bool esp_now_broadcast_timer_change_period(uint32_t period_ms) {
    if (xTimerChangePeriod(gb.broadcast_timer, pdMS_TO_TICKS(period_ms), pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}

bool esp_now_broadcast_timer_terminate(void) {
    if (xTimerDelete(gb.broadcast_timer, pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}

static void temp_on_error(ENA_ERROR error) {
    error_code_t err_code;
    if (error == ENA_SEND || error == ENA_SEND_CB_QUEUE || error == ENA_SEND_REPLY) {
        err_code = ERROR_ESP_NOW_SEND;
    } else if (error == ENA_REC || error == ENA_REC_CB_QUEUE) {
        err_code = ERROR_ESP_NOW_RECEIVE;
    } else if (error == ENA_DEVICES_NULL || error == ENA_UNKNOWN_DEV) {
        err_code = ERROR_ESP_NOW_DEVICE;
    } else {
        err_code = ERROR_ESP_UNKNOWN;
    }

    ESP_LOGE(TAG, "ESP NOW ERROR %d", error);
    errors_add(ERROR_TYPE_ESP_NOW, err_code, 200);
}

bool initialize_esp_now(void) {
    esp_err_t status = ESP_OK;
    uint8_t mac_address[] = MCB_MAC;
    ENA_config_t cfg = {
      .stack_depth = 8000,
      .priority = 3,
      .core_id = APP_CPU_NUM,
    };
    status |= ENA_init(mac_address);
    status |= ENA_register_device(&esp_now_broadcast);
    status |= ENA_register_device(&esp_now_pitot);
    // status |= ENA_register_device(&esp_now_vent_valve);
    // status |= ENA_register_device(&esp_now_main_valve);
    // status |= ENA_register_device(&esp_now_tanwa);
    status |= ENA_register_error_handler(temp_on_error);
    status |= ENA_run(&cfg);

    return status == ESP_OK ? true : false;
}