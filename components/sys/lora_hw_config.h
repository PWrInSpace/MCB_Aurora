// Copyright 2023 PWr in Space, Gliwuś
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "freertos/task.h"

/**
 * @brief Initialize spi for lora
 *
 * @param host spi host number
 * @return true :D
 * @return false :C
 */
bool lora_hw_spi_add_device(spi_host_device_t host);

/**
 * @brief Set lora gpios
 *
 * @return true :D
 * @return false :C
 */
bool lora_hw_set_gpio();

/**
 * @brief Attach d0 pin interrupt
 *
 * @param interrupt_cb on irq callback
 * @return true :D
 * @return false :C
 */
bool lora_hw_attach_d0_interrupt(gpio_isr_t interrupt_cb);

/**
 * @brief Spi transmit function for lora
 *
 * @param _in in buffer
 * @param _out out buffer
 * @return true :D
 * @return false :C
 */
bool lora_hw_spi_transmit(uint8_t _in[2], uint8_t _out[2]);

/**
 * @brief Lora delay fnc
 * 
 * @param _ms delay ms
 */
void lora_hw_delay(size_t _ms);

/**
 * @brief Lora set gpio fnc
 * 
 * @param _gpio_num gpio pin number
 * @param _level 1 - High, 0 - Low
 * @return true :D
 * @return false :C
 */
bool lora_hw_gpio_set_level(uint8_t _gpio_num, uint32_t _level);

/**
 * @brief Lora log fnc
 * 
 * @param info log message
 */
void lora_hw_log(const char* info);
