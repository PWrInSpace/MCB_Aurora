#ifndef GPS_TASK_CONFIG_H
#define GPS_TASK_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float longitude;
    float latitude;
    float altitude;
    uint8_t sats_in_view;
    uint8_t fix_type;
} gps_positioning_t;

/**
 * @brief Initialze gps task
 * 
 * @return true :D
 * @return false :C
 */
bool initialize_gps(void);

/**
 * @brief Get vehicle position
 *
 * @return gps_positioning_t position
 */
gps_positioning_t gps_get_positioning(void);

#endif