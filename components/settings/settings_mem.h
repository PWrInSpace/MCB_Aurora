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

typedef struct {

    int32_t loraFreq_MHz;
    int32_t loraFreq_ms;
    int32_t countdownTime;
    int32_t ignitTime;
} Settings;

extern Settings settings;

esp_err_t settings_init();
esp_err_t settings_save_all();
esp_err_t settings_read_all();

#endif