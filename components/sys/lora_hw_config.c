// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "lora_hw_config.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "utils.h"
#include "sdkconfig.h"
#include "spi.h"
extern SemaphoreHandle_t mutex_spi;

static spi_device_handle_t __spi;

#define TAG "LORA"

bool lora_hw_spi_add_device(spi_host_device_t host) {
    esp_err_t ret;

    spi_device_interface_config_t dev = {.clock_speed_hz = 400000,
                                         .mode = 0,
                                         .spics_io_num = CONFIG_LORA_CS,
                                         .queue_size = 1,
                                         .cs_ena_pretrans = 16,
                                         .cs_ena_posttrans = 16,
                                         .flags = 0
                                        };
    ret = spi_bus_add_device(host, &dev, &__spi);
    ESP_ERROR_CHECK(ret);

    return ret == ESP_OK ? true : false;
}
bool lora_hw_set_gpio() {
    RETURN_FALSE_ON_ERROR(gpio_set_direction(CONFIG_LORA_RS, GPIO_MODE_OUTPUT));
    // RETURN_FALSE_ON_ERROR(gpio_set_direction(CONFIG_LORA_CS, GPIO_MODE_OUTPUT));
    RETURN_FALSE_ON_ERROR(gpio_set_direction(CONFIG_LORA_D0, GPIO_MODE_INPUT));
    return true;
}

bool lora_hw_attach_d0_interrupt(gpio_isr_t interrupt_cb) {
    RETURN_FALSE_ON_ERROR(gpio_pulldown_en(CONFIG_LORA_D0));
    RETURN_FALSE_ON_ERROR(gpio_pullup_dis(CONFIG_LORA_D0));
    RETURN_FALSE_ON_ERROR(gpio_set_intr_type(CONFIG_LORA_D0, GPIO_INTR_POSEDGE));
    RETURN_FALSE_ON_ERROR(gpio_install_isr_service(0));
    RETURN_FALSE_ON_ERROR(gpio_isr_handler_add(CONFIG_LORA_D0, interrupt_cb, NULL));
    return true;
}

bool lora_hw_spi_transmit(uint8_t _in[2], uint8_t _out[2]) {
    spi_transaction_t t = {
        .flags = 0, .length = 8 * sizeof(uint8_t) * 2, .tx_buffer = _out, .rx_buffer = _in};
    xSemaphoreTake(mutex_spi, portMAX_DELAY);
    // gpio_set_level(CONFIG_LORA_CS, 0);
    spi_device_transmit(__spi, &t);
    // gpio_set_level(CONFIG_LORA_CS, 1);
    xSemaphoreGive(mutex_spi);
    return true;
}

void lora_hw_delay(size_t _ms) { vTaskDelay(pdMS_TO_TICKS(_ms)); }

bool lora_hw_gpio_set_level(uint8_t _gpio_num, uint32_t _level) {
    return gpio_set_level(_gpio_num, _level) == ESP_OK ? true : false;
}

void lora_hw_log(const char* info) { ESP_LOGD(TAG, "%s", info); }
