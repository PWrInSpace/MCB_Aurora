// Copyright 2022 PWrInSpace, Kuba
#include "mcb_structs.h"
#include "state_machine.h"
#include "utils.h"
#include "esp_log.h"
#include "mission_timer_config.h"
#include "system_timer_config.h"

#define TAG "MS"
bool mcb_update_struct(mcb_data_t *mcb) {
    mcb->state = SM_get_current_state();
    mcb->uptime = get_uptime_ms();
    mcb->flight_time = hybrid_mission_timer_get_time();
    mcb->battery_voltage = 7.8;
    
    uint64_t dc_timer_expire;
    sys_timer_get_expiry_time(TIMER_DISCONNECT, &dc_timer_expire);
    if (dc_timer_expire == 0) {
        mcb->disconnect_timer = DISCONNECT_TIMER_PERIOD_MS;
    } else {
        mcb->disconnect_timer = (dc_timer_expire / 1000) - get_uptime_ms();
    }
    
    return true;
}