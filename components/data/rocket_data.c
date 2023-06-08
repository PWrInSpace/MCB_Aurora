// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "rocket_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "errors_config.h"

#include "esp_log.h"
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

    return true;
}

void rocket_data_update_connected_dev(esp_now_connected_devices_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.connected_dev, data, sizeof(gb.rocket_data.connected_dev));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_main_valve(main_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.main_valve, data, sizeof(gb.rocket_data.main_valve));
    ESP_LOGD(TAG, "Main valve:");
    ESP_LOGD(TAG, "waken_up %d", gb.rocket_data.main_valve.waken_up);
    ESP_LOGD(TAG, "vbat %d", gb.rocket_data.main_valve.battery_voltage);
    ESP_LOGD(TAG, "valve state %d", gb.rocket_data.main_valve.valve_state);
    ESP_LOGD(TAG, "thermocouple %d", gb.rocket_data.main_valve.thermocouple1);
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_vent_valve(vent_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.vent_valve, data, sizeof(gb.rocket_data.vent_valve));
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_recovery(recovery_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.recovery, data, sizeof(gb.rocket_data.recovery));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_mcb(mcb_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.mcb, data, sizeof(gb.rocket_data.mcb));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_payload(payload_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.payload, data, sizeof(gb.rocket_data.payload));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_pitot(pitot_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.pitot, data, sizeof(gb.rocket_data.pitot));
    ESP_LOGD(TAG, "Pitot:");
    ESP_LOGD(TAG, "waken_up %d", gb.rocket_data.pitot.waken_up);
    ESP_LOGD(TAG, "vbat %f", gb.rocket_data.pitot.vbat);
    ESP_LOGD(TAG, "static press %f", gb.rocket_data.pitot.static_press);
    ESP_LOGD(TAG, "dynamic press %f", gb.rocket_data.pitot.dynamic_press);
    ESP_LOGD(TAG, "temperature %f", gb.rocket_data.pitot.temperature);
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_tanwa(tanwa_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.tanwa, data, sizeof(gb.rocket_data.tanwa));
    ESP_LOGD(TAG, "Tanwa:");
    ESP_LOGD(TAG, "state %d", gb.rocket_data.tanwa.tanWaState);
    ESP_LOGD(TAG, "vbat %f", gb.rocket_data.tanwa.vbat);
    ESP_LOGD(TAG, "solenoid fill %d", gb.rocket_data.tanwa.solenoid_fill);
    ESP_LOGD(TAG, "solenoid depr %d", gb.rocket_data.tanwa.solenoid_depr);
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
    rocket_data_t tmp;

    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data;
    xSemaphoreGive(gb.data_mutex);
    update_errors(&tmp);
    mcb_update_struct(&tmp.mcb);

    return tmp;
}

mcb_data_t rocket_data_get_mcb(void) {
    mcb_data_t tmp;
    mcb_update_struct(&tmp);
    return tmp;
}

main_valve_data_t rocket_data_get_main_valve(void) {
    main_valve_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.main_valve;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

vent_valve_data_t rocket_data_get_vent_valve(void) {
    vent_valve_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.vent_valve;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

recovery_data_t rocket_data_get_recovery(void) {
    recovery_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.recovery;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

payload_data_t rocket_data_get_payload(void) {
    payload_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.payload;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

tanwa_data_t rocket_data_get_tanwa(void) {
    tanwa_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.tanwa;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

pitot_data_t rocket_data_get_pitot(void) {
    pitot_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.pitot;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

bool rocket_data_woken_up(void) {
    bool result = false;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    if (gb.rocket_data.main_valve.waken_up == false ||
        gb.rocket_data.vent_valve.waken_up == false) {
            result = false;
    } else {
        result = true;
    }
    xSemaphoreGive(gb.data_mutex);

    return result;
}