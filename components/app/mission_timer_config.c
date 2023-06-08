#include "mission_timer_config.h"
#include "mission_timer.h"
#include "system_timer_config.h"

#include "esp_log.h"

#define TAG "MT"

bool hybrid_mission_timer_init(int32_t countdown_start_ms) {
    return mission_timer_init(countdown_start_ms);
}

bool hybrid_mission_timer_start(int32_t countdown_time, int32_t ignition_time) {
    assert(countdown_time < 0);
    assert(ignition_time < 0);
    if (countdown_time >= 0 || ignition_time >= 0) {
        return false;
    }

    if (countdown_time >= ignition_time) {
        return false;
    }

    if (sys_timer_start(TIMER_IGNITION, ignition_time - countdown_time, TIMER_TYPE_ONE_SHOT) == false) {
        ESP_LOGE(TAG, "Unable to start ignition timer");
        goto abort_start;
    }

    if (sys_timer_start(TIMER_LIFTOFF, - countdown_time, TIMER_TYPE_ONE_SHOT) == false) {
        ESP_LOGE(TAG, "Unable to start liftoff timer");
        goto abort_start;
    }

    if (mission_timer_start(countdown_time) == false) {
        ESP_LOGE(TAG, "Unable to start mission timer");
        goto abort_start;
    }

    return true;
abort_start:
    hybrid_mission_timer_stop();
    return false;
}

bool hybrid_mission_timer_stop() {
    if (sys_timer_stop(TIMER_IGNITION) == false) {
        return false;
    }

    if (sys_timer_stop(TIMER_LIFTOFF) == false) {
        return false;
    }

    if (mission_timer_stop() == false) {
        return false;
    }

    return true;
}

int hybrid_mission_timer_get_time() {
    return mission_timer_get_time();
}

void hybrid_mission_timer_set_disable_val(int64_t time) {
    mission_timer_set_disable_value(time);
}