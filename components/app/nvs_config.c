#include "nvs_config.h"

bool initialize_nvs(void)
{
    NVSData key_value[] = 
{
    {KEY1, KEY1_DATA},

};
   NVSResult res =  NVS_init(key_value, DATA_LENGTH);
    if(res==NVS_OK)
    {
        return true;
    }
    return false;
}