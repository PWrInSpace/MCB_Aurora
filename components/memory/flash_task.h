// Copyright 2022 PWr in Space, Kuba

#ifndef FLASH_TASK_H
#define FLASH_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define DROP_VALUE_RATIO 3 / 4

typedef enum {
    FT_INIT_ERROR,
    FT_FILE_OPEN,
    FT_FILE_FULL,
    FT_WRITE,
} FT_ERROR_CODE;

typedef bool (*FT_error_handler) (FT_ERROR_CODE error_code);

typedef bool (*FT_can_write) (void);
typedef bool (*FT_erase_condition) (void);
typedef bool (*FT_terminate_codnition) (void);

typedef struct {
    size_t data_size;
    size_t queue_size;

    FT_terminate_codnition terminate_codnition_fnc;
    FT_can_write can_write_to_flash_fnc;
    FT_erase_condition erase_codnition_fnc;
    FT_error_handler error_handler_fnc;
} flash_task_cfg_t;


bool init_flash_task(flash_task_cfg_t *cfg);
bool send_data_to_flash_task(void *data);
#endif
