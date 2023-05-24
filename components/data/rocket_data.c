// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "rocket_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "errors_config.h"

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

void rocket_data_update_main_valve(main_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.main_valve, data, sizeof(gb.rocket_data.main_valve));
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