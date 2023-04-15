// Copyright 2022 PWrInSpace Kuba

#ifndef PROCESSING_TASK_H
#define PROCESSING_TASK_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define SENSORS_TASK_PERIOD_MS CONFIG_SENSORS_TASK_PERIOD_MS
#define SENSORS_TASK_PRIORITY CONFIG_SENSORS_TASK_PRIORITY
#define SENSORS_TASK_DEPTH CONFIG_SENSORS_TASK_DEPTH
#define SENSORS_TASK_CPU CONFIG_SENSORS_TASK_CPU
/**
 * @brief data_buffer should be cast to user struct and overwrite by sensor data
 *
 */
typedef void (*sensors_read)(void *data_buffer);
typedef void (*sensors_process)(void *data_buffer);

typedef struct {
    sensors_read sensors_read_fnc;
    sensors_process sensors_process_fnc;
    size_t data_size;
} sensors_task_cfg_t;


bool sensors_create_task(sensors_task_cfg_t *cfg);

bool sensors_get_data(void *buffer, size_t buffer_size, uint32_t timeout_ms);

bool sensors_change_process_function(sensors_process fnc, uint32_t timeout_ms);

#endif