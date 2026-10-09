#ifndef KALMAN_H
#define KALMAN_H

#include <stdbool.h>

typedef struct {
    float alt;
    float vel;
    float P[2][2];

    float q_accel;
    float r_baro;
} kalman_t;

bool kalman_init(kalman_t *kf, float q_accel, float r_baro);

bool kalman_predict(kalman_t *kf, float accel_z, float dt);

bool kalman_update(kalman_t *kf, float measured_altitude);

#endif
