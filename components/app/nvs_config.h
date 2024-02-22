#ifndef NVS_CONFIGWS
#define NVS_CONFIG

#include <stdint.h>
#include <stdbool.h>
#include <flash_nvs.h>

#define KEY1 "klucz1"
#define KEY1_DATA 123
#define DATA_LENGTH 1
NVSData key_value[] = 
{
    {KEY1, KEY1_DATA},

};

bool initialize_nvs(void)
{
   NVSResult res =  NVS_init(key_value, DATA_LENGTH);
    if(res==NVS_OK)
    {
        return true;
    }
    return false;
}

#endif