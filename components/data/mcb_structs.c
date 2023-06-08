// Copyright 2022 PWrInSpace, Kuba
#include "mcb_structs.h"
#include "state_machine.h"
#include "utils.h"
#include "esp_log.h"
#include "mission_timer_config.h"
#include "system_timer_config.h"
#include "processing_task_config.h"
#include "vbat_wrapper.h"

#define TAG "MS"
bool mcb_update_struct(mcb_data_t *mcb) {
    mcb->state = SM_get_current_state();
    mcb->uptime = get_uptime_ms();
    mcb->flight_time = hybrid_mission_timer_get_time();
    mcb->battery_voltage = vbat_read();

    uint64_t dc_timer_expire;
    sys_timer_get_expiry_time(TIMER_DISCONNECT, &dc_timer_expire);
    if (dc_timer_expire == 0) {
        mcb->disconnect_timer = DISCONNECT_TIMER_PERIOD_S;
    } else {
        mcb->disconnect_timer = ((dc_timer_expire / 1000) - get_uptime_ms()) / 1000.0;
    }

    gps_positioning_t gps_posiotion = gps_get_positioning();
    mcb->gps_altitude = gps_posiotion.altitude;
    mcb->latitude = gps_posiotion.latitude;
    mcb->longitude = gps_posiotion.longitude;
    mcb->satelites_in_view = gps_posiotion.sats_in_view;

    sensors_data_t sensors_data;
    sensors_get_data(&sensors_data, sizeof(sensors_data), 100);
    mcb->acc_x = sensors_data.acc_x;
    mcb->acc_y = sensors_data.acc_y;
    mcb->acc_z = sensors_data.acc_z;
    mcb->gyr_x = sensors_data.gyr_x;
    mcb->gyr_y = sensors_data.gyr_y;
    mcb->gyr_z = sensors_data.gyr_z;
    mcb->mag_x = sensors_data.mag_x;
    mcb->mag_y = sensors_data.mag_y;
    mcb->mag_z = sensors_data.mag_z;
    mcb->pressure = sensors_data.pressure;
    mcb->temperature = sensors_data.temperature;
    mcb->altitude = sensors_data.altitude;
    mcb->velocity = sensors_data.velocity;
    mcb->yaw = sensors_data.yaw;
    mcb->pitch = sensors_data.pitch;
    mcb->roll = sensors_data.roll;

    return true;
}