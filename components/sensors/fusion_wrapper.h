#ifndef FUSION_WRAPPER_H
#define FUSION_WRAPPER_H

#include "Fusion.h"

typedef struct {
    FusionQuaternion quaternion;
    FusionVector acceleration;
    FusionVector acceleration_earth;
} fusion_data_t;

bool fusion_wrapper_init(FusionAhrs *ahrs);

// bool fusion_wrapper_update_settings(FusionAhrs *ahrs, float gain, float accel_rejection, float mag_rejection);

bool fusion_wrapper_update(FusionAhrs *ahrs, float gx, float gy, float gz, float ax, float ay,
                           float az, float mx, float my, float mz, float dt);

fusion_data_t fusion_wrapper_get_data(FusionAhrs *ahrs);

#endif
