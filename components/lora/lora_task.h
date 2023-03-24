#ifndef LORA_TASK_H
#define LORA_TASK_H

#include <stdint.h>
#include "lora.h"

#define LORA_TASK_PRIORITY 10
#define LORA_TASK_STACK_DEPTH 8000
#define LORA_TASK_CPU_NUM 1

#define LORA_TASK_CRC_ENABLE CONFIG_LORA_CRC_ENABLE
#define LORA_TASK_FREQUENCY_KHZ CONFIG_LORA_FREQUENCY_KHZ
#define LORA_TASK_BANDWIDTH CONFIG_LORA_BANDWIDTH
#define LORA_TASK_SPREADING_FACTOR CONFIG_LORA_SPREADING_FACTOR
#define LORA_TASK_RECEIVE_WINDOW CONFIG_LORA_RECEIVE_WINDOW

#define PRIVILAGE_MASK 0x01
#define BORADCAST_DEV_ID 0x00

typedef uint8_t lora_dev_id;

typedef enum {
    LORA_IDLE,
    LORA_SLEEP,
    LORA_TRANSMIT,
    LORA_RECEIVE,
} lora_state_t;

typedef struct {
    lora_dev_id dev_id;
    lora_struct_t* lora;
} lora_api_config_t;



bool lora_task_init(lora_api_config_t *cfg);
void lora_task_irq_notifi(void);

// void lora_api_on_irq(void);
// bool lora_wait_for_irq(void);

// bool lora_api_check_dev_id(lora_api_id_t dev_id);
#endif