#include <string.h>
#include "gen_pysd.h"

/**
 * @brief Get header size
 *
 * @param new_line_ending end string with new line character
 * @return size_t header size
 */
size_t pysd_get_header_size(bool new_line_ending) {
    size_t string_size;
    if (new_line_ending == true) {
        string_size = snprintf(NULL, 0,
            "mcb.state;mcb.uptime;mcb.flight_time;mcb.disconnect_timer;mcb.battery_voltage;mcb.sensor_placeholder;mcb.latitude;mcb.longitude;mcb.gps_altitude;mcb.satelites_in_view;mcb.acc_x;mcb.acc_y;mcb.acc_z;mcb_acc_vertical;mcb.gyr_x;mcb.gyr_y;mcb.gyr_z;mcb.mag_x;mcb.mag_y;mcb.mag_z;mcb.temperature;mcb.pressure;mcb.altitude;mcb.velocity;mcb.yaw;mcb.pitch;mcb.roll;"
            "eth_vent_valve.waken_up;eth_vent_valve.valve_1_state;eth_vent_valve.valve_2_state;eth_vent_valve.temperature_1;eth_vent_valve.temperature_2;eth_vent_valve.temperature_3;eth_vent_valve.pressure_1;eth_vent_valve.pressure_2;eth_vent_valve.battery_voltage;"
            "n2_vent_valve.waken_up;n2_vent_valve.valve_1_state;n2_vent_valve.valve_2_state;n2_vent_valve.temperature_1;n2_vent_valve.temperature_2;n2_vent_valve.temperature_3;n2_vent_valve.pressure_1;n2_vent_valve.pressure_2;n2_vent_valve.battery_voltage;"
            "ox_vent_eth_main_valves.waken_up;ox_vent_eth_main_valves.valve_1_state;ox_vent_eth_main_valves.valve_2_state;ox_vent_eth_main_valves.temperature_1;ox_vent_eth_main_valves.temperature_2;ox_vent_eth_main_valves.temperature_3;ox_vent_eth_main_valves.pressure_1;ox_vent_eth_main_valves.pressure_2;ox_vent_eth_main_valves.battery_voltage;"
            "ox_main_valve.waken_up;ox_main_valve.valve_1_state;ox_main_valve.valve_2_state;ox_main_valve.temperature_1;ox_main_valve.temperature_2;ox_main_valve.temperature_3;ox_main_valve.pressure_1;ox_main_valve.pressure_2;ox_main_valve.battery_voltage;"
            "recovery.isArmed;recovery.isTeleActive;recovery.easyMiniFirstStage;recovery.easyMiniSecondStage;recovery.telemetrumFirstStage;recovery.telemetrumSecondStage;recovery.firstStageDone;recovery.secondStageDone;recovery.firstStageContinouity;recovery.secondStageContinouity;recovery.separationSwitch1;recovery.separationSwitch2;recovery.pressure1;"
            "payload.waken_up;payload.vbat;"
            "tanwa.vbat;tanwa.tanWaState;tanwa.thrust_val;tanwa.tankWeight_val;tanwa.temperature_postFill;tanwa.temperature_Wall;tanwa.postFillN2O_pres;tanwa.cutoffN2O_pres;tanwa.droidN2O_press;tanwa.preRegulatorN2_pres;tanwa.postRegulatorN2_pres;tanwa.postFillN2_pres;tanwa.droidN2_press;tanwa.combChamber_pres;"
            "tanwa.soft_arm;tanwa.canWeighta_con;tanwa.canSensor_con;tanwa.canSolenoid_con;tanwa.canUtility_con;tanwa.canPower_con;tanwa.igniterContinouity_1;tanwa.igniterContinouity_2;tanwa.fillN2OState;tanwa.deprN2OState;tanwa.fillN2State;tanwa.deprN2State;tanwa.droidN2OState;tanwa.droidN2State;tanwa.heatingTankState;tanwa.heatingValveState;tanwa.abortButton;"
            "pitot.waken_up;pitot.dynamic_press;pitot.static_press;pitot.temperature;pitot.humid;pitot.vbat;pitot.speed;pitot.alt;pitot.pred_apogee;"
            "connected_dev.n2_vent_valve;connected_dev.eth_vent_valve;connected_dev.ox_vent_eth_main_valves;connected_dev.ox_main_valve;connected_dev.pitot;connected_dev.payload;connected_dev.tanwa;"
            "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception\n");
    } else {
        string_size = snprintf(NULL, 0,
            "mcb.state;mcb.uptime;mcb.flight_time;mcb.disconnect_timer;mcb.battery_voltage;mcb.sensor_placeholder;mcb.latitude;mcb.longitude;mcb.gps_altitude;mcb.satelites_in_view;mcb.acc_x;mcb.acc_y;mcb.acc_z;mcb_acc_vertical;mcb.gyr_x;mcb.gyr_y;mcb.gyr_z;mcb.mag_x;mcb.mag_y;mcb.mag_z;mcb.temperature;mcb.pressure;mcb.altitude;mcb.velocity;mcb.yaw;mcb.pitch;mcb.roll;"
            "eth_vent_valve.waken_up;eth_vent_valve.valve_1_state;eth_vent_valve.valve_2_state;eth_vent_valve.temperature_1;eth_vent_valve.temperature_2;eth_vent_valve.temperature_3;eth_vent_valve.pressure_1;eth_vent_valve.pressure_2;eth_vent_valve.battery_voltage;"
            "n2_vent_valve.waken_up;n2_vent_valve.valve_1_state;n2_vent_valve.valve_2_state;n2_vent_valve.temperature_1;n2_vent_valve.temperature_2;n2_vent_valve.temperature_3;n2_vent_valve.pressure_1;n2_vent_valve.pressure_2;n2_vent_valve.battery_voltage;"
            "ox_vent_eth_main_valves.waken_up;ox_vent_eth_main_valves.valve_1_state;ox_vent_eth_main_valves.valve_2_state;ox_vent_eth_main_valves.temperature_1;ox_vent_eth_main_valves.temperature_2;ox_vent_eth_main_valves.temperature_3;ox_vent_eth_main_valves.pressure_1;ox_vent_eth_main_valves.pressure_2;ox_vent_eth_main_valves.battery_voltage;"
            "ox_main_valve.waken_up;ox_main_valve.valve_1_state;ox_main_valve.valve_2_state;ox_main_valve.temperature_1;ox_main_valve.temperature_2;ox_main_valve.temperature_3;ox_main_valve.pressure_1;ox_main_valve.pressure_2;ox_main_valve.battery_voltage;"
            "recovery.isArmed;recovery.isTeleActive;recovery.easyMiniFirstStage;recovery.easyMiniSecondStage;recovery.telemetrumFirstStage;recovery.telemetrumSecondStage;recovery.firstStageDone;recovery.secondStageDone;recovery.firstStageContinouity;recovery.secondStageContinouity;recovery.separationSwitch1;recovery.separationSwitch2;recovery.pressure1;"
            "payload.waken_up;payload.vbat;"
            "tanwa.vbat;tanwa.tanWaState;tanwa.thrust_val;tanwa.tankWeight_val;tanwa.temperature_postFill;tanwa.temperature_Wall;tanwa.postFillN2O_pres;tanwa.cutoffN2O_pres;tanwa.droidN2O_press;tanwa.preRegulatorN2_pres;tanwa.postRegulatorN2_pres;tanwa.postFillN2_pres;tanwa.droidN2_press;tanwa.combChamber_pres;"
            "tanwa.soft_arm;tanwa.canWeighta_con;tanwa.canSensor_con;tanwa.canSolenoid_con;tanwa.canUtility_con;tanwa.canPower_con;tanwa.igniterContinouity_1;tanwa.igniterContinouity_2;tanwa.fillN2OState;tanwa.deprN2OState;tanwa.fillN2State;tanwa.deprN2State;tanwa.droidN2OState;tanwa.droidN2State;tanwa.heatingTankState;tanwa.heatingValveState;tanwa.abortButton;"
            "pitot.waken_up;pitot.dynamic_press;pitot.static_press;pitot.temperature;pitot.humid;pitot.vbat;pitot.speed;pitot.alt;pitot.pred_apogee;"
            "connected_dev.n2_vent_valve;connected_dev.eth_vent_valve;connected_dev.ox_vent_eth_main_valves;connected_dev.ox_main_valve;connected_dev.pitot;connected_dev.payload;connected_dev.tanwa;"
            "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception");
    }

    return string_size;
}

