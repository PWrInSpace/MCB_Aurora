#include "sd_card_config.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "sd_task.h"
#include "esp_log.h"
#include "init_task.h"
#include "gen_pysd.h"
#include "esp_timer.h"
#include "spi.h"

#define TAG "SD_C"

extern SemaphoreHandle_t mutex_spi;

static size_t convert_data_to_frame(char *buf, size_t buf_size, void* data, size_t size) {
    rocket_data_t* rocket_data = (rocket_data_t*)data;
    // ESP_LOGI(TAG, "On timer %d, %d", rocket_data->mcb.uptime, rocket_data->mcb.state);
    return pysd_create_sd_frame(buf, buf_size, *rocket_data, true);
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
        .spi_mutex = mutex_spi,
    };

    return SDT_init(&cfg);
}