// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "rocket_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

static struct {
    main_valves_data_t main_valves;
    vent_valves_data_t vent_valves;
    eth_vent_valve_data_t eth_vent_valve;
    ox_main_valve_data_t ox_main_valve;
    recovery_data_t recovery;
    payload_data_t payload;
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



void rocket_data_update_main_valves(main_valves_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.main_valves, data, sizeof(gb.main_valves));
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_vent_valves(vent_valves_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.vent_valves, data, sizeof(gb.vent_valves));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_ox_main_valve(ox_main_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.ox_main_valve, data, sizeof(gb.ox_main_valve));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_eth_vent_valve(eth_vent_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.eth_vent_valve, data, sizeof(gb.eth_vent_valve));
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_recovery(recovery_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.recovery, data, sizeof(gb.recovery));
    xSemaphoreGive(gb.data_mutex);
}


main_valves_data_t rocket_data_get_main_valves(void) {
    main_valves_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.main_valves;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

vent_valves_data_t rocket_data_get_vent_valves(void) {
    vent_valves_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.vent_valves;
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
