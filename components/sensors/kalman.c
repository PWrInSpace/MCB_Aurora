#include "kalman.h"

bool kalman_init(kalman_t *kf, float q_accel, float r_baro) {
    kf->alt = 0.0f;
    kf->vel = 0.0f;

    kf->P[0][0] = 10.0f;
    kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;
    kf->P[1][1] = 10.0f;

    kf->q_accel = q_accel;
    kf->r_baro = r_baro;

    return true;
}

bool kalman_predict(kalman_t *kf, float accel_z, float dt) {
    kf->alt += kf->vel * dt + 0.5f * accel_z * dt * dt;
    kf->vel += accel_z * dt;

    float dt2 = dt * dt;
    float dt3 = dt2 * dt;
    float dt4 = dt2 * dt2;

    kf->P[0][0] += dt * (kf->P[1][0] + kf->P[0][1]) + dt2 * kf->P[1][1] + 0.25f * dt4 * kf->q_accel;
    kf->P[0][1] += dt * kf->P[1][1] + 0.5f * dt3 * kf->q_accel;
    kf->P[1][0] += dt * kf->P[1][1] + 0.5f * dt3 * kf->q_accel;
    kf->P[1][1] += dt2 * kf->q_accel;

    return true;
}

bool kalman_update(kalman_t *kf, float measured_altitude) {
    float y = measured_altitude - kf->alt;

    float S = kf->P[0][0] + kf->r_baro;
    float S_inv = 1.0f / S; 
    
    float K0 = kf->P[0][0] * S_inv;
    float K1 = kf->P[1][0] * S_inv;

    kf->alt += K0 * y;
    kf->vel += K1 * y;

    float p00_temp = kf->P[0][0];
    float p01_temp = kf->P[0][1];

    kf->P[0][0] -= K0 * p00_temp;
    kf->P[0][1] -= K0 * p01_temp;
    kf->P[1][0] -= K1 * p00_temp;
    kf->P[1][1] -= K1 * p01_temp;

    return true;
}