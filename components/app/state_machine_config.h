// Copyright 2022 PWrInSpace, Kuba
#ifndef STATE_MACHINE_WRAPPER_H
#define STATE_MACHINE_WRAPPER_H

#include "state_machine.h"
#include "stdbool.h"

typedef enum {
    INIT = 0,
    IDLE,
    RECOVERY_ARM,
    FUELING,
    ARMED_TO_LAUNCH,
    RDY_TO_LAUNCH,
    COUNTDOWN,
    FLIGHT,
    FIRST_STAGE_RECOVERY,
    SECOND_STAGE_RECOVERY,
    ON_GROUND,
    HOLD,
    ABORT,
} states_t;

bool initialize_state_machine(void);

#endif
