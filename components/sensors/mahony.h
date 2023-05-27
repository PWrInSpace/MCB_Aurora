//=============================================================================================
// MahonyAHRS.h
//=============================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=============================================================================================
#ifndef MahonyAHRS_h
#define MahonyAHRS_h

typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
} quaternion_t;

void Mahony_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void Mahony_Init(float sampleFrequency);
float Mahony_invSqrt(float x);
void Mahony_computeAngles(void);
void getQuat(quaternion_t *q);
float getRoll(void);
float getPitch(void);
float getYaw(void);

#endif