// Copyright 2023 PWr in Space, Gliwuś
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "freertos/task.h"
#include "lora.h"

bool _lora_spi_and_pins_init();

bool _lora_SPI_transmit(uint8_t _in[2], uint8_t _out[2]);

void _lora_delay(size_t _ms);

bool _lora_GPIO_set_level(uint8_t _gpio_num, uint32_t _level);

void _lora_log(const char* info);
