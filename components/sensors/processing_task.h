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
/**
 * @brief data_buffer should be cast to user struct
 *
 */
typedef void (*sensors_process)(void *data_buffer);

typedef struct {
    sensors_read sensors_read_fnc;
    sensors_process sensors_process_fnc;
    size_t data_size;
} sensors_task_cfg_t;

/**
 * @brief Create and run sensors data processing task
 *
 * @param cfg task connfig
 * @return true :D
 * @return false :C
 */
bool sensors_create_task(sensors_task_cfg_t *cfg);

/**
 * @brief Get data collected from sensors
 *
 * @param buffer buffer to store data
 * @param buffer_size buffer size
 * @param timeout_ms data get timeout
 * @return true :D
 * @return false :C
 */
bool sensors_get_data(void *buffer, size_t buffer_size, uint32_t timeout_ms);

/**
 * @brief Replace process function
 *
 * @param fnc new function
 * @param timeout_ms replacing timeout
 * @return true :D
 * @return false :C
 */
bool sensors_change_process_function(sensors_process fnc, uint32_t timeout_ms);


/**
 * @brief Remove process function
 * 
 * @param timeout timeout
 * @return true :D
 * @return false :C
 */
bool sensors_remove_process_function(uint32_t timeout);

#endif