// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "rocket_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

static struct {
    main_valve_data_t main_valve;
    vent_valve_data_t vent_valve;
    recovery_data_t recovery;
    SemaphoreHandle_t data_mutex;
} gb;

bool rocket_data_init(void) {
    memset(&gb, 0, sizeof(gb));
    gb.data_mutex = NULL;
    gb.data_mutex = xSemaphoreCreateMutex();
    if (gb.data_mutex == NULL) {
        return false;
    }

    return true;
}

void rocket_data_update_main_valve(main_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.main_valve, &data, sizeof(gb.main_valve));
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_vent_valve(vent_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.vent_valve, &data, sizeof(gb.vent_valve));
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_recovery(recovery_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.recovery, &data, sizeof(gb.recovery));
    xSemaphoreGive(gb.data_mutex);
}

main_valve_data_t rocket_data_get_main_valve(void) {
    main_valve_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.main_valve;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

vent_valve_data_t rocket_data_get_vent_valve(void) {
    vent_valve_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.vent_valve;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

recovery_data_t rocket_data_get_recovery(void) {
    recovery_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.recovery;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}