// Copyright 2022 PWrInSpace, Kuba
#include "rocket_data.h"

#include <memory.h>

#include "errors_config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#define TAG "DATA"

static struct {
    rocket_data_t rocket_data;
    SemaphoreHandle_t data_mutex;
} gb;

bool rocket_data_init(void) {
    memset(&gb.rocket_data, 0, sizeof(gb.rocket_data));
    gb.data_mutex = NULL;
    gb.data_mutex = xSemaphoreCreateMutex();
    if (gb.data_mutex == NULL) {
        return false;
    }
    /* initialize mutex */

    return true;
}

void rocket_data_update_connected_dev(esp_now_connected_devices_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.connected_dev, data, sizeof(gb.rocket_data.connected_dev));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_n2_vent_valve(n2_vent_valve_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.n2_vent_valve, data, sizeof(gb.rocket_data.n2_vent_valve));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_eth_vent_valve(eth_vent_valve_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.eth_vent_valve, data, sizeof(gb.rocket_data.eth_vent_valve));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_ox_vent_eth_main_valves(ox_vent_eth_main_valves_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.ox_vent_eth_main_valves, data,
           sizeof(gb.rocket_data.ox_vent_eth_main_valves));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_ox_main_valve(ox_main_valve_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.ox_main_valve, data, sizeof(gb.rocket_data.ox_main_valve));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_recovery(recovery_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.recovery, data, sizeof(gb.rocket_data.recovery));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_mcb(mcb_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.mcb, data, sizeof(gb.rocket_data.mcb));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_payload(payload_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }

    memcpy(&gb.rocket_data.payload, data, sizeof(gb.rocket_data.payload));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_pitot(pitot_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.pitot, data, sizeof(gb.rocket_data.pitot));
    ESP_LOGD(TAG, "Pitot updated - waken_up:%d vbat:%f static:%f dynamic:%f temp:%f",
             (int)data->waken_up, data->vbat, data->static_press, data->dynamic_press,
             data->temperature);
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_tanwa(tanwa_data_t *data) {
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return;
    }
    memcpy(&gb.rocket_data.tanwa, data, sizeof(gb.rocket_data.tanwa));
    xSemaphoreGive(gb.data_mutex);
}

static void update_errors(rocket_data_t *data) {
    error_data_t errors[MAX_NUMBER_OF_ERRORS];
    errors_get_all(errors, sizeof(errors));

    data->error_last_exception = errors[ERROR_TYPE_LAST_EXCEPTION];
    data->error_recovery = errors[ERROR_TYPE_RECOVERY];
    data->error_esp_now = errors[ERROR_TYPE_ESP_NOW];
    data->error_memory = errors[ERROR_TYPE_MEMORY];
    data->error_mcb = errors[ERROR_TYPE_MCB];
    data->error_sensors = errors[ERROR_TYPE_SENSORS];
}

rocket_data_t rocket_data_get(void) {
    rocket_data_t tmp = {0};

    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data;
    xSemaphoreGive(gb.data_mutex);
    update_errors(&tmp);
    mcb_update_struct(&tmp.mcb);

    return tmp;
}

void rocket_data_copy(rocket_data_t *out) {
    if (out == NULL) return;
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        memset(out, 0, sizeof(*out));
        return;
    }
    memcpy(out, &gb.rocket_data, sizeof(*out));
    xSemaphoreGive(gb.data_mutex);
    update_errors(out);
    mcb_update_struct(&out->mcb);
}

mcb_data_t rocket_data_get_mcb(void) {
    mcb_data_t tmp;
    mcb_update_struct(&tmp);
    return tmp;
}

n2_vent_valve_data_t rocket_data_get_n2_vent_valve(void) {
    n2_vent_valve_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.n2_vent_valve;

    xSemaphoreGive(gb.data_mutex);

    return tmp;
}

eth_vent_valve_data_t rocket_data_get_eth_vent_valve(void) {
    eth_vent_valve_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.eth_vent_valve;

    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

ox_vent_eth_main_valves_data_t rocket_data_get_ox_vent_eth_main_valves(void) {
    ox_vent_eth_main_valves_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.ox_vent_eth_main_valves;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

ox_main_valve_data_t rocket_data_get_ox_main_valve(void) {
    ox_main_valve_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.ox_main_valve;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

recovery_data_t rocket_data_get_recovery(void) {
    recovery_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.recovery;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

payload_data_t rocket_data_get_payload(void) {
    payload_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.payload;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

tanwa_data_t rocket_data_get_tanwa(void) {
    tanwa_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.tanwa;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

pitot_data_t rocket_data_get_pitot(void) {
    pitot_data_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.pitot;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

bool rocket_data_woken_up(void) {
    bool result = false;
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return false;
    }
    if (gb.rocket_data.n2_vent_valve.waken_up == false ||
        gb.rocket_data.eth_vent_valve.waken_up == false ||
        gb.rocket_data.ox_vent_eth_main_valves.waken_up == false ||
        gb.rocket_data.ox_main_valve.waken_up == false) {
        result = false;
    } else {
        result = true;
    }
    xSemaphoreGive(gb.data_mutex);

    return result;
}

esp_now_connected_devices_t rocket_data_get_connected_dev(void) {
    esp_now_connected_devices_t tmp = {0};
    if (xSemaphoreTake(gb.data_mutex, 1000) != pdTRUE) {
        ESP_LOGE(TAG, "Data semaphore errror :C");
        return tmp;
    }
    tmp = gb.rocket_data.connected_dev;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}