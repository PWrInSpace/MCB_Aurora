#include "sd_card_config.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "sd_task.h"
#include "esp_log.h"

#define TAG "SD_C"

static struct {
    TimerHandle_t sd_data_timer;
    char sd_data_buffer[CONFIG_SD_DATA_BUFFER_MAX_SIZE];
    size_t sd_buffer_data_size;
} gb = {
    .sd_data_timer = 0,
    .sd_data_buffer = {0},
    .sd_buffer_data_size = 0,
};

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
        .error_handler_fnc = NULL,
    };

    return SDT_init(&cfg);
}

static void on_sd_data_timer(TimerHandle_t xTimer) {
    // create sd string
    static int i = 0;
    i++;
    ESP_LOGI(TAG, "Sending data to sd");
    gb.sd_buffer_data_size = snprintf(gb.sd_data_buffer, sizeof(gb.sd_data_buffer), "Hello world :D %d\n", i);
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