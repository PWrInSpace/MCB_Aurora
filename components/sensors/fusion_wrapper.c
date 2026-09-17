#include "fusion_wrapper.h"

#include "Fusion/Fusion/FusionAhrs.h"
#include "Fusion/Fusion/FusionBias.h"
#include "Fusion/Fusion/FusionConvention.h"

bool fusion_wrapper_init(FusionAhrs *ahrs) {
    // Instantiate AHRS algorithm
    FusionAhrsInitialise(ahrs);

    const FusionAhrsSettings settings = {
        .sampleRate = 50.0f,
        .convention = FusionConventionNed,
        .gain = 0.5f,
        .gyroscopeRange = 2000.0f,
        .accelerationRejection = 6.0f,
        .magneticRejection = 8.0f,
        .rejectionTimeout = 8.0f,
    };

    FusionAhrsSetSettings(ahrs, &settings);

    // Instantiate bias algorithm
    FusionBias bias;
    FusionBiasInitialise(&bias);

    FusionBiasSettings biasSettings = fusionBiasDefaultSettings;
    biasSettings.sampleRate = 50.0f;

    FusionBiasSetSettings(&bias, &biasSettings);

    return true;
}

bool fusion_wrapper_update(FusionAhrs *ahrs, float gx, float gy, float gz, float ax, float ay,
                           float az, float mx, float my, float mz, float dt) {
    FusionAhrsSetSamplePeriod(ahrs, dt);

    const FusionVector gyro = {
        .axis = {
            .x = gx,
            .y = gy,
            .z = gz,
        }
    };

    const FusionVector accel = {
        .axis = {
            .x = ax,
            .y = ay,
            .z = az,
        }
    };

    const FusionVector mag = {
        .axis = {
            .x = mx,
            .y = my,
            .z = mz,
        }
    };

    FusionAhrsUpdate(ahrs, gyro, accel, mag);
    return true;
}

fusion_data_t fusion_wrapper_get_data(FusionAhrs *ahrs) {
    fusion_data_t data;
    data.quaternion = FusionAhrsGetQuaternion(ahrs);
    data.acceleration = FusionAhrsGetLinearAcceleration(ahrs);
    data.acceleration = FusionAhrsGetEarthAcceleration(ahrs);
    return data;
}

