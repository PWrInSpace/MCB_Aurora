#include "flash_task_config.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "rocket_data.h"
#include "errors_config.h"
#include "flash_task.h"
#include "esp_log.h"

#define TAG "FLASH_C"

static bool on_error(FT_ERROR_CODE error) {
    error_code_t err_code;
    if (error == FT_WRITE) {
        err_code = ERROR_FLASH_WRITE;
    } else if (error == FT_FILE_FULL) {
        err_code = ERROR_FLASH_FULL;
    } else if (error == FT_FILE_OPEN) {
        err_code = ERROR_FLASH_FILE;
    } else if (error == FT_FORMAT) {
        err_code = ERROR_FLASH_FORMAT;
    } else {
        err_code = ERROR_MEMORY_UNKNOWN;
    }

    ESP_LOGE(TAG, "!!! FLASH ERROR CODE: %d !!!", err_code);
    errors_add(ERROR_TYPE_MEMORY, err_code, 200);
    return true;
}

bool initialize_flash_memory(void) {
    flash_task_cfg_t cfg = {
        .data_size = sizeof(rocket_data_t),
        .stack_depth = CONFIG_FLASH_TASK_STACK_DEPTH,
        .core_id = CONFIG_FLASH_TASK_CORE_ID,
        .priority = CONFIG_FLASH_TASK_PRIORITY,
        .error_handler_fnc = on_error,
    };
    return FT_init(&cfg);
}