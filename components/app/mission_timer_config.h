#ifndef MISSION_TIMER_CONFIG_H
#define MISSION_TIMER_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

bool hybrid_mission_timer_init();

bool hybrid_mission_timer_start(int32_t countdown_time, int32_t ignition_time);

bool hybrid_mission_timer_interrupt();

int hybrid_mission_timer_get_time();

#endif