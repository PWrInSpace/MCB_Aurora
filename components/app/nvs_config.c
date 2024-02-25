#include "nvs_config.h"

bool initialize_nvs(void) {
    NVSData key_value[] = {{SETTINGS_LORA_FREQ_KHZ, DATA_loraFreq_KHz},
                           {SETTINGS_LORA_TRANSMIT_MS, DATA_lora_transmit_ms},
                           {SETTINGS_COUNTDOWN_TIME, DATA_countdownTime},
                           {SETTINGS_IGNIT_TIME, DATA_ignitTime},
                           {SETTINGS_BUZZER_ON, DATA_buzzer_on},
                           {SETTINGS_FLASH_ON, DATA_flash_on}};
    size_t length = sizeof(key_value) / sizeof(NVSData);

    NVSResult res = NVS_init(key_value, length);
    if (res == NVS_OK) {
        return true;
    }
    return false;
}