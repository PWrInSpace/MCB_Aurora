// Copyright 2022 PWrInSpace, Kuba
#ifndef ROCKET_DATA_H
#define ROCKET_DATA_H

#include "slave_structs.h"
#include "mcb_structs.h"

typedef struct {
    mcb_data_t mcb;
    vent_valves_data_t vent_valves;
    main_valves_data_t main_valves;
    eth_vent_valve_data_t eth_vent_valve;
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

void rocket_data_update_main_valves(main_valves_data_t *data);

void rocket_data_update_vent_valves(vent_valves_data_t *data);

void rocket_data_update_eth_vent_valve(eth_vent_valve_data_t *data);

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

main_valves_data_t rocket_data_get_main_valves(void);

vent_valves_data_t rocket_data_get_vent_valves(void);

eth_vent_valve_data_t rocket_data_get_eth_vent_valve(void);

ox_main_valve_data_t rocket_data_get_ox_main_valve(void);

recovery_data_t rocket_data_get_recovery(void);

payload_data_t rocket_data_get_payload(void);

tanwa_data_t rocket_data_get_tanwa(void);

pitot_data_t rocket_data_get_pitot(void);

bool rocket_data_woken_up(void);

#endif