// Copyright 2022 PWrInSpace, Kuba
#include "init_task.h"

#include "console_config.h"
#include "esp_log.h"
#include "esp_now_config.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "flash_task_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.pb-c.h"
#include "lora_hw_config.h"
#include "lora_task.h"
#include "lora_task_config.h"
#include "processing_task_config.h"
#include "rocket_data.h"
#include "errors_config.h"
#include "sd_card_config.h"
#include "spi.h"
#include "state_machine_config.h"
#include "system_timer_config.h"
#include "utils.h"
#include "mission_timer_config.h"
#include "i2c.h"

#define TAG "INIT"

inline static void CHECK_RESULT_ESP(esp_err_t res, char *message) {
    if (res == ESP_OK) {
        ESP_LOGI(TAG, "Initialized %s", message);
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(res));
    esp_restart();
}

inline static void CHECK_RESULT_BOOL(esp_err_t res, char *message) {
    if (res == true) {
        ESP_LOGI(TAG, "Initialized %s", message);
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    esp_restart();
}

static void TASK_init(void *arg) {
    CHECK_RESULT_BOOL(rocket_data_init(), "data");
    CHECK_RESULT_BOOL(initialize_state_machine(), "STATE_MACHINE");
    CHECK_RESULT_BOOL(i2c_sensors_init(), "i2c sensors");
    CHECK_RESULT_BOOL(i2c_com_init(), "i2c com");
    CHECK_RESULT_BOOL(spi_init(VSPI_HOST, CONFIG_SPI_MOSI, CONFIG_SPI_MISO, CONFIG_SPI_SCK), "SPI");
    CHECK_RESULT_BOOL(initialize_sd_card(), "SD CARD");
    CHECK_RESULT_BOOL(initialize_lora(), "LORA");
    CHECK_RESULT_BOOL(initialize_esp_now(), "ESP_NOW");
    CHECK_RESULT_BOOL(initialize_flash_memory(), "FLASH");
    CHECK_RESULT_BOOL(initialize_processing_task(), "PROCESSING TASK");
    CHECK_RESULT_BOOL(initialize_errors(), "Errors");
    CHECK_RESULT_BOOL(hybrid_mission_timer_init(30000), "Mission timer");
    vTaskDelay(pdMS_TO_TICKS(200));
    CHECK_RESULT_BOOL(initialize_timers(), "TIMERS");
    CHECK_RESULT_BOOL(sys_timer_start(TIMER_SD_DATA, 30, TIMER_TYPE_PERIODIC), "SD TIMER");
    CHECK_RESULT_BOOL(sys_timer_start(TIMER_ESP_NOW_BROADCAST, 500, TIMER_TYPE_PERIODIC),
                      "ESP_NOW_TIMER");
    CHECK_RESULT_BOOL(sys_timer_start(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS, TIMER_TYPE_ONE_SHOT), "SD TIMER");
    CHECK_RESULT_BOOL(sys_timer_start(TIMER_DEBUG, 1000, TIMER_TYPE_PERIODIC), "SD TIMER");
    CHECK_RESULT_ESP(init_console(), "CLI");
    vTaskDelete(NULL);
}

void run_init_task(void) {
    xTaskCreatePinnedToCore(TASK_init, "Init", 8800, NULL, 20, NULL, PRO_CPU_NUM);
}
