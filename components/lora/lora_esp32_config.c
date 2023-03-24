// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "lora_esp32_config.h"
#include "lora_task.h"
#include "driver/gpio.h"

static spi_device_handle_t __spi;

#define LORA_CS_PIN CONFIG_LORA_CS
#define CONFIG_RST_GPIO CONFIG_LORA_RS
#define LORA_D0_PIN CONFIG_LORA_D0


#define TAG "LORA"

static void IRAM_ATTR gpio_interrupt_cb(void *args) {
    lora_task_irq_notifi();
}

bool spi_lora_add_device() {
  esp_err_t ret;

  spi_device_interface_config_t dev = {.clock_speed_hz = 9000000,
                                       .mode = 0,
                                       .spics_io_num = -1,
                                       .queue_size = 1,
                                       .flags = 0,
                                       .pre_cb = NULL};
  ret = spi_bus_add_device(VSPI_HOST, &dev, &__spi);
  ESP_ERROR_CHECK(ret);

  /*
   * Configure CPU hardware to communicate with the radio chip
   */
  gpio_set_direction(CONFIG_RST_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(LORA_CS_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(LORA_D0_PIN, GPIO_MODE_INPUT);
  gpio_pulldown_en(LORA_D0_PIN);
  gpio_pullup_dis(LORA_D0_PIN);
  gpio_set_intr_type(LORA_D0_PIN, GPIO_INTR_POSEDGE);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(LORA_D0_PIN, gpio_interrupt_cb, NULL);
  return ret == ESP_OK ? true : false;
}

bool _lora_SPI_transmit(uint8_t _in[2], uint8_t _out[2]) {
  spi_transaction_t t = {.flags = 0,
                         .length = 8 * sizeof(uint8_t) * 2,
                         .tx_buffer = _out,
                         .rx_buffer = _in};

  gpio_set_level(LORA_CS_PIN, 0);
  spi_device_transmit(__spi, &t);
  gpio_set_level(LORA_CS_PIN, 1);
  return true;
}

void _lora_delay(size_t _ms) { vTaskDelay(pdMS_TO_TICKS(_ms)); }

bool _lora_GPIO_set_level(uint8_t _gpio_num, uint32_t _level) {
  return gpio_set_level(_gpio_num, _level) == ESP_OK ? true : false;
}

void _lora_log(const char* info) { ESP_LOGI(TAG, "%s", info); }
