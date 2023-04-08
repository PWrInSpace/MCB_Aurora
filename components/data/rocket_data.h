#ifndef ROCKET_DATA_H
#define ROCKET_DATA_H

#include "slave_structs.h"
#include "mcb_structs.h"

typedef struct {
    mcb_data_t mcb;
    vent_valve_data_t vent_valve;
    main_valve_data_t main_valve;
    recovery_data_t  recovery;
} rocket_data_t;

bool rocket_data_init(void);

void rocket_data_update_main_valve(main_valve_data_t *data);

void rocket_data_update_vent_valve(vent_valve_data_t *data);

void rocket_data_update_recovery(recovery_data_t *data);

void rocket_data_update_mcb(mcb_data_t *data);

rocket_data_t rocket_data_get(void);

mcb_data_t rocket_data_get_mcb(void);

main_valve_data_t rocket_data_get_main_valve(void);

vent_valve_data_t rocket_data_get_vent_valve(void);

recovery_data_t rocket_data_get_recovery(void);

#endif