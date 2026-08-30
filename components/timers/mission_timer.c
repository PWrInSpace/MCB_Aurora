// Copyright 2022 PWrInSpace, Kuba
#include "mission_timer.h"
#include "utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static struct {
    uint32_t t_zero_time;
    bool enable;
    int32_t disableValue;
    SemaphoreHandle_t time_mutex;
} gb = {
    .t_zero_time = 0,
    .enable = false,
    .disableValue = 999,
    .time_mutex = NULL,
};

bool mission_timer_init(int32_t timer_disable_value) {
    gb.time_mutex = xSemaphoreCreateMutex();
    if (gb.time_mutex == NULL) {
        return false;
    }
    gb.disableValue = timer_disable_value;
    return true;
}

bool mission_timer_start(int32_t countdown_begin_time_ms) {
    assert(countdown_begin_time_ms < 0);

    if (countdown_begin_time_ms > 0) {
        return false;
    }

    if (xSemaphoreTake(gb.time_mutex, 1000) != pdTRUE) {
        return false;
    }
    gb.t_zero_time = get_uptime_ms() - countdown_begin_time_ms;
    gb.enable = true;
    xSemaphoreGive(gb.time_mutex);

    return true;
}

int32_t mission_timer_get_time() {
    int32_t ret = 0;

    if (xSemaphoreTake(gb.time_mutex, 100) != pdTRUE) {
        return gb.disableValue;
    }

    if (gb.enable == true) {
        ret = (int32_t)get_uptime_ms() - gb.t_zero_time;
    } else {
        ret = gb.disableValue;
    }

    xSemaphoreGive(gb.time_mutex);

    return ret;
}

bool mission_timer_is_enable() {
    return gb.enable;
}

bool mission_timer_stop() {
    if (xSemaphoreTake(gb.time_mutex, 1000) != pdTRUE) {
        return false;
    }

    gb.t_zero_time = 0;
    gb.enable = false;

    xSemaphoreGive(gb.time_mutex);

    return true;
}

bool mission_timer_set_disable_value(int64_t value) {
    if (xSemaphoreTake(gb.time_mutex, 100) != pdTRUE) {
        return false;
    }

    gb.disableValue = value;
    xSemaphoreGive(gb.time_mutex);
    return true;
}