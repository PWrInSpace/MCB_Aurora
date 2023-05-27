#ifndef MAG_WRAPPER_H
#define MAG_WRAPPER_H

#include <stdbool.h>
#include "mmc5983.h"

bool mag_init(void);
bool mag_set_continous_mode(mmc5983_freq_t freq, mmc5983_prd_set_t prd);
bool mag_data_ready();
bool mag_get_data(mmc5983_mag_t *mag);

#endif