/**
 * @brief Get size of sd data frame
 *
 * @param pysd_main
 * @param new_line_ending end string with new line character
 * @return size_t frame size
 */
size_t pysd_get_sd_frame_size(rocket_data_t pysd_main, bool new_line_ending) {
    size_t string_size = 0;
    if (new_line_ending == true) {
        string_size = snprintf(NULL, 0,
            "%u;%lu;%ld;%lu;%f;%d;%f;%f;%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;"
            "%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%d;%d;%d;%d;%d;%d;"
            "%lu;%lu;%lu;%lu;%lu;%lu\n",
            /* mcb */
            (unsigned)pysd_main.mcb.state,
            pysd_main.mcb.uptime,
            pysd_main.mcb.flight_time,
            pysd_main.mcb.disconnect_timer,
            pysd_main.mcb.battery_voltage,
            pysd_main.mcb.sensor_placeholder,
            pysd_main.mcb.latitude,
            pysd_main.mcb.longitude,
            pysd_main.mcb.gps_altitude,
            (unsigned)pysd_main.mcb.satelites_in_view,
            pysd_main.mcb.acc_x,
            pysd_main.mcb.acc_y,
            pysd_main.mcb.acc_z,
            pysd_main.mcb.acc_vertical,
            pysd_main.mcb.gyr_x,
            pysd_main.mcb.gyr_y,
            pysd_main.mcb.gyr_z,
            pysd_main.mcb.mag_x,
            pysd_main.mcb.mag_y,
            pysd_main.mcb.mag_z,
            pysd_main.mcb.temperature,
            pysd_main.mcb.pressure,
            pysd_main.mcb.altitude,
            pysd_main.mcb.velocity,
            pysd_main.mcb.yaw,
            pysd_main.mcb.pitch,
            pysd_main.mcb.roll,
            /* eth_vent_valve */
            (int)pysd_main.eth_vent_valve.waken_up,
            (unsigned)pysd_main.eth_vent_valve.valve_1_state,
            (unsigned)pysd_main.eth_vent_valve.valve_2_state,
            pysd_main.eth_vent_valve.temperature_1,
            pysd_main.eth_vent_valve.temperature_2,
            pysd_main.eth_vent_valve.temperature_3,
            pysd_main.eth_vent_valve.pressure_1,
            pysd_main.eth_vent_valve.pressure_2,
            pysd_main.eth_vent_valve.battery_voltage,
            /* n2_vent_valve */
            (int)pysd_main.n2_vent_valve.waken_up,
            (unsigned)pysd_main.n2_vent_valve.valve_1_state,
            (unsigned)pysd_main.n2_vent_valve.valve_2_state,
            pysd_main.n2_vent_valve.temperature_1,
            pysd_main.n2_vent_valve.temperature_2,
            pysd_main.n2_vent_valve.temperature_3,
            pysd_main.n2_vent_valve.pressure_1,
            pysd_main.n2_vent_valve.pressure_2,
            pysd_main.n2_vent_valve.battery_voltage,
            /* ox_vent_eth_main_valves */
            (int)pysd_main.ox_vent_eth_main_valves.waken_up,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_1_state,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_2_state,
            pysd_main.ox_vent_eth_main_valves.temperature_1,
            pysd_main.ox_vent_eth_main_valves.temperature_2,
            pysd_main.ox_vent_eth_main_valves.temperature_3,
            pysd_main.ox_vent_eth_main_valves.pressure_1,
            pysd_main.ox_vent_eth_main_valves.pressure_2,
            pysd_main.ox_vent_eth_main_valves.battery_voltage,
            /* ox_main_valve */
            (int)pysd_main.ox_main_valve.waken_up,
            (unsigned)pysd_main.ox_main_valve.valve_1_state,
            (unsigned)pysd_main.ox_main_valve.valve_2_state,
            pysd_main.ox_main_valve.temperature_1,
            pysd_main.ox_main_valve.temperature_2,
            pysd_main.ox_main_valve.temperature_3,
            pysd_main.ox_main_valve.pressure_1,
            pysd_main.ox_main_valve.pressure_2,
            pysd_main.ox_main_valve.battery_voltage,
            /* recovery */
            (int)pysd_main.recovery.isArmed,
            (int)pysd_main.recovery.isTeleActive,
            (int)pysd_main.recovery.easyMiniFirstStage,
            (int)pysd_main.recovery.easyMiniSecondStage,
            (int)pysd_main.recovery.telemetrumFirstStage,
            (int)pysd_main.recovery.telemetrumSecondStage,
            (int)pysd_main.recovery.firstStageDone,
            (int)pysd_main.recovery.secondStageDone,
            (int)pysd_main.recovery.firstStageContinouity,
            (int)pysd_main.recovery.secondStageContinouity,
            (int)pysd_main.recovery.separationSwitch1,
            (int)pysd_main.recovery.separationSwitch2,
            /* payload */
            (int)pysd_main.payload.waken_up,
            pysd_main.payload.vbat,
            /* tanwa */
            pysd_main.tanwa.vbat,
            (unsigned)pysd_main.tanwa.tanWaState,
            pysd_main.tanwa.thrust_val,
            pysd_main.tanwa.tankWeight_val,
            pysd_main.tanwa.temperature_postFill,
            pysd_main.tanwa.temperature_Wall,
            pysd_main.tanwa.postFillN2O_pres,
            pysd_main.tanwa.cutoffN2O_pres,
            pysd_main.tanwa.droidN2O_press,
            pysd_main.tanwa.preRegulatorN2_pres,
            pysd_main.tanwa.postRegulatorN2_pres,
            pysd_main.tanwa.postFillN2_pres,
            pysd_main.tanwa.droidN2_press,
            pysd_main.tanwa.combChamber_pres,
            (int)pysd_main.tanwa.soft_arm,
            (int)pysd_main.tanwa.canWeights_con,
            (int)pysd_main.tanwa.canSensor_con,
            (int)pysd_main.tanwa.canSolenoid_con,
            (int)pysd_main.tanwa.canUtility_con,
            (int)pysd_main.tanwa.canPower_con,
            (int)pysd_main.tanwa.igniterContinouity_1,
            (int)pysd_main.tanwa.igniterContinouity_2,
            (int)pysd_main.tanwa.fillN2OState,
            (int)pysd_main.tanwa.deprN2OState,
            (int)pysd_main.tanwa.fillN2State,
            (int)pysd_main.tanwa.deprN2State,
            (int)pysd_main.tanwa.droidN2OState,
            (int)pysd_main.tanwa.droidN2State,
            (int)pysd_main.tanwa.heatingTankState,
            (int)pysd_main.tanwa.heatingValveState,
            (int)pysd_main.tanwa.abortButton,
            /* pitot */
            (int)pysd_main.pitot.waken_up,
            pysd_main.pitot.dynamic_press,
            pysd_main.pitot.static_press,
            pysd_main.pitot.temperature,
            pysd_main.pitot.humid,
            pysd_main.pitot.vbat,
            pysd_main.pitot.speed,
            pysd_main.pitot.alt,
            pysd_main.pitot.pred_apogee,
            /* connected */
            (int)pysd_main.connected_dev.n2_vent_valve,
            (int)pysd_main.connected_dev.eth_vent_valve,
            (int)pysd_main.connected_dev.ox_vent_eth_main_valves,
            (int)pysd_main.connected_dev.ox_main_valve,
            (int)pysd_main.connected_dev.pitot,
            (int)pysd_main.connected_dev.payload,
            (int)pysd_main.connected_dev.tanwa,
            /* errors */
            pysd_main.error_esp_now,
            pysd_main.error_memory,
            pysd_main.error_mcb,
            pysd_main.error_sensors,
            pysd_main.error_recovery,
            pysd_main.error_last_exception);
    } else {
        string_size = snprintf(NULL, 0,
            "%u;%lu;%ld;%lu;%f;%d;%f;%f;%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;"
            "%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%d;%d;%d;%d;%d;%d;"
            "%lu;%lu;%lu;%lu;%lu;%lu",
            (unsigned)pysd_main.mcb.state,
            pysd_main.mcb.uptime,
            pysd_main.mcb.flight_time,
            pysd_main.mcb.disconnect_timer,
            pysd_main.mcb.battery_voltage,
            pysd_main.mcb.sensor_placeholder,
            pysd_main.mcb.latitude,
            pysd_main.mcb.longitude,
            pysd_main.mcb.gps_altitude,
            (unsigned)pysd_main.mcb.satelites_in_view,
            pysd_main.mcb.acc_x,
            pysd_main.mcb.acc_y,
            pysd_main.mcb.acc_z,
            pysd_main.mcb.acc_vertical,
            pysd_main.mcb.gyr_x,
            pysd_main.mcb.gyr_y,
            pysd_main.mcb.gyr_z,
            pysd_main.mcb.mag_x,
            pysd_main.mcb.mag_y,
            pysd_main.mcb.mag_z,
            pysd_main.mcb.temperature,
            pysd_main.mcb.pressure,
            pysd_main.mcb.altitude,
            pysd_main.mcb.velocity,
            pysd_main.mcb.yaw,
            pysd_main.mcb.pitch,
            pysd_main.mcb.roll,
            /* eth_vent_valve */
            (int)pysd_main.eth_vent_valve.waken_up,
            (unsigned)pysd_main.eth_vent_valve.valve_1_state,
            (unsigned)pysd_main.eth_vent_valve.valve_2_state,
            pysd_main.eth_vent_valve.temperature_1,
            pysd_main.eth_vent_valve.temperature_2,
            pysd_main.eth_vent_valve.temperature_3,
            pysd_main.eth_vent_valve.pressure_1,
            pysd_main.eth_vent_valve.pressure_2,
            pysd_main.eth_vent_valve.battery_voltage,
            /* n2_vent_valve */
            (int)pysd_main.n2_vent_valve.waken_up,
            (unsigned)pysd_main.n2_vent_valve.valve_1_state,
            (unsigned)pysd_main.n2_vent_valve.valve_2_state,
            pysd_main.n2_vent_valve.temperature_1,
            pysd_main.n2_vent_valve.temperature_2,
            pysd_main.n2_vent_valve.temperature_3,
            pysd_main.n2_vent_valve.pressure_1,
            pysd_main.n2_vent_valve.pressure_2,
            pysd_main.n2_vent_valve.battery_voltage,
            /* ox_vent_eth_main_valves */
            (int)pysd_main.ox_vent_eth_main_valves.waken_up,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_1_state,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_2_state,
            pysd_main.ox_vent_eth_main_valves.temperature_1,
            pysd_main.ox_vent_eth_main_valves.temperature_2,
            pysd_main.ox_vent_eth_main_valves.temperature_3,
            pysd_main.ox_vent_eth_main_valves.pressure_1,
            pysd_main.ox_vent_eth_main_valves.pressure_2,
            pysd_main.ox_vent_eth_main_valves.battery_voltage,
            /* ox_main_valve */
            (int)pysd_main.ox_main_valve.waken_up,
            (unsigned)pysd_main.ox_main_valve.valve_1_state,
            (unsigned)pysd_main.ox_main_valve.valve_2_state,
            pysd_main.ox_main_valve.temperature_1,
            pysd_main.ox_main_valve.temperature_2,
            pysd_main.ox_main_valve.temperature_3,
            pysd_main.ox_main_valve.pressure_1,
            pysd_main.ox_main_valve.pressure_2,
            pysd_main.ox_main_valve.battery_voltage,
            /* recovery */
            (int)pysd_main.recovery.isArmed,
            (int)pysd_main.recovery.isTeleActive,
            (int)pysd_main.recovery.easyMiniFirstStage,
            (int)pysd_main.recovery.easyMiniSecondStage,
            (int)pysd_main.recovery.telemetrumFirstStage,
            (int)pysd_main.recovery.telemetrumSecondStage,
            (int)pysd_main.recovery.firstStageDone,
            (int)pysd_main.recovery.secondStageDone,
            (int)pysd_main.recovery.firstStageContinouity,
            (int)pysd_main.recovery.secondStageContinouity,
            (int)pysd_main.recovery.separationSwitch1,
            (int)pysd_main.recovery.separationSwitch2,
            /* payload */
            (int)pysd_main.payload.waken_up,
            pysd_main.payload.vbat,
            /* tanwa */
            pysd_main.tanwa.vbat,
            (unsigned)pysd_main.tanwa.tanWaState,
            pysd_main.tanwa.thrust_val,
            pysd_main.tanwa.tankWeight_val,
            pysd_main.tanwa.temperature_postFill,
            pysd_main.tanwa.temperature_Wall,
            pysd_main.tanwa.postFillN2O_pres,
            pysd_main.tanwa.cutoffN2O_pres,
            pysd_main.tanwa.droidN2O_press,
            pysd_main.tanwa.preRegulatorN2_pres,
            pysd_main.tanwa.postRegulatorN2_pres,
            pysd_main.tanwa.postFillN2_pres,
            pysd_main.tanwa.droidN2_press,
            pysd_main.tanwa.combChamber_pres,
            (int)pysd_main.tanwa.soft_arm,
            (int)pysd_main.tanwa.canWeights_con,
            (int)pysd_main.tanwa.canSensor_con,
            (int)pysd_main.tanwa.canSolenoid_con,
            (int)pysd_main.tanwa.canUtility_con,
            (int)pysd_main.tanwa.canPower_con,
            (int)pysd_main.tanwa.igniterContinouity_1,
            (int)pysd_main.tanwa.igniterContinouity_2,
            (int)pysd_main.tanwa.fillN2OState,
            (int)pysd_main.tanwa.deprN2OState,
            (int)pysd_main.tanwa.fillN2State,
            (int)pysd_main.tanwa.deprN2State,
            (int)pysd_main.tanwa.droidN2OState,
            (int)pysd_main.tanwa.droidN2State,
            (int)pysd_main.tanwa.heatingTankState,
            (int)pysd_main.tanwa.heatingValveState,
            (int)pysd_main.tanwa.abortButton,
            /* pitot */
            (int)pysd_main.pitot.waken_up,
            pysd_main.pitot.dynamic_press,
            pysd_main.pitot.static_press,
            pysd_main.pitot.temperature,
            pysd_main.pitot.humid,
            pysd_main.pitot.vbat,
            pysd_main.pitot.speed,
            pysd_main.pitot.alt,
            pysd_main.pitot.pred_apogee,
            /* connected */
            (int)pysd_main.connected_dev.n2_vent_valve,
            (int)pysd_main.connected_dev.eth_vent_valve,
            (int)pysd_main.connected_dev.ox_vent_eth_main_valves,
            (int)pysd_main.connected_dev.ox_main_valve,
            (int)pysd_main.connected_dev.pitot,
            (int)pysd_main.connected_dev.payload,
            (int)pysd_main.connected_dev.tanwa,
            /* errors */
            pysd_main.error_esp_now,
            pysd_main.error_memory,
            pysd_main.error_mcb,
            pysd_main.error_sensors,
            pysd_main.error_recovery,
            pysd_main.error_last_exception);
    }

    return string_size;
}

