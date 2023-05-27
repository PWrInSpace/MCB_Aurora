//==============================================================================================
// MahonyAHRS.c
//==============================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// From the x-io website "Open-source resources available on this website are
// provided under the GNU General Public Licence unless an alternative licence
// is provided in source."
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
// Algorithm paper:
// http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=4608934&url=http%3A%2F%2Fieeexplore.ieee.org%2Fstamp%2Fstamp.jsp%3Ftp%3D%26arnumber%3D4608934
//
//=========
#include <stdint.h>

#ifndef MAHONY_FILTER_H
#define MAHONY_FILTER_H

#define SENSORFS 100

typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
} quaternion_t;

typedef struct {
    float x;
    float y;
    float z;
} vector_3d_f_t;

typedef struct {
    float two_kp;
    float two_ki;
    uint8_t reset_next_update;
    quaternion_t q;
    vector_3d_f_t integral_fb;
} mahony_t;

void mahony_init(float kp, float ki, mahony_t *mahony);
void mahony_updateIMU(float gx, float gy, float gz, float ax, float ay, float az, mahony_t *mahony);
void quaternion_to_euler_ZYX(quaternion_t *q, double output[3]);
void mahony_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, mahony_t *mahony);
#endif