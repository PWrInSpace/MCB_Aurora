// Copyright 2022 PWrInSpace, Kuba
#ifndef SLAVE_STRUCTS_H
#define SLAVE_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool n2_vent_valve : 1;
    bool eth_vent_valve : 1;
    bool ox_vent_eth_main_valves : 1;
    bool ox_main_valve : 1;
    bool pitot : 1;
    bool payload : 1;
    bool tanwa : 1;
} esp_now_connected_devices_t;

typedef struct {
    bool waken_up : 1;
    bool dump_valve_arm : 1;
    bool dump_valve_cont: 1;
    bool is_charging : 1;
    uint8_t valve_1_state : 2;
    uint8_t valve_2_state : 2;
    int16_t temperature_1;
    float pressure_1;
    float pressure_2;
    float battery_voltage;
    float battery_consumption;
    float charger_temperature;
} n2_vent_valve_data_t;

typedef struct {
    bool waken_up : 1;
    bool dump_valve_arm : 1;
    bool dump_valve_cont: 1;
    bool is_charging : 1;
    uint8_t valve_1_state : 2;
    uint8_t valve_2_state : 2;
    int16_t temperature_1;
    float pressure_1;
    float pressure_2;
    float battery_voltage;
    float battery_consumption;
    float charger_temperature;
} eth_vent_valve_data_t;

typedef struct {
    bool waken_up : 1;
    bool dump_valve_arm : 1;
    bool dump_valve_cont: 1;
    bool is_charging : 1;
    bool auto_vent_activated: 1;
    bool auto_vent_triggered: 1;
    int16_t ox_temperature;
    uint8_t valve_1_state : 2;
    uint8_t valve_2_state : 2;
    int16_t temperature_1;
    float pressure_1;
    float pressure_2;
    float battery_voltage;
    float battery_consumption;
    float charger_temperature;
} ox_vent_eth_main_valves_data_t;

typedef struct {
    bool waken_up : 1;
    bool dump_valve_arm : 1;
    bool dump_valve_cont: 1;
    bool is_charging : 1;
    uint8_t valve_1_state : 2;
    uint8_t valve_2_state : 2;
    int16_t temperature_1;
    float pressure_1;
    float pressure_2;
    float battery_voltage;
    float battery_consumption;
    float charger_temperature;
} ox_main_valve_data_t;

typedef struct {
    bool isArmed :1;
    bool isTeleActive :1; 
    bool easyMiniFirstStage :1;
    bool easyMiniSecondStage :1;
    bool telemetrumFirstStage :1;
    bool telemetrumSecondStage :1;
    bool firstStageDone :1;
    bool secondStageDone :1;
    bool firstStageContinouity :1;
    bool secondStageContinouity :1;
    bool separationSwitch1 :1;
    bool separationSwitch2 :1;
} recovery_data_t;

typedef struct {
    bool waken_up : 1;
    float vbat;
} payload_data_t;

// typedef struct {
//     uint8_t tanWaState;
//     uint16_t pressureSensor;
//     uint16_t solenoid_fill; 
//     uint16_t solenoid_depr; 
//     bool abortButton : 1;
//     bool igniterContinouity_1;
//     bool igniterContinouity_2;
//     uint8_t hxRequest_RCK;
//     uint8_t hxRequest_TANK;
//     float vbat;
//     uint8_t motorState_1;
//     uint8_t motorState_2;
//     uint8_t motorState_3;
//     uint8_t motorState_4;
//     int rocketWeight_blink;
//     float rocketWeight_temp;
//     int tankWeight_blink;
//     float tankWeight_temp;
//     float rocketWeight_val;
//     float tankWeight_val;
//     uint32_t rocketWeightRaw_val;
//     uint32_t tankWeightRaw_val;
//     bool interface_rck;
//     bool interface_tank;
//     bool interface_mcu;
// } tanwa_data_t_old;

typedef struct{
    float vbat;
    uint8_t tanWaState;
    float thrust_val;
    float tankWeight_val;
    float temperature_postFill;
    float temperature_Wall;
    float postFillN2O_pres;
    float cutoffN2O_pres;
    float droidN2O_press;
    float preRegulatorN2_pres;
    float postRegulatorN2_pres;
    float postFillN2_pres;
    float droidN2_press;
    float combChamber_pres;
    bool soft_arm : 1;
    bool canWeights_con : 1;
    bool canSensor_con : 1;
    bool canSolenoid_con : 1;
    bool canUtility_con : 1;
    bool canPower_con : 1;
    bool igniterContinouity_1 : 1;
    bool igniterContinouity_2 : 1;
    bool fillN2OState : 1;
    bool deprN2OState : 1;
    bool fillN2State : 1;
    bool deprN2State : 1;
    bool droidN2OState : 1;
    bool droidN2State : 1;
    bool heatingTankState : 1;
    bool heatingValveState : 1;
    bool abortButton : 1;
} tanwa_data_t;

typedef struct {
    bool waken_up;
    float dynamic_press;
    float static_press;
    float temperature;
    float humid;
    float vbat;
    float speed;
    float alt;
    float pred_apogee;
} pitot_data_t;

#endif
