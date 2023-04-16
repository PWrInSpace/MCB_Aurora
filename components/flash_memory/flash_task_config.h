#ifndef FLASH_CONFIG
#define FLASH_CONFIG

#include <stdint.h>
#include <stdbool.h>

bool initialize_flash_memory(void);

bool flash_data_timer_start(uint32_t period_ms);

bool flash_data_timer_change_period(uint32_t period_ms);

bool flash_data_timer_terminate(void);

#endif