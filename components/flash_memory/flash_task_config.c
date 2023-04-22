#include "flash_task_config.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "rocket_data.h"

#include "flash_task.h"
#include "esp_log.h"

#define TAG "FLASH_C"

bool initialize_flash_memory(void) {
    flash_task_cfg_t cfg = {
        .data_size = sizeof(rocket_data_t),
        .stack_depth = CONFIG_FLASH_TASK_STACK_DEPTH,
        .core_id = CONFIG_FLASH_TASK_CORE_ID,
        .priority = CONFIG_FLASH_TASK_PRIORITY,
        .error_handler_fnc = NULL,
    };
    return FT_init(&cfg);
}