// Copyright 2022 PWr in Space, Kuba

#ifndef FLASH_TASK_H
#define FLASH_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"

#define FLASH_DROP_VALUE CONFIG_FLASH_DATA_DROP_VALUE
#define FLASH_QUEUE_SIZE CONFIG_FLASH_QUEUE_SIZE

typedef enum {
    FT_INIT_ERROR,
    FT_FILE_OPEN,
    FT_FILE_FULL,
    FT_FORMAT,
    FT_WRITE,
} FT_ERROR_CODE;

typedef bool (*FT_error_handler) (FT_ERROR_CODE error_code);

typedef struct {
    size_t data_size;   // size of transmitting data

    uint32_t stack_depth;
    BaseType_t core_id;
    UBaseType_t priority;

    FT_error_handler error_handler_fnc;
} flash_task_cfg_t;

/**
 * @brief Initialzie flash task
 * 
 * @param cfg pointer to flash config
 * @return true :)
 * @return false :C
 */
bool FT_init(flash_task_cfg_t *cfg);

/**
 * @brief Send data to flash task, only works when task was initialized and
 * can_write function return true
 * 
 * @param data pointer to data to save [ ONLY CONST SIZE DATA WITH SIZE GIVEN IN CONFIG]
 * @return true :)
 * @return false :C
 */
bool FT_send_data(void *data);

/**
 * @brief Erase flash and run task loop, enable flash writting
 * 
 */
void FT_erase_and_run_loop(void);

/**
 * @brief Terminate flash task
 * 
 */
void FT_terminate_task(void);

#endif
