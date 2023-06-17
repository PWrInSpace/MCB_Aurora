#include "slave_structs.h"
#include "lora.pb-c.h"
#include "rocket_data.h"
#include "errors_config.h"
#include "esp_log.h"
#include "settings_mem.h"

#define TAG "PBF"

void create_porotobuf_data_frame(LoRaFrame *frame) {
    rocket_data_t data = rocket_data_get();
    lo_ra_frame__init(frame);   // fill struct with 0
    error_data_t errors[MAX_NUMBER_OF_ERRORS];
    if (errors_get_all(errors, sizeof(errors)) == false) {
        ESP_LOGI(TAG, "Unable to get errrors");
    }

    // mcb
    frame->obc_state = data.mcb.state;
    frame->dc_time = data.mcb.disconnect_timer;
    frame->flight_time = data.mcb.flight_time;
    frame->mcb_battery = data.mcb.battery_voltage;
    frame->gps_lat = data.mcb.latitude;
    frame->gps_long = data.mcb.longitude;
    frame->gps_sat = data.mcb.satelites_in_view;

    frame->mcb_altitude = data.mcb.altitude;
    frame->mcb_velocity = data.mcb.velocity;
    frame->mcb_temperature = data.mcb.temperature;
    frame->euler_psi = data.mcb.pitch;
    frame->euler_theta = data.mcb.roll;
    frame->euler_fi = data.mcb.yaw;

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
    frame->temperature1 = data.tanwa.rocketWeight_temp;
    frame->temperature2 = data.tanwa.tankWeight_temp;
    frame->pressure = data.tanwa.pressureSensor;

    frame->tanwa_byte_data |= data.tanwa.motorState_1;
    frame->tanwa_byte_data |= (data.tanwa.motorState_2 << 3);
    frame->tanwa_byte_data |= (data.tanwa.motorState_3 << 6);
    frame->tanwa_byte_data |= (data.tanwa.motorState_4 << 9);
    frame->tanwa_byte_data |= (data.tanwa.hxRequest_RCK << 20);
    frame->tanwa_byte_data |= (data.tanwa.hxRequest_TANK << 22);

    frame->tanwa_byte_data |= (data.tanwa.tankWeight_blink << 28);
    frame->tanwa_byte_data |= (data.tanwa.rocketWeight_blink << 29);
    frame->tanwa_byte_data |= (data.tanwa.igniterContinouity_1 << 30);
    frame->tanwa_byte_data |= (data.tanwa.igniterContinouity_2 << 31);

    // payload
    frame->payload_battery = data.payload.vbat;

    // esp now
    frame->esp_now_byte_data |= (data.pitot.waken_up << 0);
    frame->esp_now_byte_data |= (data.main_valve.waken_up << 1);
    frame->esp_now_byte_data |= (data.vent_valve.waken_up << 2);
    frame->esp_now_byte_data |= (data.payload.waken_up << 3);

    frame->esp_now_byte_data |= (data.connected_dev.pitot << 16);
    frame->esp_now_byte_data |= (data.connected_dev.main_valve << 17);
    frame->esp_now_byte_data |= (data.connected_dev.vent_valve << 18);
    frame->esp_now_byte_data |= (data.connected_dev.tanwa << 19);
    frame->esp_now_byte_data |= (data.connected_dev.payload << 20);


    // errors
    frame->errors |= errors[ERROR_TYPE_LAST_EXCEPTION];
    frame->errors |= (errors[ERROR_TYPE_RECOVERY] << 8);
    frame->errors |= (errors[ERROR_TYPE_ESP_NOW] << 12);
    frame->errors |= (errors[ERROR_TYPE_MEMORY] << 16);
    frame->errors |= (errors[ERROR_TYPE_MCB] << 24);
    frame->errors |= (errors[ERROR_TYPE_SENSORS] << 28);
}

void create_protobuf_settings_frame(LoRaSettings *frame) {
    Settings settings = settings_get_all();
    frame->countdown_time = settings.countdownTime;
    frame->ingition_time = settings.ignitTime;
    frame->lora_freq_khz = settings.loraFreq_KHz;
    frame->lora_transmit_ms = settings.lora_transmit_ms;
}