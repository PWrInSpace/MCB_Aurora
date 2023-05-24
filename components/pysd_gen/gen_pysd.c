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
        string_size = snprintf(NULL, 0, "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception;mcb state;mcb uptime;mcb flight_time;mcb disconnect_timer;mcb battery_voltage;mcb sensor_placeholder;mcb latitude;mcb longitude;mcb altitude;mcb satelites_in_view;vent_valve waken_up;vent_valve valve_state;vent_valve thermistor1;vent_valve thermistor2;vent_valve tank_pressure;vent_valve battery_voltage;main_valve waken_up;main_valve valve_state;main_valve thermocouple1;main_valve thermocouple2;main_valve battery_voltage;recovery isArmed;recovery isTeleActive;recovery easyMiniFirstStage;recovery easyMiniSecondStage;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery firstStageDone;recovery secondStageDone;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery pressure1;recovery pressure2;\n");
    } else {
        string_size = snprintf(NULL, 0, "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception;mcb state;mcb uptime;mcb flight_time;mcb disconnect_timer;mcb battery_voltage;mcb sensor_placeholder;mcb latitude;mcb longitude;mcb altitude;mcb satelites_in_view;vent_valve waken_up;vent_valve valve_state;vent_valve thermistor1;vent_valve thermistor2;vent_valve tank_pressure;vent_valve battery_voltage;main_valve waken_up;main_valve valve_state;main_valve thermocouple1;main_valve thermocouple2;main_valve battery_voltage;recovery isArmed;recovery isTeleActive;recovery easyMiniFirstStage;recovery easyMiniSecondStage;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery firstStageDone;recovery secondStageDone;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery pressure1;recovery pressure2;");
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
    size_t string_size;
    if (new_line_ending == true) {
        string_size = snprintf(NULL, 0, "%lu;%lu;%lu;%lu;%lu;%lu;%d;%lu;%d;%lu;%f;%d;%f;%f;%f;%d;%d;%d;%d;%d;%f;%f;%d;%d;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;\n", pysd_main.error_esp_now, pysd_main.error_memory, pysd_main.error_mcb, pysd_main.error_sensors, pysd_main.error_recovery, pysd_main.error_last_exception, pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.flight_time, pysd_main.mcb.disconnect_timer, pysd_main.mcb.battery_voltage, pysd_main.mcb.sensor_placeholder, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.altitude, pysd_main.mcb.satelites_in_view, pysd_main.vent_valve.waken_up, pysd_main.vent_valve.valve_state, pysd_main.vent_valve.thermistor1, pysd_main.vent_valve.thermistor2, pysd_main.vent_valve.tank_pressure, pysd_main.vent_valve.battery_voltage, pysd_main.main_valve.waken_up, pysd_main.main_valve.valve_state, pysd_main.main_valve.thermocouple1, pysd_main.main_valve.thermocouple2, pysd_main.main_valve.battery_voltage, pysd_main.recovery.isArmed, pysd_main.recovery.isTeleActive, pysd_main.recovery.easyMiniFirstStage, pysd_main.recovery.easyMiniSecondStage, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.pressure1, pysd_main.recovery.pressure2);
    } else {
        string_size = snprintf(NULL, 0, "%lu;%lu;%lu;%lu;%lu;%lu;%d;%lu;%d;%lu;%f;%d;%f;%f;%f;%d;%d;%d;%d;%d;%f;%f;%d;%d;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;", pysd_main.error_esp_now, pysd_main.error_memory, pysd_main.error_mcb, pysd_main.error_sensors, pysd_main.error_recovery, pysd_main.error_last_exception, pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.flight_time, pysd_main.mcb.disconnect_timer, pysd_main.mcb.battery_voltage, pysd_main.mcb.sensor_placeholder, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.altitude, pysd_main.mcb.satelites_in_view, pysd_main.vent_valve.waken_up, pysd_main.vent_valve.valve_state, pysd_main.vent_valve.thermistor1, pysd_main.vent_valve.thermistor2, pysd_main.vent_valve.tank_pressure, pysd_main.vent_valve.battery_voltage, pysd_main.main_valve.waken_up, pysd_main.main_valve.valve_state, pysd_main.main_valve.thermocouple1, pysd_main.main_valve.thermocouple2, pysd_main.main_valve.battery_voltage, pysd_main.recovery.isArmed, pysd_main.recovery.isTeleActive, pysd_main.recovery.easyMiniFirstStage, pysd_main.recovery.easyMiniSecondStage, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.pressure1, pysd_main.recovery.pressure2);
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

    if (new_line_ending == true) {
        header_size = snprintf(buffer, size, "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception;mcb state;mcb uptime;mcb flight_time;mcb disconnect_timer;mcb battery_voltage;mcb sensor_placeholder;mcb latitude;mcb longitude;mcb altitude;mcb satelites_in_view;vent_valve waken_up;vent_valve valve_state;vent_valve thermistor1;vent_valve thermistor2;vent_valve tank_pressure;vent_valve battery_voltage;main_valve waken_up;main_valve valve_state;main_valve thermocouple1;main_valve thermocouple2;main_valve battery_voltage;recovery isArmed;recovery isTeleActive;recovery easyMiniFirstStage;recovery easyMiniSecondStage;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery firstStageDone;recovery secondStageDone;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery pressure1;recovery pressure2;\n");
    } else {
        header_size = snprintf(buffer, size, "error_esp_now;error_memory;error_mcb;error_sensors;error_recovery;error_last_exception;mcb state;mcb uptime;mcb flight_time;mcb disconnect_timer;mcb battery_voltage;mcb sensor_placeholder;mcb latitude;mcb longitude;mcb altitude;mcb satelites_in_view;vent_valve waken_up;vent_valve valve_state;vent_valve thermistor1;vent_valve thermistor2;vent_valve tank_pressure;vent_valve battery_voltage;main_valve waken_up;main_valve valve_state;main_valve thermocouple1;main_valve thermocouple2;main_valve battery_voltage;recovery isArmed;recovery isTeleActive;recovery easyMiniFirstStage;recovery easyMiniSecondStage;recovery telemetrumFirstStage;recovery telemetrumSecondStage;recovery firstStageDone;recovery secondStageDone;recovery firstStageContinouity;recovery secondStageContinouity;recovery separationSwitch1;recovery separationSwitch2;recovery pressure1;recovery pressure2;");
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
        frame_size = snprintf(buffer, size, "%lu;%lu;%lu;%lu;%lu;%lu;%d;%lu;%d;%lu;%f;%d;%f;%f;%f;%d;%d;%d;%d;%d;%f;%f;%d;%d;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;\n", pysd_main.error_esp_now, pysd_main.error_memory, pysd_main.error_mcb, pysd_main.error_sensors, pysd_main.error_recovery, pysd_main.error_last_exception, pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.flight_time, pysd_main.mcb.disconnect_timer, pysd_main.mcb.battery_voltage, pysd_main.mcb.sensor_placeholder, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.altitude, pysd_main.mcb.satelites_in_view, pysd_main.vent_valve.waken_up, pysd_main.vent_valve.valve_state, pysd_main.vent_valve.thermistor1, pysd_main.vent_valve.thermistor2, pysd_main.vent_valve.tank_pressure, pysd_main.vent_valve.battery_voltage, pysd_main.main_valve.waken_up, pysd_main.main_valve.valve_state, pysd_main.main_valve.thermocouple1, pysd_main.main_valve.thermocouple2, pysd_main.main_valve.battery_voltage, pysd_main.recovery.isArmed, pysd_main.recovery.isTeleActive, pysd_main.recovery.easyMiniFirstStage, pysd_main.recovery.easyMiniSecondStage, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.pressure1, pysd_main.recovery.pressure2);
    } else {
        frame_size = snprintf(buffer, size, "%lu;%lu;%lu;%lu;%lu;%lu;%d;%lu;%d;%lu;%f;%d;%f;%f;%f;%d;%d;%d;%d;%d;%f;%f;%d;%d;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;", pysd_main.error_esp_now, pysd_main.error_memory, pysd_main.error_mcb, pysd_main.error_sensors, pysd_main.error_recovery, pysd_main.error_last_exception, pysd_main.mcb.state, pysd_main.mcb.uptime, pysd_main.mcb.flight_time, pysd_main.mcb.disconnect_timer, pysd_main.mcb.battery_voltage, pysd_main.mcb.sensor_placeholder, pysd_main.mcb.latitude, pysd_main.mcb.longitude, pysd_main.mcb.altitude, pysd_main.mcb.satelites_in_view, pysd_main.vent_valve.waken_up, pysd_main.vent_valve.valve_state, pysd_main.vent_valve.thermistor1, pysd_main.vent_valve.thermistor2, pysd_main.vent_valve.tank_pressure, pysd_main.vent_valve.battery_voltage, pysd_main.main_valve.waken_up, pysd_main.main_valve.valve_state, pysd_main.main_valve.thermocouple1, pysd_main.main_valve.thermocouple2, pysd_main.main_valve.battery_voltage, pysd_main.recovery.isArmed, pysd_main.recovery.isTeleActive, pysd_main.recovery.easyMiniFirstStage, pysd_main.recovery.easyMiniSecondStage, pysd_main.recovery.telemetrumFirstStage, pysd_main.recovery.telemetrumSecondStage, pysd_main.recovery.firstStageDone, pysd_main.recovery.secondStageDone, pysd_main.recovery.firstStageContinouity, pysd_main.recovery.secondStageContinouity, pysd_main.recovery.separationSwitch1, pysd_main.recovery.separationSwitch2, pysd_main.recovery.pressure1, pysd_main.recovery.pressure2);
    }

    return frame_size;
}