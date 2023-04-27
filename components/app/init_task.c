// Copyright 2022 PWrInSpace, Kuba
#include "init_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_now_config.h"
#include "esp_system.h"
#include "state_machine_config.h"
#include "lora_task.h"
#include "lora_hw_config.h"
#include "spi.h"
#include "utils.h"
#include "processing_task_config.h"
#include "lora.pb-c.h"
#include "esp_timer.h"
#include "console_config.h"
#include "lora_task_config.h"
#include "sd_card_config.h"
#include "flash_task_config.h"
#include "esp_now_config.h"
#include "rocket_data.h"
#include "errors_config.h"


#define TAG "INIT"

inline static void CHECK_RESULT_ESP(esp_err_t res, char* message) {
    if (res == ESP_OK) {
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(res));
    esp_restart();
}

inline static void CHECK_RESULT_BOOL(esp_err_t res, char *message) {
    if (res == true) {
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    esp_restart();
}

static void TASK_init(void *arg) {
    CHECK_RESULT_BOOL(rocket_data_init(), "data");
    CHECK_RESULT_BOOL(initialize_esp_now(), "ESP_NOW");
    CHECK_RESULT_BOOL(esp_now_broadcast_timer_start(500), "ESP NOW BROADCAST");
    CHECK_RESULT_BOOL(initialize_state_machine(), "STATE_MACHINE");
    CHECK_RESULT_BOOL(spi_init(VSPI_HOST, CONFIG_SPI_MOSI, CONFIG_SPI_MISO, CONFIG_SPI_SCK), "SPI");
    CHECK_RESULT_BOOL(initialize_sd_card(), "SD CARD");
    CHECK_RESULT_BOOL(sd_data_timer_start(1000), "SD CARD");
    CHECK_RESULT_BOOL(initialize_lora(), "LORA");
    CHECK_RESULT_BOOL(initialize_flash_memory(), "FLASH");
    CHECK_RESULT_BOOL(initialize_processing_task(), "PROCESSING TASK");
    CHECK_RESULT_BOOL(initialize_errors(), "Errors");
    CHECK_RESULT_ESP(init_console(), "CLI");
    vTaskDelete(NULL);
}

void run_init_task(void) {
    xTaskCreatePinnedToCore(
        TASK_init,
        "Init",
        8800,
        NULL,
        20,
        NULL,
        PRO_CPU_NUM);
}
