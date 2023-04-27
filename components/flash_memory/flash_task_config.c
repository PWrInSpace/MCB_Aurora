#include "flash_task_config.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "rocket_data.h"
#include "errors_config.h"
#include "flash_task.h"
#include "esp_log.h"

#define TAG "FLASH_C"

static struct {
    TimerHandle_t flash_timer;
} gb = {
    .flash_timer = NULL,
};

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

static void on_flash_data_timer(TimerHandle_t xTimer) {
    ESP_LOGI(TAG, "Sending data flash");
    rocket_data_t data = rocket_data_get();
    FT_send_data(&data);
}

bool flash_data_timer_start(uint32_t period_ms) {
    ESP_LOGI(TAG, "Creating flash timer");
    gb.flash_timer = xTimerCreate("flash data", pdMS_TO_TICKS(period_ms),
                                    pdTRUE, NULL, on_flash_data_timer);

    if (gb.flash_timer == NULL) {
        return false;
    }

    if (xTimerStart(gb.flash_timer, pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}

bool flash_data_timer_change_period(uint32_t period_ms) {
    if (xTimerChangePeriod(gb.flash_timer, pdMS_TO_TICKS(period_ms), pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}

bool flash_data_timer_terminate(void) {
    if (xTimerDelete(gb.flash_timer, pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}