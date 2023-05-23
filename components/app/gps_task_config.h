#ifndef GPS_TASK_CONFIG_H
#define GPS_TASK_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float longitude;
    float latitude;
    float altitude;
    uint8_t sats_in_view;
} gps_positioning_t;

bool gps_initialize(void);

gps_positioning_t gps_get_positioning(void);

#endif