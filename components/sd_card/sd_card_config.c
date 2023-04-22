#include "sd_card_config.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "sd_task.h"
#include "esp_log.h"
#include "init_task.h"
#include "gen_pysd.h"
#include "esp_timer.h"

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

static size_t convert_data_to_frame(char *buf, size_t buf_size, void* data, size_t size) {
    rocket_data_t* rocket_data = (rocket_data_t*)data;
    // ESP_LOGI(TAG, "On timer %d, %d", rocket_data->mcb.uptime, rocket_data->mcb.state);
    return pysd_create_sd_frame(buf, buf_size, *rocket_data, true);
}

static void on_sd_data_timer(void *arg) {
    // UBaseType_t uxHighWaterMark;
    // /* Inspect our own high water mark on entering the task. */
    // uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // ESP_LOGI(TAG, "On timer %d", uxHighWaterMark * 4);
    // // SDT_send_data(&test, sizeof(test));
    // esp_event_post_to(event_get_handle(), TASK_EVENTS, APOGEE_EVENT,
    //                     (void *)NULL, 0, 0);

    // ESP_LOGI(TAG, "ON SD TIMER");
    rocket_data_t test = rocket_data_get();
    UBaseType_t uxHighWaterMark;
    /* Inspect our own high water mark on entering the task. */
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // ESP_LOGI(TAG, "DDDD %d %d", uxHighWaterMark * 4, test.mcb.uptime);
    SDT_send_data(&test, sizeof(test));
}

bool initialize_sd_card(void) {
    esp_timer_init();
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
        .data_size = sizeof(rocket_data_t),
        .create_sd_frame_fnc = convert_data_to_frame,
    };

    return SDT_init(&cfg);
}

bool sd_data_timer_start(uint32_t period_ms) {
    ESP_LOGI(TAG, "Creating sd data timer");
    // gb.sd_data_timer = xTimerCreate("sd data", pdMS_TO_TICKS(period_ms),
    //                                 pdTRUE, NULL, on_sd_data_timer);

    // if (gb.sd_data_timer == NULL) {
    //     return false;
    // }

    // if (xTimerStart(gb.sd_data_timer, pdMS_TO_TICKS(100)) == pdFAIL) {
    //     return false;
    // }

    esp_timer_create_args_t args = {
        .callback = on_sd_data_timer,
        .arg = NULL,
    };
    esp_timer_handle_t handle;
    esp_timer_create(&args, &handle);
    esp_timer_start_periodic(handle, 20 * 1000);
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