// Copyright 2022 PWrInSpace, Kuba
#ifndef SLAVE_STRUCTS_H
#define SLAVE_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>

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
    uint16_t thermistor1;
    uint16_t thermistor2;
    float tank_pressure;
    float battery_voltage;
} vent_valve_data_t;

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

    uint16_t pressure1;
    uint16_t pressure2;
} recovery_data_t;

typedef struct {
    bool waken_up : 1;
    uint16_t vbat;
} payload_data_t;

typedef struct {
    uint32_t tanWaState;
    uint32_t pressureSensor;
    uint32_t solenoid_fill;
    uint32_t solenoid_depr;
    bool abortButton;
    bool igniterContinouity_1;
    bool igniterContinouity_2;
    uint32_t hxRequest_RCK; /* arduino string */
    uint32_t hxRequest_TANK; /* arduino string */
    float vbat;
    uint32_t motorState_1;
    uint32_t motorState_2;
    uint32_t motorState_3;
    uint32_t motorState_4;
    float rocketWeight_val;
    float tankWeight_val;
    uint32_t rocketWeightRaw_val;
    uint32_t tankWeightRaw_val;
    bool interface_rck;
    bool interface_tank;
    bool interface_mcu;
} tanwa_data_t;

typedef struct {
    bool wakenUp;
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
