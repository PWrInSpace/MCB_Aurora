#include "nvs_config.h"
static NVSData key_value[] = {{SETTINGS_LORA_FREQ_KHZ, DATA_loraFreq_KHz},
                           {SETTINGS_LORA_TRANSMIT_MS, DATA_lora_transmit_ms},
                           {SETTINGS_COUNTDOWN_TIME, DATA_countdownTime},
                           {SETTINGS_IGNITE_TIME, DATA_igniteTime},
                           {SETTINGS_BUZZER_ON, DATA_buzzer_on},
                           {SETTINGS_FLASH_ON, DATA_flash_on}};

bool initialize_nvs(void) {
    
    size_t length = sizeof(key_value) / sizeof(NVSData);

    NVSResult res = NVS_init(key_value, length);
    if (res == NVS_OK) {
        return true;
    }
    return false;
}

bool default_nvs(void)
{
    size_t length = sizeof(key_value) / sizeof(NVSData);
    for(int i=0; i<length; i++)
    {
        while(NVS_write_int32(key_value[i].key,key_value[i].value)==NVS_OK);
    }
    return 0;
}