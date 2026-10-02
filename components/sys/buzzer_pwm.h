#ifndef CUSTOM_LEDC_H
#define CUSTOM_LEDC_H

#include <stdbool.h>
#include <stdint.h>

bool buzzer_init(void);

bool buzzer_turn_on(void);

bool buzzer_turn_off(void);

bool buzzer_change_freq(uint16_t freq);

#endif