/**
 * @brief Fill buffer with struct variables names
 * 
 * @param buffer pointer to buffer
 * @param size buffer size
 * @param rocket_data_t struct with data
 * @param new_line_ending end string with new line character
 * @return size_t size of wrote data, return 0 in case of failure
 */
size_t pysd_create_header(char *buffer, size_t size, bool new_line_ending) {
    size_t header_size = 0;
    /* Create header that matches pysd_create_sd_frame order */
    if (new_line_ending == true) {
        header_size = snprintf(buffer, size,
            "mcb_state;mcb_uptime;mcb_flight_time;mcb_disconnect_timer;mcb_battery_voltage;mcb_sensor_placeholder;mcb_latitude;mcb_longitude;mcb_gps_altitude;mcb_satelites_in_view;mcb_acc_x;mcb_acc_y;mcb_acc_z;mcb_acc_vertical;mcb_gyr_x;mcb_gyr_y;mcb_gyr_z;mcb_mag_x;mcb_mag_y;mcb_mag_z;mcb_temperature;mcb_pressure;mcb_altitude;mcb_velocity;mcb_yaw;mcb_pitch;mcb_roll;"
            "vent_waken_up;vent_valve_1_state;vent_valve_2_state;vent_temperature_1;vent_temperature_2;vent_temperature_3;vent_pressure_1;vent_pressure_2;vent_battery_voltage;"
            "main_waken_up;main_valve_1_state;main_valve_2_state;main_temperature_1;main_temperature_2;main_temperature_3;main_pressure_1;main_pressure_2;main_battery_voltage;"
            "eth_waken_up;eth_valve_1_state;eth_valve_2_state;eth_temperature_1;eth_temperature_2;eth_temperature_3;eth_pressure_1;eth_pressure_2;eth_battery_voltage;"
            "ox_waken_up;ox_valve_1_state;ox_valve_2_state;ox_temperature_1;ox_temperature_2;ox_temperature_3;ox_pressure_1;ox_pressure_2;ox_battery_voltage;"
            "recovery_isArmed;recovery_isTeleActive;recovery_easyMiniFirstStage;recovery_easyMiniSecondStage;recovery_telemetrumFirstStage;recovery_telemetrumSecondStage;recovery_firstStageDone;recovery_secondStageDone;recovery_firstStageContinouity;recovery_secondStageContinouity;recovery_separationSwitch1;recovery_separationSwitch2;recovery_pressure1;"
            "payload_waken_up;payload_vbat;"
            "tanwa_vbat;tanwa_state;tanwa_thrust_val;tanwa_tankWeight_val;tanwa_temperature_postFill;tanwa_temperature_Wall;tanwa_postFillN2O_pres;tanwa_cutoffN2O_pres;tanwa_droidN2O_press;tanwa_preRegulatorN2_pres;tanwa_postRegulatorN2_pres;tanwa_postFillN2_pres;tanwa_droidN2_press;tanwa_combChamber_pres;tanwa_soft_arm;tanwa_canWeighta_con;tanwa_canSensor_con;tanwa_canSolenoid_con;tanwa_canUtility_con;tanwa_canPower_con;tanwa_igniterContinouity_1;tanwa_igniterContinouity_2;tanwa_fillN2OState;tanwa_deprN2OState;tanwa_fillN2State;tanwa_deprN2State;tanwa_droidN2OState;tanwa_droidN2State;tanwa_heatingTankState;tanwa_heatingValveState;tanwa_abortButton;"
            "pitot_waken_up;pitot_dynamic_press;pitot_static_press;pitot_temperature;pitot_humid;pitot_vbat;pitot_speed;pitot_alt;pitot_pred_apogee;"
            "connected_n2_vent_valve;connected_eth_vent_valve;connected_ox_vent_eth_main_valves;connected_ox_main_valve;connected_pitot;connected_payload;connected_tanwa;"
            "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception\n");
    } else {
        header_size = snprintf(buffer, size,
            "mcb_state;mcb_uptime;mcb_flight_time;mcb_disconnect_timer;mcb_battery_voltage;mcb_sensor_placeholder;mcb_latitude;mcb_longitude;mcb_gps_altitude;mcb_satelites_in_view;mcb_acc_x;mcb_acc_y;mcb_acc_z;mcb_acc_vertical;mcb_gyr_x;mcb_gyr_y;mcb_gyr_z;mcb_mag_x;mcb_mag_y;mcb_mag_z;mcb_temperature;mcb_pressure;mcb_altitude;mcb_velocity;mcb_yaw;mcb_pitch;mcb_roll;"
            "vent_waken_up;vent_valve_1_state;vent_valve_2_state;vent_temperature_1;vent_temperature_2;vent_temperature_3;vent_pressure_1;vent_pressure_2;vent_battery_voltage;"
            "main_waken_up;main_valve_1_state;main_valve_2_state;main_temperature_1;main_temperature_2;main_temperature_3;main_pressure_1;main_pressure_2;main_battery_voltage;"
            "eth_waken_up;eth_valve_1_state;eth_valve_2_state;eth_temperature_1;eth_temperature_2;eth_temperature_3;eth_pressure_1;eth_pressure_2;eth_battery_voltage;"
            "ox_waken_up;ox_valve_1_state;ox_valve_2_state;ox_temperature_1;ox_temperature_2;ox_temperature_3;ox_pressure_1;ox_pressure_2;ox_battery_voltage;"
            "recovery_isArmed;recovery_isTeleActive;recovery_easyMiniFirstStage;recovery_easyMiniSecondStage;recovery_telemetrumFirstStage;recovery_telemetrumSecondStage;recovery_firstStageDone;recovery_secondStageDone;recovery_firstStageContinouity;recovery_secondStageContinouity;recovery_separationSwitch1;recovery_separationSwitch2;recovery_pressure1;"
            "payload_waken_up;payload_vbat;"
            "tanwa_vbat;tanwa_state;tanwa_thrust_val;tanwa_tankWeight_val;tanwa_temperature_postFill;tanwa_temperature_Wall;tanwa_postFillN2O_pres;tanwa_cutoffN2O_pres;tanwa_droidN2O_press;tanwa_preRegulatorN2_pres;tanwa_postRegulatorN2_pres;tanwa_postFillN2_pres;tanwa_droidN2_press;tanwa_combChamber_pres;tanwa_soft_arm;tanwa_canWeighta_con;tanwa_canSensor_con;tanwa_canSolenoid_con;tanwa_canUtility_con;tanwa_canPower_con;tanwa_igniterContinouity_1;tanwa_igniterContinouity_2;tanwa_fillN2OState;tanwa_deprN2OState;tanwa_fillN2State;tanwa_deprN2State;tanwa_droidN2OState;tanwa_droidN2State;tanwa_heatingTankState;tanwa_heatingValveState;tanwa_abortButton;"
            "pitot_waken_up;pitot_dynamic_press;pitot_static_press;pitot_temperature;pitot_humid;pitot_vbat;pitot_speed;pitot_alt;pitot_pred_apogee;"
            "connected_n2_vent_valve;connected_eth_vent_valve;connected_ox_vent_eth_main_valves;connected_ox_main_valve;connected_pitot;connected_payload;connected_tanwa;"
            "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception");
    }

    return header_size;
}

