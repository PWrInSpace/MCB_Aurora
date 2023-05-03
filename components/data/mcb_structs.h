// Copyright 2022 PWrInSpace, Kuba
#ifndef MCB_STRUCTS_H
#define MCB_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t state;
    uint32_t uptime;
    int32_t flight_time;
    uint32_t disconnect_timer;
    float battery_voltage;
    int sensor_placeholder;
} mcb_data_t;

bool mcb_update_struct(mcb_data_t *mcb);

#endif