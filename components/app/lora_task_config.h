// Copyright 2022 PWrInSpace, Kuba
#ifndef LORA_TASK_CONFIG_H
#define LORA_TASK_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#define PACKET_HEADER ((uint8_t)0x32)
#define DATA_SIZE_LEN 1
#define CHECKSUM_LEN 2
#define LORA_UART_PORT 1
#define LORA_UART_RX 10
#define LORA_UART_TX 9
#define LORA_UART_BAUDRATE 115200

/**
 * @brief Initialize lora api and run lora task
 *q
 * @param frequency_khz lora frequency kzh
 * @param transmitting_period lora transmitting period
 * @return true :D
 * @return false :C
 */
bool initialize_lora(uint32_t frequency_khz, uint32_t transmitting_period);

void lora_send_settings_frame(void);

#endif
