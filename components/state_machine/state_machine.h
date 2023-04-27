// Copyright 2022 PWrInSpace

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum {
    SM_OK,
    SM_STATES_ERROR,
    SM_STATES_NUMBER_ERROR,
    SM_STATE_CHANGE_ERROR,
    SM_NO_MORE_STATES,
    SM_INVALID_STATE,
    SM_RUN_ERROR,
    SM_NULL_FUNCTION,
} SM_Response;

typedef void (*state_change_callback)(void *arg);
typedef void (*end_looped_function)(void);
typedef uint8_t state_id;

typedef struct {
    state_id id;
    state_change_callback callback;
    void *arg;
} state_config_t;

typedef struct {
    uint32_t stack_depth;
    BaseType_t core_id;
    UBaseType_t priority;
} state_machine_task_cfg_t;

void SM_init(void);
SM_Response SM_set_states(state_config_t *states, uint8_t states_number);
SM_Response SM_set_end_function(end_looped_function fct, uint32_t freq_ms);
state_id SM_get_current_state(void);
state_id SM_get_previous_state(void);
SM_Response SM_change_state(state_id new_state);
SM_Response SM_force_change_state(state_id new_state);
SM_Response SM_change_to_previous_state(bool run_callback);
SM_Response SM_change_state_ISR(state_id new_state);
SM_Response SM_run(state_machine_task_cfg_t *cfg);
SM_Response SM_destroy(void);




#endif

