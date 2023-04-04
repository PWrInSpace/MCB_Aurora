#ifndef MCB_STRUCTS_H
#define MCB_STRUCTS_H

#include <stdint.h>

typedef struct {
    uint8_t state;
    uint32_t uptime;
    int32_t flight_time;
    int32_t last_rx_time;
    int sensor_placeholder;
} mcb_data_t;

#endif