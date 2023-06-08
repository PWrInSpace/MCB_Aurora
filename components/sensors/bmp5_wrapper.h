#ifndef BMP5_CONFIG_H
#define BMP5_CONFIG_H
#include "bmp5_defs.h"
#include <stdbool.h>

bool bmp5_wrapper_init(void);
bool bmp5_wrapper_get_data(struct bmp5_sensor_data *data);
float bmp5_wrapper_altitude(float pressure, float altitude_offset);

#endif