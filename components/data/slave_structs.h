// Copyright 2022 PWrInSpace, Kuba
#ifndef SLAVE_STRUCTS_H
#define SLAVE_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>
#include "protobuf-c.h"

typedef struct {
    bool main_valve : 1;    
    bool vent_valve : 1;
    bool pitot : 1;
    bool payload : 1;
    bool tanwa : 1;
} esp_now_connected_devices_t;

typedef struct {
    bool waken_up :1;
    uint8_t valve_state : 2;
    uint32_t thermocouple1;
    uint32_t thermocouple2;
    uint32_t battery_voltage;
} main_valve_data_t;

typedef struct {
    bool waken_up : 1;
    uint8_t valve_state : 2;
    uint32_t tank_pressure;
    int8_t thermistor1;
    int8_t thermistor2;
    uint32_t battery_voltage;
} vent_valve_data_t;

typedef struct {
    bool isArmed :1; // Easymini arming signall
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

    uint16_t pressure1;
    uint16_t pressure2;
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
    float rocketWeight_val;
    float tankWeight_val;
    float fill_temp;
    float preFill_pres;
    float postFill_pres;
    float tank_pres;
    bool canHxBtl_con : 1;
    bool canHxRck_con : 1;
    bool canFac_con : 1;
    bool canFlc_con : 1;
    bool canTermo_con : 1;
    bool igniterContinouity_1 : 1;
    bool igniterContinouity_2 : 1;
    bool limitSwitch_1 : 1;
    bool limitSwitch_2 : 1;
    bool fillState : 1;
    bool deprState : 1;
    uint8_t facMotorState_1;
    uint8_t facMotorState_2;
    bool coolingState : 1;
    bool heatingState : 1;
    bool abortButton : 1;
}tanwa_data_t;

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
