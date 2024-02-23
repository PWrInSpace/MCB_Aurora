#ifndef NVS_CONFIGWS
#define NVS_CONFIG

#include <stdint.h>
#include <stdbool.h>
#include <flash_nvs.h>

#define KEY1 "klucz1"
#define KEY1_DATA 123
#define DATA_LENGTH 1


bool initialize_nvs(void);

#endif