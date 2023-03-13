// Copyright 2022 PWr in Space, Kuba
#ifndef SD_TASK_H
#define SD_TASK_H

#include <stdint.h>

// #include "sdkconfig"


#define SD_QUEUE_SIZE CONFIG_SD_QUEUE_SIZE
#define SD_DATA_DROP_VALUE CONFIG_DATA_DROP_VALUE

typedef enum {
    SD_QUEUE_READ,
    SD_CREATE_CSV_STRING,
} SD_TASK_ERR;


typedef void (*error_handler)(SD_TASK_ERR error_code);


typedef struct {
    uint8_t cs_pin;
    spi_host_device_t spi_host;

    char *path;
    size_t path_size;

    size_t queue_data_size;

    error_handler error_handler_fnc;
} sd_task_cfg_t;

bool init_sd_task(sd_task_cfg_t *cfg);

bool send_data_to_sd_task(void *data);

bool send_log_to_sd_task(char *log, size_t log_size);

bool change_path_to_file(char *new_path);
#endif
