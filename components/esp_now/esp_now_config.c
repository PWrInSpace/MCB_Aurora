// Copyright 2022 PWrInSpace, Kuba
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_now_config.h"
#include "esp_log.h"
#include "esp_now_api.h"
#include "rocket_data.h"
#include "state_machine.h"

#define TAG "ENC"

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
    // if (size == sizeof())
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

static void temp_on_error(ENA_ERROR error) {
    ESP_LOGE(TAG, "ESP NOW ERROR %d", error);
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