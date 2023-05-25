// Copyright 2022 PWrInSpace
#include "processing_task_config.h"
#include "esp_log.h"
#include "errors_config.h"
#include "rocket_data.h"

#define TAG "SENSORS_CFG"

static void sensors_read_data(void *data_buffer) {
    sensors_data_t *data = (sensors_data_t*)data_buffer;
    data->test_float += 1.12123123;
}

bool initialize_processing_task(void) {
    sensors_task_cfg_t cfg = {
        .sensors_read_fnc = sensors_read_data,
        .sensors_process_fnc = NULL,
        .data_size = sizeof(sensors_data_t),
    };
    return sensors_create_task(&cfg);
}