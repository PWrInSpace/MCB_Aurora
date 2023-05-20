#include "slave_structs.h"
#include "lora.pb-c.h"
#include "rocket_data.h"
#include "errors_config.h"
#include "esp_log.h"

#define TAG "PBF"

void create_porotobuf_frame(LoRaFrame *frame) {
    rocket_data_t data = rocket_data_get();
    lo_ra_frame__init(frame);   // fill struct with 0
    error_data_t errors[MAX_NUMBER_OF_ERRORS];
    if (errors_get_all(errors, sizeof(errors)) == false) {
        ESP_LOGI(TAG, "Unable to get errrors");
    }

    // mcb
    frame->obc_state = data.mcb.state;
    frame->uptime = data.mcb.disconnect_timer;
    frame->flight_time = data.mcb.flight_time;
    frame->mcb_battery = data.mcb.battery_voltage;

    // recovery
    frame->recov_pressure_1 = data.recovery.pressure1;
    frame->recov_pressure_2 = data.recovery.pressure2;

    frame->recov_byte_data |= (data.recovery.isArmed << 0);
    frame->recov_byte_data |= (data.recovery.isTeleActive << 1);
    frame->recov_byte_data |= (data.recovery.easyMiniFirstStage << 2);
    frame->recov_byte_data |= (data.recovery.easyMiniSecondStage << 3);
    frame->recov_byte_data |= (data.recovery.telemetrumFirstStage << 4);
    frame->recov_byte_data |= (data.recovery.telemetrumSecondStage<< 5);
    frame->recov_byte_data |= (data.recovery.firstStageDone << 6);
    frame->recov_byte_data |= (data.recovery.secondStageDone << 7);

    frame->recov_byte_data |= (data.recovery.firstStageContinouity << 8);
    frame->recov_byte_data |= (data.recovery.secondStageContinouity << 9);
    frame->recov_byte_data |= (data.recovery.separationSwitch1 << 10);
    frame->recov_byte_data |= (data.recovery.separationSwitch2 << 11);

    // pitot
    frame->pitot_battery = data.pitot.vbat;
    frame->pitot_altitude = data.pitot.alt;
    frame->pitot_velocity = data.pitot.speed;
    frame->pitot_temperature = data.pitot.temperature;

    // main valve
    frame->mval_battery = data.main_valve.battery_voltage;

    frame->mval_byte_data |= data.main_valve.valve_state;

    // vent valve
    frame->vent_battery = data.vent_valve.battery_voltage;
    frame->tank_pressure = data.vent_valve.tank_pressure;

    frame->vent_byte_data |= data.vent_valve.valve_state;

    // tanwa
    frame->tanwa_battery = data.tanwa.vbat;
    frame->tanwa_state = data.tanwa.tanWaState;
    frame->rocket_weight = data.tanwa.rocketWeight_val;
    frame->tank_weight = data.tanwa.tankWeight_val;
    frame->tanwa_byte_data |= 0x00;

    // esp now
    frame->esp_now_byte_data |= (data.main_valve.waken_up << 1);
    frame->esp_now_byte_data |= (data.vent_valve.waken_up << 2);
    frame->esp_now_byte_data |= (data.payload.waken_up << 3);

    // errors
    frame->errors |= errors[ERROR_TYPE_LAST_EXCEPTION];
    frame->errors |= (errors[ERROR_TYPE_RECOVERY] << 8);
    frame->errors |= (errors[ERROR_TYPE_ESP_NOW] << 12);
    frame->errors |= (errors[ERROR_TYPE_MEMORY] << 16);
    frame->errors |= (errors[ERROR_TYPE_MCB] << 24);
    frame->errors |= (errors[ERROR_TYPE_SENSORS] << 28);
}