// Copyright 2022 PWrInSpace, Kuba
#ifndef ROCKET_DATA_H
#define ROCKET_DATA_H

#include "slave_structs.h"
#include "mcb_structs.h"

typedef struct {
    mcb_data_t mcb;
    eth_vent_valve_data_t eth_vent_valve;
    n2_vent_valve_data_t n2_vent_valve;
    ox_vent_eth_main_valves_data_t ox_vent_eth_main_valves;
    ox_main_valve_data_t ox_main_valve;
    recovery_data_t recovery;
    payload_data_t payload;
    tanwa_data_t tanwa;
    pitot_data_t pitot;
    esp_now_connected_devices_t connected_dev;
    uint32_t error_esp_now;
    uint32_t error_memory;
    uint32_t error_mcb;
    uint32_t error_sensors;
    uint32_t error_recovery;
    uint32_t error_last_exception;
} rocket_data_t;

bool rocket_data_init(void);

void rocket_data_update_connected_dev(esp_now_connected_devices_t *data);

void rocket_data_update_n2_vent_valve(n2_vent_valve_data_t *data);

void rocket_data_update_eth_vent_valve(eth_vent_valve_data_t *data);

void rocket_data_update_ox_vent_eth_main_valves(ox_vent_eth_main_valves_data_t *data);

void rocket_data_update_ox_main_valve(ox_main_valve_data_t *data);

void rocket_data_update_recovery(recovery_data_t *data);

void rocket_data_update_mcb(mcb_data_t *data);

void rocket_data_update_payload(payload_data_t *data);

void rocket_data_update_tanwa(tanwa_data_t *data);

void rocket_data_update_pitot(pitot_data_t *data);

rocket_data_t rocket_data_get(void);

/* Copy current rocket data into provided buffer under mutex. */
void rocket_data_copy(rocket_data_t *out);

mcb_data_t rocket_data_get_mcb(void);

n2_vent_valve_data_t rocket_data_get_n2_vent_valve(void);

eth_vent_valve_data_t rocket_data_get_eth_vent_valve(void);

ox_vent_eth_main_valves_data_t rocket_data_get_ox_vent_eth_main_valves(void);

ox_main_valve_data_t rocket_data_get_ox_main_valve(void);

recovery_data_t rocket_data_get_recovery(void);

payload_data_t rocket_data_get_payload(void);

tanwa_data_t rocket_data_get_tanwa(void);

pitot_data_t rocket_data_get_pitot(void);

bool rocket_data_woken_up(void);

esp_now_connected_devices_t rocket_data_get_connected_dev(void);

#endif