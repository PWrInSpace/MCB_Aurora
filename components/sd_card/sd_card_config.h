#ifndef SD_CARD_CONFIG
#define SD_CARD_CONFIG

#include <stdint.h>
#include <stdbool.h>

bool initialize_sd_card(void);

bool sd_data_timer_start(uint32_t period_ms);

bool sd_data_timer_change_period(uint32_t period_ms);

bool sd_data_timer_terminate(void);

#endif