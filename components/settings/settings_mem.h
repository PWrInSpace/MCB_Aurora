#ifndef SETTINGS_MEM_H
#define SETTINGS_MEM_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#define SETTINGS_TAG "SettingsTag"

#define SETTINGS_NAMESPACE "Settings"
#define SETTINGS_LORA_FREQ_KHZ "lora_MHz"
#define SETTINGS_LORA_TRANSMIT_MS "lora_ms"
#define SETTINGS_COUNTDOWN_TIME "countdownTime"
#define SETTINGS_IGNIT_TIME "ignitTime"
#define SETTINGS_BUZZER_ON "buzz"
#define SETTINGS_FLASH_ON "flas"

typedef struct {
    int32_t loraFreq_KHz;
    int32_t lora_transmit_ms;
    int32_t countdownTime;
    int32_t ignitTime;
    int32_t buzzer_on;
    int32_t flash_on;
} Settings;


esp_err_t settings_init();
esp_err_t settings_save_all();
esp_err_t settings_read_all();
void settings_init_default();
Settings settings_get_all();
int32_t settings_read(char* setting_name);
esp_err_t settings_save(char* setting_name, int32_t newVal);

#endif