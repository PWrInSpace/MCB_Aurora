// Copyright 2022 PWrInSpace, Kuba
#ifndef SLAVE_STRUCTS_H
#define SLAVE_STRUCTS_H

#include <stdint.h>

typedef struct {
    bool waken_up :1;
    uint8_t valve_state : 2;
    float thermocouple1;
    float thermocouple2;
    float battery_voltage;
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

#endif
