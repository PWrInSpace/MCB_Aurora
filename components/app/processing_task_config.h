// Copyright 2022 PWrInSpace
#ifndef PROCESSING_TASK_CONFIG_H
#define PROCESSING_TASK_CONFIG_H

#include "processing_task.h"
#include "slave_structs.h"



typedef struct {
    float acc_x;
    float acc_y;
    float acc_z;
    float gyr_x;
    float gyr_y;
    float gyr_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float temperature;
    float pressure;
    float altitude;
    float velocity;
    float yaw;
    float pitch;
    float roll;
} sensors_data_t;

/**
 * @brief Initialize configured processing task
 *
 * @return true :D
 * @return false :C
 */
bool initialize_processing_task(void);
#endif