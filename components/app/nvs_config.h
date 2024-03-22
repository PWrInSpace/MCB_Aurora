#ifndef NVS_CONFIG
#define NVS_CONFIG

#include <flash_nvs.h>
#include <stdbool.h>
#include <stdint.h>

#define SETTINGS_LORA_FREQ_KHZ "lora_MHz"
#define SETTINGS_LORA_TRANSMIT_MS "lora_ms"
#define SETTINGS_COUNTDOWN_TIME "countdownTime"
#define SETTINGS_IGNITE_TIME "ignitTime"
#define SETTINGS_BUZZER_ON "buzz"
#define SETTINGS_FLASH_ON "flash"

#define DATA_countdownTime -30000
#define DATA_igniteTime -15000
#define DATA_lora_transmit_ms 1800
#define DATA_loraFreq_KHz 915000
#define DATA_flash_on 0
#define DATA_buzzer_on 0

bool initialize_nvs(void);
bool default_nvs(void);
#endif