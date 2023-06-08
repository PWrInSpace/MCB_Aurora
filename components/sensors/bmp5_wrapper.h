#ifndef BMP5_CONFIG_H
#define BMP5_CONFIG_H
#include "bmp5_defs.h"
#include <stdbool.h>

#define BMP5_AVERAGE_PRESSURE 1001.67f
#define BMP5_CALIBRATE_NB_OF_MEAS 10

#define BMP5_Pa_TO_hPa(x) x / 100.0

bool bmp5_wrapper_init(void);
bool bmp5_wrapper_get_data(struct bmp5_sensor_data *data);
bool bmp5_calculate_altitude_offset(void);
float bmp5_wrapper_altitude(float pressure);

#endif