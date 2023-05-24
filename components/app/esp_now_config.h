// Copyright 2022 PWrInSpace, Kuba

#ifndef ESP_NOW_CONFIG_H
#define ESP_NOW_CONFIG_H

#include <stdint.h>
#include "esp_now_api.h"

#define ESP_NOW_CHANNEL 0

#define MCB_MAC {0x04, 0x20, 0x04, 0x20, 0x04, 0x20}
#define BROADCAST_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define PITOT_MAC {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}
#define VENT_VALVE_MAC {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}
#define MAIN_VALVE_MAC {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}
#define TANWA_MAC {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}

#define ESP_NOW_TASK_STACK_DEPTH CONFIG_ESP_NOW_TASK_STACK_DEPTH
#define ESP_NOW_TASK_PRIORITY CONFIG_ESP_NOW_TASK_PRIORITY
#define ESP_NOW_TASK_CORE_ID CONFIG_ESP_NOW_TASK_CORE_ID

extern const ENA_device_t esp_now_broadcast;  // :CCC
extern const ENA_device_t esp_now_pitot;  // :CCC
extern const ENA_device_t esp_now_vent_valve;  // :CCC
extern const ENA_device_t esp_now_main_valve;  // :CCC
extern const ENA_device_t esp_now_tanwa;  // :CCC

/**
 * @brief Initialize esp now task
 *
 * @return true :D
 * @return false :C
 */
bool initialize_esp_now(void);

#endif
