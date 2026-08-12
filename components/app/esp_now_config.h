// Copyright 2022 PWrInSpace, Kuba

#ifndef ESP_NOW_CONFIG_H
#define ESP_NOW_CONFIG_H

#include <stdint.h>

#include "esp_now_api.h"
#include "slave_structs.h"

#define ESP_NOW_CHANNEL 0

#define MCB_MAC {0x04, 0x20, 0x04, 0x20, 0x04, 0x20}
#define BROADCAST_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define PITOT_MAC {0x50, 0x49, 0x54, 0x55, 0x53, 0x32}
#define ETH_VENT_VALVE_MAC {0xBA, 0x11, 0x22, 0x33, 0x44, 0x88}
#define N2_VENT_VALVE_MAC {0xBA, 0x11, 0x22, 0x33, 0x44, 0x66}
#define OX_MAIN_VALVE_MAC {0xBA, 0x11, 0x22, 0x33, 0x44, 0x55}
#define OX_VENT_ETH_MAIN_VALVES_MAC {0xBA, 0x11, 0x22, 0x33, 0x44, 0x77}
#define TANWA_MAC {0x80, 0x08, 0x50, 0x80, 0x08, 0x50}
#define PAYLOAD_MAC {0x7C, 0x87, 0xCE, 0x28, 0x04, 0x28}

#define ESP_NOW_TASK_STACK_DEPTH CONFIG_ESP_NOW_TASK_STACK_DEPTH
#define ESP_NOW_TASK_PRIORITY CONFIG_ESP_NOW_TASK_PRIORITY
#define ESP_NOW_TASK_CORE_ID CONFIG_ESP_NOW_TASK_CORE_ID

extern const ENA_device_t esp_now_broadcast;       // :CCC
extern const ENA_device_t esp_now_pitot;           // :CCC
extern const ENA_device_t esp_now_eth_vent_valve;  // :CCC
extern const ENA_device_t esp_now_n2_vent_valve;   // :CCC
extern const ENA_device_t esp_now_ox_main_valve;
extern const ENA_device_t esp_now_ox_vent_eth_main_valves;
extern const ENA_device_t esp_now_tanwa;    // :CCC
extern const ENA_device_t esp_now_payload;  // :CCC

/**
 * @brief Initialize esp now task
 *
 * @return true :D
 * @return false :C
 */
bool initialize_esp_now(void);

void esp_now_get_connected_dev(esp_now_connected_devices_t *dev);

void esp_now_clear_connected_dev(void);

#endif
