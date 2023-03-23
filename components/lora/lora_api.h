#ifndef LORA_API_H
#define LORA_API_H

#include <stdint.h>
#include "lora.h"

typedef uint8_t lora_api_id_t;
typedef void (*lora_api_on_receive)(void);

typedef struct {
    lora_api_id_t devi_id;
    lora_struct_t* lora_config;
} lora_api_config_t;

bool lora_api_init(lora_api_config_t *cfg);


#endif