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
#define SETTINGS_LORA_FREQ_MHZ "lora_MHz"
#define SETTINGS_LORA_FREQ_MS "lora_ms"
#define SETTINGS_COUNTDOWN_TIME "countdownTime"
#define SETTINGS_IGNIT_TIME "ignitTime"

typedef struct {
    int32_t loraFreq_MHz;
    int32_t loraFreq_ms;
    int32_t countdownTime;
    int32_t ignitTime;
} Settings;


esp_err_t settings_init();
esp_err_t settings_save_all();
esp_err_t settings_read_all();
Settings settings_get_all();
int32_t settings_read(char* setting_name);
esp_err_t settings_save(char* setting_name, int32_t newVal);

#endif