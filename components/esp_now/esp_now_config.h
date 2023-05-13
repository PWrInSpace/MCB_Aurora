// Copyright 2022 PWrInSpace, Kuba

#ifndef ESP_NOW_CONFIG_H
#define ESP_NOW_CONFIG_H

#include <stdint.h>
#include "esp_now_api.h"

#define ESP_NOW_CHANNEL 0

#define MCB_MAC {0x04, 0x20, 0x04, 0x20, 0x04, 0x20}
#define BROADCAST_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define PITOT_MAC {0xB0, 0xB2, 0x1C, 0x09, 0xDF, 0xAC}
#define VENT_VALVE_MAC {0x40, 0x91, 0x51, 0x9A, 0x48, 0x20}
#define MAIN_VALVE_MAC {0x40, 0x91, 0x51, 0x9A, 0x48, 0x21}
#define TANWA_MAC {0x40, 0x91, 0x51, 0x9A, 0x48, 0x23}
#define RECOVERY_MAC {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}

extern const ENA_device_t esp_now_broadcast;  // :CCC
extern const ENA_device_t esp_now_pitot;  // :CCC
extern const ENA_device_t esp_now_vent_valve;  // :CCC
extern const ENA_device_t esp_now_main_valve;  // :CCC
extern const ENA_device_t esp_now_tanwa;  // :CCC
extern const ENA_device_t esp_now_recovery; // :CCC


bool initialize_esp_now(void);

bool esp_now_broadcast_timer_start(uint32_t period_ms);

bool esp_now_broadcast_timer_change_period(uint32_t period_ms);

bool esp_now_broadcast_timer_terminate(void);


#endif
