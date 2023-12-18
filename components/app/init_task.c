// Copyright 2022 PWrInSpace, Kuba
#include "init_task.h"

#include "buzzer_pwm.h"
#include "console_config.h"
#include "errors_config.h"
#include "esp_log.h"
#include "esp_now_config.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "flash_task_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_expander.h"
#include "gps_task_config.h"
#include "i2c.h"
#include "lora.pb-c.h"
#include "lora_hw_config.h"
#include "lora_task.h"
#include "lora_task_config.h"
#include "mission_timer_config.h"
#include "processing_task_config.h"
#include "recovery_task_config.h"
#include "rocket_data.h"
#include "sd_card_config.h"
#include "settings_mem.h"
#include "spi.h"
#include "state_machine_config.h"
#include "system_timer_config.h"
#include "uart.h"
#include "utils.h"
#include "vbat_wrapper.h"

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
    CHECK_RESULT_ESP(settings_init(), "Change state");
    Settings settings = settings_get_all();

    CHECK_RESULT_BOOL(rocket_data_init(), "data");
    CHECK_RESULT_BOOL(initialize_errors(), "Errors");
    CHECK_RESULT_BOOL(hybrid_mission_timer_init(settings.countdownTime), "Mission timer");
    CHECK_RESULT_BOOL(vbat_init(), "VBAT MEASUREMENT");
    CHECK_RESULT_BOOL(buzzer_init(), "Buzzer");

    CHECK_RESULT_BOOL(i2c_sensors_init(), "i2c sensors");
    CHECK_RESULT_BOOL(i2c_com_init(), "i2c com");
    CHECK_RESULT_BOOL(spi_init(VSPI_HOST, CONFIG_SPI_MOSI, CONFIG_SPI_MISO, CONFIG_SPI_SCK), "SPI");
    CHECK_RESULT_BOOL(
        uart_init(CONFIG_UART_PORT_NUM, CONFIG_UART_TX, CONFIG_UART_RX, CONFIG_UART_BAUDRATE),
        "UART init");

    CHECK_RESULT_BOOL(gpioexp_init(), "GPIO Expander");
    CHECK_RESULT_BOOL(gpioexp_led_set_color(WHITE), "GPIO Expander change color");

    CHECK_RESULT_BOOL(initialize_state_machine(), "STATE_MACHINE");
    CHECK_RESULT_BOOL(initialize_sd_card(), "SD CARD");
    CHECK_RESULT_BOOL(initialize_esp_now(), "ESP_NOW");
    CHECK_RESULT_BOOL(initialize_flash_memory(), "FLASH");
    CHECK_RESULT_BOOL(initialize_processing_task(), "PROCESSING TASK");
    CHECK_RESULT_BOOL(initialize_gps(), "Gps task");
    CHECK_RESULT_BOOL(initialize_recovery(), "Recovery task");

    CHECK_RESULT_BOOL(initialize_timers(), "TIMERS");
    CHECK_RESULT_BOOL(sys_timer_start(TIMER_ESP_NOW_BROADCAST, 500, TIMER_TYPE_PERIODIC),
                      "ESP_NOW_TIMER");
    // CHECK_RESULT_BOOL(
    //     sys_timer_start(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS, TIMER_TYPE_ONE_SHOT),
    //     "DC TIMER");
    // CHECK_RESULT_BOOL(sys_timer_start(TIMER_BUZZER, 2000, TIMER_TYPE_PERIODIC), "BUZZER TIMER");
    CHECK_RESULT_BOOL(sys_timer_start(TIMER_CONNECTED_DEV, 40000, TIMER_TYPE_PERIODIC),
                      "CONNECTED TIMER");
    // CHECK_RESULT_BOOL(sys_timer_start(TIMER_SD_DATA, 50, TIMER_TYPE_PERIODIC), "SD TIMER");
    CHECK_RESULT_BOOL(sys_timer_start(TIMER_DEBUG, 1000, TIMER_TYPE_PERIODIC), "DEBUG TIMER");

    CHECK_RESULT_BOOL(initialize_lora(settings.loraFreq_KHz, settings.lora_transmit_ms), "LORA");
    CHECK_RESULT_ESP(init_console(), "CLI");
    // esp_log_level_set("*", ESP_LOG_INFO);

    CHECK_RESULT_ESP(SM_change_state(IDLE), "Change state to idle");

    // if (settings.buzzer_on != 0) {
    //     // CHECK_RESULT_BOOL(buzzer_init(), "Buzzer");
    //     buzzer_turn_on();
    // }
    vTaskDelete(NULL);
}

void run_init_task(void) {
    xTaskCreatePinnedToCore(TASK_init, "Init", 8800, NULL, 20, NULL, PRO_CPU_NUM);
}
