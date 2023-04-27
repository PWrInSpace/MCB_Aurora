#include "sd_card_config.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "errors_config.h"
#include "sd_task.h"
#include "esp_log.h"
#include "gen_pysd.h"

#define TAG "SD_C"

static struct {
    TimerHandle_t sd_data_timer;
    char sd_data_buffer[CONFIG_SD_DATA_BUFFER_MAX_SIZE];
    size_t sd_buffer_data_size;
} gb = {
    .sd_data_timer = NULL,
    .sd_data_buffer = {0},
    .sd_buffer_data_size = 0,
};

void on_error(SD_TASK_ERR error) {
    error_code_t err_code;
    if (error == SD_WRITE || error == SD_QUEUE_READ) {
        err_code = ERROR_SD_WRITE;
    } else {
        err_code = ERROR_MEMORY_UNKNOWN;
    }

    ESP_LOGE(TAG, "SD error %d", error);
    errors_add(ERROR_TYPE_MEMORY, err_code, 200);
}

bool initialize_sd_card(void) {
    ESP_LOGI(TAG, "Initializing sd task");
    sd_task_cfg_t cfg = {
        .cs_pin = CONFIG_SD_CS,
        .data_path = "data",
        .data_path_size = 9,
        .spi_host = VSPI_HOST,
        .log_path = "log",
        .log_path_size = 5,
        .stack_depth = CONFIG_SD_TASK_STACK_DEPTH,
        .priority = CONFIG_SD_TASK_PRIORITY,
        .core_id = CONFIG_SD_TASK_CORE_ID,
        .error_handler_fnc = on_error,
    };

    return SDT_init(&cfg);
}

static void on_sd_data_timer(TimerHandle_t xTimer) {
    rocket_data_t test = rocket_data_get();
    gb.sd_buffer_data_size = pysd_create_sd_frame(gb.sd_data_buffer,
                                                sizeof(gb.sd_data_buffer),
                                                test,
                                                true);
    ESP_LOGI(TAG, "Writing data to sd -> %d", gb.sd_buffer_data_size);
    SDT_send_data(gb.sd_data_buffer, gb.sd_buffer_data_size);
}

bool sd_data_timer_start(uint32_t period_ms) {
    ESP_LOGI(TAG, "Creating sd data timer");
    gb.sd_data_timer = xTimerCreate("sd data", pdMS_TO_TICKS(period_ms),
                                    pdTRUE, NULL, on_sd_data_timer);

    if (gb.sd_data_timer == NULL) {
        return false;
    }

    if (xTimerStart(gb.sd_data_timer, pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}

bool sd_data_timer_change_period(uint32_t period_ms) {
    if (xTimerChangePeriod(gb.sd_data_timer, pdMS_TO_TICKS(period_ms), pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}

bool sd_data_timer_terminate(void) {
    if (xTimerDelete(gb.sd_data_timer, pdMS_TO_TICKS(100)) == pdFAIL) {
        return false;
    }

    return true;
}