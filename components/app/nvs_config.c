#include "nvs_config.h"

bool initialize_nvs(void) {
    NVSData key_value[] = {
        {KEY1_SETTINGS_LORA_FREQ_KHZ, KEY4_DATA_loraFreq_KHz},
        {KEY2_SETTINGS_LORA_TRANSMIT_MS, KEY3_DATA_lora_transmit_ms},
        {KEY3_SETTINGS_COUNTDOWN_TIME, KEY1_DATA_countdownTime},
        {KEY4_SETTINGS_IGNIT_TIME, KEY2_DATA_ignitTime},
        {KEY5_SETTINGS_BUZZER_ON, KEY6_DATA_buzzer_on},
        {KEY6_SETTINGS_FLASH_ON, KEY5_DATA_flash_on}
    };
    size_t length = sizeof(key_value)/sizeof(NVSData);
    
    NVSResult res = NVS_init(key_value, length);
    if (res == NVS_OK) {
        return true;
    }
    return false;
}