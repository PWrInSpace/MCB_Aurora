// Copyright 2022 PWrInSpace, Kuba
#ifndef TIMER_CFG_H
#define TIMER_CFG_H

#include "system_timer.h"
#include "stdbool.h"

typedef enum {
    TIMER_SD_DATA = 0,
    TIMER_ESP_NOW_BROADCAST,
    TIMER_FLASH_DATA,
    TIMER_IGNITION,
    TIMER_LIFTOFF,
    TIMER_DISCONNECT,
    TIMER_DEBUG,
} timers_id_def;

#define DISCONNECT_TIMER_PERIOD_MS 15 * 1000 * 1000

bool initialize_timers(void);

#endif