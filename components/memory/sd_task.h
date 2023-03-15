// Copyright 2022 PWr in Space, Kuba
#ifndef SD_TASK_H
#define SD_TASK_H

#include <stdint.h>
#include "sdcard.h"
#include "sdkconfig.h"

#define SD_LOG_BUFFER_MAX_SIZE CONFIG_SD_LOG_BUFFER_MAX_SIZE
#define SD_DATA_BUFFER_MAX_SIZE CONFIG_SD_DATA_BUFFER_MAX_SIZE
#define SD_MOUNT_POINT CONFIG_SD_MOUNT_POINT

#define SD_DATA_QUEUE_SIZE CONFIG_SD_DATA_QUEUE_SIZE
#define SD_LOG_QUEUE_SIZE CONFIG_SD_LOG_QUEUE_SIZE

#define SD_DATA_DROP_VALUE CONFIG_SD_DATA_DROP_VALUE

#define SD_PATH_SIZE CONFIG_SD_PATH_SIZE

typedef enum {
    SD_INIT,
    SD_QUEUE_READ,
    SD_WRITE,
    SD_MUTEX,
} SD_TASK_ERR;


typedef void (*error_handler)(SD_TASK_ERR error_code);


typedef struct {
    spi_host_device_t spi_host;
    uint8_t cs_pin;
    char *data_path;
    size_t data_path_size;
    char *log_path;
    size_t log_path_size;

    error_handler error_handler_fnc;
} sd_task_cfg_t;

bool SDT_init(sd_task_cfg_t *cfg);

bool SDT_send_data(char *data, size_t data_size);

bool SDT_send_log(char *log, size_t log_size);

bool SDT_change_data_path(char *new_path, size_t path_size);

void SDT_terminate(void);
#endif
