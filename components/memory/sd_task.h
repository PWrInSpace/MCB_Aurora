// Copyright 2022 PWr in Space, Kuba
#ifndef SD_TASK_H
#define SD_TASK_H

#include <stdint.h>

// #include "sdkconfig"


// #define SD_QUEUE_SIZE CONFIG_SD_QUEUE_SIZE
// #define SD_DATA_DROP_VALUE CONFIG_DATA_DROP_VALUE

// typedef enum {
//     SD_QUEUE_READ,
//     SD_CREATE_CSV_STRING,
// } SD_TASK_ERR;


// typedef void (*error_handler)(SD_TASK_ERR error_code);
// typedef bool (*task_terminate_condition)(void);

// typedef size_t (*create_csv_string)(char *buffer, size_t buff_size,
//                  void *data,size_t data_size);
// typedef size_t (*create_csv_header)(char *buffer);

// typedef struct {
//     char *path;
//     size_t path_size;
//     size_t data_buffer_size;

//     uint8_t cs_pin;
//     spi_host_device_t spi_host;

//     size_t queue_data_size;

//     create_csv_header create_csv_header_fnc;
//     create_csv_string create_csv_string_fnc;
//     task_terminate_condition task_terminate_condition_fnc;
//     error_handler error_handler_fnc;
// } sd_task_cfg_t;

// bool init_sd_task(sd_task_cfg_t *cfg);
// // bool sendDataToSdTask(DataToSave *data);

// bool change_path_to_file(char *new_path);

#endif
