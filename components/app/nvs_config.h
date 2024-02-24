#ifndef NVS_CONFIGWS
#define NVS_CONFIG

#include <flash_nvs.h>
#include <stdbool.h>
#include <stdint.h>


#define KEY1_SETTINGS_LORA_FREQ_KHZ "lora_MHz" 
 #define KEY2_SETTINGS_LORA_TRANSMIT_MS "lora_ms" 
 #define KEY3_SETTINGS_COUNTDOWN_TIME "countdownTime" 
 #define KEY4_SETTINGS_IGNIT_TIME "ignitTime" 
 #define KEY5_SETTINGS_BUZZER_ON "buzz" 
 #define KEY6_SETTINGS_FLASH_ON "flas" 

#define KEY1_DATA_countdownTime -30000
#define KEY2_DATA_ignitTime -15000
#define KEY3_DATA_lora_transmit_ms 1800 
#define KEY4_DATA_loraFreq_KHz  915000
#define KEY5_DATA_flash_on  0
#define KEY6_DATA_buzzer_on  0 

bool initialize_nvs(void);

#endif