/**
 * @brief Fill buffer with struct data
 *
 * @param buffer pointer to buffer
 * @param size buffer size
 * @param ...pysd_main... struct with data
 * @param new_line_ending end string with new line character
 * @return size_t size of wrote data, return 0 in case of failure
 */
size_t pysd_create_sd_frame(char *buffer, size_t size, rocket_data_t pysd_main, bool new_line_ending) {
    size_t frame_size = 0 ;

    if (new_line_ending == true) {
        frame_size = snprintf(buffer, size,
            "%u;%lu;%ld;%lu;%f;%d;%f;%f;%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;"
            "%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%d;%d;%d;%d;%d;%d;"
            "%lu;%lu;%lu;%lu;%lu;%lu\n",
            /* mcb */
            (unsigned)pysd_main.mcb.state,
            pysd_main.mcb.uptime,
            pysd_main.mcb.flight_time,
            pysd_main.mcb.disconnect_timer,
            pysd_main.mcb.battery_voltage,
            pysd_main.mcb.sensor_placeholder,
            pysd_main.mcb.latitude,
            pysd_main.mcb.longitude,
            pysd_main.mcb.gps_altitude,
            (unsigned)pysd_main.mcb.satelites_in_view,
            pysd_main.mcb.acc_x,
            pysd_main.mcb.acc_y,
            pysd_main.mcb.acc_z,
            pysd_main.mcb.acc_vertical,
            pysd_main.mcb.gyr_x,
            pysd_main.mcb.gyr_y,
            pysd_main.mcb.gyr_z,
            pysd_main.mcb.mag_x,
            pysd_main.mcb.mag_y,
            pysd_main.mcb.mag_z,
            pysd_main.mcb.temperature,
            pysd_main.mcb.pressure,
            pysd_main.mcb.altitude,
            pysd_main.mcb.velocity,
            pysd_main.mcb.yaw,
            pysd_main.mcb.pitch,
            pysd_main.mcb.roll,
            /* eth_vent_valve */
            (int)pysd_main.eth_vent_valve.waken_up,
            (unsigned)pysd_main.eth_vent_valve.valve_1_state,
            (unsigned)pysd_main.eth_vent_valve.valve_2_state,
            pysd_main.eth_vent_valve.temperature_1,
            pysd_main.eth_vent_valve.temperature_2,
            pysd_main.eth_vent_valve.temperature_3,
            pysd_main.eth_vent_valve.pressure_1,
            pysd_main.eth_vent_valve.pressure_2,
            pysd_main.eth_vent_valve.battery_voltage,
            /* n2_vent_valve */
            (int)pysd_main.n2_vent_valve.waken_up,
            (unsigned)pysd_main.n2_vent_valve.valve_1_state,
            (unsigned)pysd_main.n2_vent_valve.valve_2_state,
            pysd_main.n2_vent_valve.temperature_1,
            pysd_main.n2_vent_valve.temperature_2,
            pysd_main.n2_vent_valve.temperature_3,
            pysd_main.n2_vent_valve.pressure_1,
            pysd_main.n2_vent_valve.pressure_2,
            pysd_main.n2_vent_valve.battery_voltage,
            /* ox_vent_eth_main_valves */
            (int)pysd_main.ox_vent_eth_main_valves.waken_up,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_1_state,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_2_state,
            pysd_main.ox_vent_eth_main_valves.temperature_1,
            pysd_main.ox_vent_eth_main_valves.temperature_2,
            pysd_main.ox_vent_eth_main_valves.temperature_3,
            pysd_main.ox_vent_eth_main_valves.pressure_1,
            pysd_main.ox_vent_eth_main_valves.pressure_2,
            pysd_main.ox_vent_eth_main_valves.battery_voltage,
            /* ox_main_valve */
            (int)pysd_main.ox_main_valve.waken_up,
            (unsigned)pysd_main.ox_main_valve.valve_1_state,
            (unsigned)pysd_main.ox_main_valve.valve_2_state,
            pysd_main.ox_main_valve.temperature_1,
            pysd_main.ox_main_valve.temperature_2,
            pysd_main.ox_main_valve.temperature_3,
            pysd_main.ox_main_valve.pressure_1,
            pysd_main.ox_main_valve.pressure_2,
            pysd_main.ox_main_valve.battery_voltage,
            /* recovery */
            (int)pysd_main.recovery.isArmed,
            (int)pysd_main.recovery.isTeleActive,
            (int)pysd_main.recovery.easyMiniFirstStage,
            (int)pysd_main.recovery.easyMiniSecondStage,
            (int)pysd_main.recovery.telemetrumFirstStage,
            (int)pysd_main.recovery.telemetrumSecondStage,
            (int)pysd_main.recovery.firstStageDone,
            (int)pysd_main.recovery.secondStageDone,
            (int)pysd_main.recovery.firstStageContinouity,
            (int)pysd_main.recovery.secondStageContinouity,
            (int)pysd_main.recovery.separationSwitch1,
            (int)pysd_main.recovery.separationSwitch2,
            /* payload */
            (int)pysd_main.payload.waken_up,
            pysd_main.payload.vbat,
            /* tanwa */
            pysd_main.tanwa.vbat,
            (unsigned)pysd_main.tanwa.tanWaState,
            pysd_main.tanwa.thrust_val,
            pysd_main.tanwa.tankWeight_val,
            pysd_main.tanwa.temperature_postFill,
            pysd_main.tanwa.temperature_Wall,
            pysd_main.tanwa.postFillN2O_pres,
            pysd_main.tanwa.cutoffN2O_pres,
            pysd_main.tanwa.droidN2O_press,
            pysd_main.tanwa.preRegulatorN2_pres,
            pysd_main.tanwa.postRegulatorN2_pres,
            pysd_main.tanwa.postFillN2_pres,
            pysd_main.tanwa.droidN2_press,
            pysd_main.tanwa.combChamber_pres,
            (int)pysd_main.tanwa.soft_arm,
            (int)pysd_main.tanwa.canWeights_con,
            (int)pysd_main.tanwa.canSensor_con,
            (int)pysd_main.tanwa.canSolenoid_con,
            (int)pysd_main.tanwa.canUtility_con,
            (int)pysd_main.tanwa.canPower_con,
            (int)pysd_main.tanwa.igniterContinouity_1,
            (int)pysd_main.tanwa.igniterContinouity_2,
            (int)pysd_main.tanwa.fillN2OState,
            (int)pysd_main.tanwa.deprN2OState,
            (int)pysd_main.tanwa.fillN2State,
            (int)pysd_main.tanwa.deprN2State,
            (int)pysd_main.tanwa.droidN2OState,
            (int)pysd_main.tanwa.droidN2State,
            (int)pysd_main.tanwa.heatingTankState,
            (int)pysd_main.tanwa.heatingValveState,
            (int)pysd_main.tanwa.abortButton,
            /* pitot */
            (int)pysd_main.pitot.waken_up,
            pysd_main.pitot.dynamic_press,
            pysd_main.pitot.static_press,
            pysd_main.pitot.temperature,
            pysd_main.pitot.humid,
            pysd_main.pitot.vbat,
            pysd_main.pitot.speed,
            pysd_main.pitot.alt,
            pysd_main.pitot.pred_apogee,
            /* connected */
            (int)pysd_main.connected_dev.n2_vent_valve,
            (int)pysd_main.connected_dev.eth_vent_valve,
            (int)pysd_main.connected_dev.ox_vent_eth_main_valves,
            (int)pysd_main.connected_dev.ox_main_valve,
            (int)pysd_main.connected_dev.pitot,
            (int)pysd_main.connected_dev.payload,
            (int)pysd_main.connected_dev.tanwa,
            /* errors */
            pysd_main.error_esp_now,
            pysd_main.error_memory,
            pysd_main.error_mcb,
            pysd_main.error_sensors,
            pysd_main.error_recovery,
            pysd_main.error_last_exception);
    } else {
        frame_size = snprintf(buffer, size,
            "%u;%lu;%ld;%lu;%f;%d;%f;%f;%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%u;%u;%d;%d;%d;%u;%u;%f;"
            "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;"
            "%f;%u;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;"
            "%d;%f;%f;%f;%f;%f;%f;%f;%f;"
            "%d;%d;%d;%d;%d;%d;%d;"
            "%lu;%lu;%lu;%lu;%lu;%lu",

            (unsigned)pysd_main.mcb.state,
            pysd_main.mcb.uptime,
            pysd_main.mcb.flight_time,
            pysd_main.mcb.disconnect_timer,
            pysd_main.mcb.battery_voltage,
            pysd_main.mcb.sensor_placeholder,
            pysd_main.mcb.latitude,
            pysd_main.mcb.longitude,
            pysd_main.mcb.gps_altitude,
            (unsigned)pysd_main.mcb.satelites_in_view,
            pysd_main.mcb.acc_x,
            pysd_main.mcb.acc_y,
            pysd_main.mcb.acc_z,
            pysd_main.mcb.acc_vertical,
            pysd_main.mcb.gyr_x,
            pysd_main.mcb.gyr_y,
            pysd_main.mcb.gyr_z,
            pysd_main.mcb.mag_x,
            pysd_main.mcb.mag_y,
            pysd_main.mcb.mag_z,
            pysd_main.mcb.temperature,
            pysd_main.mcb.pressure,
            pysd_main.mcb.altitude,
            pysd_main.mcb.velocity,
            pysd_main.mcb.yaw,
            pysd_main.mcb.pitch,
            pysd_main.mcb.roll,
            /* eth_vent_valve */
            (int)pysd_main.eth_vent_valve.waken_up,
            (unsigned)pysd_main.eth_vent_valve.valve_1_state,
            (unsigned)pysd_main.eth_vent_valve.valve_2_state,
            pysd_main.eth_vent_valve.temperature_1,
            pysd_main.eth_vent_valve.temperature_2,
            pysd_main.eth_vent_valve.temperature_3,
            pysd_main.eth_vent_valve.pressure_1,
            pysd_main.eth_vent_valve.pressure_2,
            pysd_main.eth_vent_valve.battery_voltage,
            /* n2_vent_valve */
            (int)pysd_main.n2_vent_valve.waken_up,
            (unsigned)pysd_main.n2_vent_valve.valve_1_state,
            (unsigned)pysd_main.n2_vent_valve.valve_2_state,
            pysd_main.n2_vent_valve.temperature_1,
            pysd_main.n2_vent_valve.temperature_2,
            pysd_main.n2_vent_valve.temperature_3,
            pysd_main.n2_vent_valve.pressure_1,
            pysd_main.n2_vent_valve.pressure_2,
            pysd_main.n2_vent_valve.battery_voltage,
            /* ox_vent_eth_main_valves */
            (int)pysd_main.ox_vent_eth_main_valves.waken_up,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_1_state,
            (unsigned)pysd_main.ox_vent_eth_main_valves.valve_2_state,
            pysd_main.ox_vent_eth_main_valves.temperature_1,
            pysd_main.ox_vent_eth_main_valves.temperature_2,
            pysd_main.ox_vent_eth_main_valves.temperature_3,
            pysd_main.ox_vent_eth_main_valves.pressure_1,
            pysd_main.ox_vent_eth_main_valves.pressure_2,
            pysd_main.ox_vent_eth_main_valves.battery_voltage,
            /* ox_main_valve */
            (int)pysd_main.ox_main_valve.waken_up,
            (unsigned)pysd_main.ox_main_valve.valve_1_state,
            (unsigned)pysd_main.ox_main_valve.valve_2_state,
            pysd_main.ox_main_valve.temperature_1,
            pysd_main.ox_main_valve.temperature_2,
            pysd_main.ox_main_valve.temperature_3,
            pysd_main.ox_main_valve.pressure_1,
            pysd_main.ox_main_valve.pressure_2,
            pysd_main.ox_main_valve.battery_voltage,
            /* recovery */
            (int)pysd_main.recovery.isArmed,
            (int)pysd_main.recovery.isTeleActive,
            (int)pysd_main.recovery.easyMiniFirstStage,
            (int)pysd_main.recovery.easyMiniSecondStage,
            (int)pysd_main.recovery.telemetrumFirstStage,
            (int)pysd_main.recovery.telemetrumSecondStage,
            (int)pysd_main.recovery.firstStageDone,
            (int)pysd_main.recovery.secondStageDone,
            (int)pysd_main.recovery.firstStageContinouity,
            (int)pysd_main.recovery.secondStageContinouity,
            (int)pysd_main.recovery.separationSwitch1,
            (int)pysd_main.recovery.separationSwitch2,
            /* payload */
            (int)pysd_main.payload.waken_up,
            pysd_main.payload.vbat,
            /* tanwa */
            pysd_main.tanwa.vbat,
            (unsigned)pysd_main.tanwa.tanWaState,
            pysd_main.tanwa.thrust_val,
            pysd_main.tanwa.tankWeight_val,
            pysd_main.tanwa.temperature_postFill,
            pysd_main.tanwa.temperature_Wall,
            pysd_main.tanwa.postFillN2O_pres,
            pysd_main.tanwa.cutoffN2O_pres,
            pysd_main.tanwa.droidN2O_press,
            pysd_main.tanwa.preRegulatorN2_pres,
            pysd_main.tanwa.postRegulatorN2_pres,
            pysd_main.tanwa.postFillN2_pres,
            pysd_main.tanwa.droidN2_press,
            pysd_main.tanwa.combChamber_pres,
            (int)pysd_main.tanwa.soft_arm,
            (int)pysd_main.tanwa.canWeights_con,
            (int)pysd_main.tanwa.canSensor_con,
            (int)pysd_main.tanwa.canSolenoid_con,
            (int)pysd_main.tanwa.canUtility_con,
            (int)pysd_main.tanwa.canPower_con,
            (int)pysd_main.tanwa.igniterContinouity_1,
            (int)pysd_main.tanwa.igniterContinouity_2,
            (int)pysd_main.tanwa.fillN2OState,
            (int)pysd_main.tanwa.deprN2OState,
            (int)pysd_main.tanwa.fillN2State,
            (int)pysd_main.tanwa.deprN2State,
            (int)pysd_main.tanwa.droidN2OState,
            (int)pysd_main.tanwa.droidN2State,
            (int)pysd_main.tanwa.heatingTankState,
            (int)pysd_main.tanwa.heatingValveState,
            (int)pysd_main.tanwa.abortButton,
            /* pitot */
            (int)pysd_main.pitot.waken_up,
            pysd_main.pitot.dynamic_press,
            pysd_main.pitot.static_press,
            pysd_main.pitot.temperature,
            pysd_main.pitot.humid,
            pysd_main.pitot.vbat,
            pysd_main.pitot.speed,
            pysd_main.pitot.alt,
            pysd_main.pitot.pred_apogee,
            /* connected */
            (int)pysd_main.connected_dev.n2_vent_valve,
            (int)pysd_main.connected_dev.eth_vent_valve,
            (int)pysd_main.connected_dev.ox_vent_eth_main_valves,
            (int)pysd_main.connected_dev.ox_main_valve,
            (int)pysd_main.connected_dev.pitot,
            (int)pysd_main.connected_dev.payload,
            (int)pysd_main.connected_dev.tanwa,
            /* errors */
            pysd_main.error_esp_now,
            pysd_main.error_memory,
            pysd_main.error_mcb,
            pysd_main.error_sensors,
            pysd_main.error_recovery,
            pysd_main.error_last_exception);
    }

    return frame_size;
}