// Copyright 2022 PWrInSpace, Kuba
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdcard.h"
#include "spi.h"
#include "esp_timer.h"
#include "i2c.h"

spi_t spi;
i2c_t i2c;
sd_card_t sd;

#define TAG "AURORA"

void app_main(void) {
    ESP_LOGI(TAG, "Hello world!\n");

    I2C_master_init(&i2c, I2C_NUM_1, 21, 22);

    if (SPI_init(&spi, HSPI_HOST, 23, 19, 18) == false) {
        ESP_LOGE(TAG, "SPI init :C");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
    }
    if (SD_init(&sd, spi.spi_host, 5, MOUNT_POINT) == false) {
        ESP_LOGE(TAG, "SD init :C");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
    }

    uint32_t start;
    uint32_t stop;
    float x = 12;

    while (1) {
        ESP_LOGI(TAG, "HELLO");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
