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
#include "lora.pb-c.h"
#include "esp_timer.h"
#include "console_config.h"
#include "flash_task.h"
#include "sd_task.h"
#include "lora_task_config.h"

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


// change to error from error api
static void temp_on_error(ENA_ERROR error) {
    ESP_LOGE(TAG, "ESP NOW ERROR %d", error);
}

static bool init_esp_now(void) {
    esp_err_t status = ESP_OK;
    uint8_t mac_address[] = MCB_MAC;
    ENA_config_t cfg = {
      .stack_depth = 8000,
      .priority = 3,
      .core_id = APP_CPU_NUM,
    };
    status |= ENA_init(mac_address);
    status |= ENA_register_device(&esp_now_broadcast);
    status |= ENA_register_device(&esp_now_pitot);
    // status |= ENA_register_device(&esp_now_vent_valve);
    // status |= ENA_register_device(&esp_now_main_valve);
    // status |= ENA_register_device(&esp_now_tanwa);
    status |= ENA_register_error_handler(temp_on_error);
    status |= ENA_run(&cfg);

    return status == ESP_OK ? true : false;
}


static bool init_spi(void) {
    return spi_init(VSPI_HOST, 23, 19, 18);
}

static bool can_write() {
    return true;
}


static bool init_flash_task() {
    flash_task_cfg_t cfg = {
        .data_size = 120,
        .stack_depth = 8000,
        .core_id = APP_CPU_NUM,
        .priority = 4,
        .error_handler_fnc = NULL,
        .can_write_to_flash_fnc = can_write,
    };
    return FT_init(&cfg);
}

static bool init_sd_card() {
    sd_task_cfg_t cfg = {
        .cs_pin = 5,
        .data_path = "data",
        .data_path_size = 9,
        .spi_host = VSPI_HOST,
        .log_path = "log",
        .log_path_size = 5,
        .stack_depth = 8000,
        .priority = 0,
        .core_id = APP_CPU_NUM,
        .error_handler_fnc = NULL,
    };

    return SDT_init(&cfg);
}


static void TASK_init(void *arg) {
    CHECK_RESULT_BOOL(init_esp_now(), "ESP_NOW");
    CHECK_RESULT_BOOL(initialize_state_machine(), "STATE_MACHINE");
    CHECK_RESULT_BOOL(init_spi(), "SPI");
    CHECK_RESULT_BOOL(init_sd_card(), "SD CARD");
    CHECK_RESULT_BOOL(initialize_lora(), "LORA");
    CHECK_RESULT_BOOL(init_flash_task(), "FLASH");
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
