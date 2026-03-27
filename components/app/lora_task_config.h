// Copyright 2022 PWrInSpace, Kuba
#ifndef LORA_TASK_CONFIG_H
#define LORA_TASK_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include "lora_task.h"

#define PACKET_PREFIX "SP3MIK"
#define LORA_UART_PORT 1
#define LORA_UART_RX 9
#define LORA_UART_TX 10
#define LORA_UART_BAUDRATE 115200

/**
 * @brief Initialize lora api and run lora task
 *q
 * @param frequency_khz lora frequency kzh
 * @param transmiting_period lora transmiting period
 * @return true :D
 * @return false :C
 */
bool initialize_lora(uint32_t frequency_khz, uint32_t transmiting_period);


void lora_send_settings_frame(void);

#endif
