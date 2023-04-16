#include "mcb_structs.h"
#include "state_machine.h"
#include "utils.h"

bool mcb_update_struct(mcb_data_t *mcb) {
    mcb->state = SM_get_current_state();
    mcb->uptime = get_uptime_ms();
    // mcb->battery_voltage = get
    // get data from sensor task

    return true;
}