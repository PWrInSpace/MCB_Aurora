#include "slave_structs.h"
#include "lora.pb-c.h"
#include "rocket_data.h"

void create_porotobuf_frame(LoRaFrame *frame) {
    rocket_data_t data = rocket_data_get();
    lo_ra_frame__init(frame);   // fill struct with 0

    // mcb
    frame->obc_state = data.mcb.state;
    frame->uptime = data.mcb.uptime;
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



    // main valve
    frame->mval_battery = data.main_valve.battery_voltage;

    frame->mval_byte_data |= data.main_valve.valve_state;

    // vent valve
    frame->vent_battery = data.vent_valve.battery_voltage;
    frame->tank_pressure = data.vent_valve.tank_pressure;

    frame->vent_byte_data |= data.vent_valve.valve_state;


    // tanwa


    // esp now


    // errors
}