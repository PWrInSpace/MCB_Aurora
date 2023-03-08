#ifndef STATE_MACHINE_WRAPPER_H
#define STATE_MACHINE_WRAPPER_H

#include "state_machine.h"

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
} STATE;

/**
 * @brief Get the states config object
 *
 * @param cfg pointer to overwrite by pointer to config
 *             (create state_config_t * and pass as argument)
 * @param nb_of_states number of states
 */
void SMW_get_states_config(state_config_t **cfg, uint8_t *nb_of_states);

#endif