#ifndef BMI088_CONFIG_H
#define BMI088_CONFIG_H
#include "bmi08_defs.h"
#include <stdbool.h>

bool bmi08_wrapper_init(void);

bool bmi08_acc_data_ready(void);
bool bmi08_gyro_data_ready(void);
bool bmi08_get_acc_data(struct bmi08_sensor_data_f *acc);
bool bmi08_get_gyro_data(struct bmi08_sensor_data_f *gyro);
bool bmi08_get_raw_gyro_data(int16_t *x, int16_t *y, int16_t *z);

#endif