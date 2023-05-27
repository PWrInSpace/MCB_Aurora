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
#include "mahony.h"
#include <math.h>

#define INV_SAMPLE_RATE  (1.0f / SENSORFS)

// static float twoKp = twoKpDef;		// 2 * proportional gain (Kp)
// static float twoKi = twoKiDef;		// 2 * integral gain (Ki)
// static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f; // quaternion of sensor frame relative to auxiliary frame
// static float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f; // integral error terms scaled by Ki

void mahony_init(float kp, float ki, mahony_t *mahony) {

	mahony->two_kp = 2 * kp;	// 2 * proportional gain (Kp)
	mahony->two_ki = 2 * ki;	// 2 * integral gain (Ki)
	mahony->q.q0 = 1.0f;
	mahony->q.q1 = 0.0f;
	mahony->q.q2 = 0.0f;
	mahony->q.q3 = 0.0f;

	mahony->reset_next_update = 0;
	mahony->integral_fb.x = 0.0f;
	mahony->integral_fb.y = 0.0f;
	mahony->integral_fb.z = 0.0f;
}

//---------------------------------------------------------------------------------------------
// Fast inverse square-root quake
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

static float invSqrt(float x) {
	union {
		float f;
		int32_t i;
	} y;
	float halfx = 0.5f * x;

	y.f = x;
	y.i = 0x5f375a86 - (y.i >> 1);
	y.f = y.f * (1.5f - (halfx * y.f * y.f));
	y.f = y.f * (1.5f - (halfx * y.f * y.f));
	y.f = y.f * (1.5f - (halfx * y.f * y.f));
	return y.f;
}

void mahony_updateIMU(float gx, float gy, float gz, float ax, float ay, float az, mahony_t *mahony)
{
	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Compute feedback only if accelerometer measurement valid
	// (avoids NaN in accelerometer normalisation)
	if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = mahony->q.q1 * mahony->q.q3 - mahony->q.q0 * mahony->q.q2;
		halfvy = mahony->q.q0 * mahony->q.q1 + mahony->q.q2 * mahony->q.q3;
		halfvz = mahony->q.q0 * mahony->q.q0 - 0.5f + mahony->q.q3 * mahony->q.q3;

		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(mahony->two_ki > 0.0f) {
			// integral error scaled by Ki
			mahony->integral_fb.x += mahony->two_ki * halfex * INV_SAMPLE_RATE;
			mahony->integral_fb.y += mahony->two_ki * halfey * INV_SAMPLE_RATE;
			mahony->integral_fb.z += mahony->two_ki * halfez * INV_SAMPLE_RATE;
			gx += mahony->integral_fb.x;	// apply integral feedback
			gy += mahony->integral_fb.y;
			gz += mahony->integral_fb.z;
		} else {
			mahony->integral_fb.x = 0.0f;	// prevent integral windup
			mahony->integral_fb.y = 0.0f;
			mahony->integral_fb.z = 0.0f;
		}

		// Apply proportional feedback
		gx += mahony->two_kp * halfex;
		gy += mahony->two_kp * halfey;
		gz += mahony->two_kp * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * INV_SAMPLE_RATE);		// pre-multiply common factors
	gy *= (0.5f * INV_SAMPLE_RATE);
	gz *= (0.5f * INV_SAMPLE_RATE);
	qa = mahony->q.q0;
	qb = mahony->q.q1;
	qc = mahony->q.q2;
	mahony->q.q0 += (-qb * gx - qc * gy - mahony->q.q3 * gz);
	mahony->q.q1 += (qa * gx + qc * gz - mahony->q.q3 * gy);
	mahony->q.q2 += (qa * gy - qb * gz + mahony->q.q3 * gx);
	mahony->q.q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recipNorm = invSqrt(mahony->q.q0 * mahony->q.q0 + mahony->q.q1 * mahony->q.q1 + mahony->q.q2 * mahony->q.q2 + mahony->q.q3 * mahony->q.q3);
	mahony->q.q0 *= recipNorm;
	mahony->q.q1 *= recipNorm;
	mahony->q.q2 *= recipNorm;
	mahony->q.q3 *= recipNorm;
}

void mahony_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, mahony_t *mahony)
{
	float recipNorm;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
	float hx, hy, bx, bz;
	float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Use IMU algorithm if magnetometer measurement invalid
	// (avoids NaN in magnetometer normalisation)
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		mahony_updateIMU(gx, gy, gz, ax, ay, az, mahony);
		return;
	}

	// Compute feedback only if accelerometer measurement valid
	// (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Normalise magnetometer measurement
		recipNorm = invSqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;
#if 0
		// crazy experiement - no filter, just use magnetometer...
		q0 = 0;
		q1 = mx;
		q2 = my;
		q3 = mz;
		return;
#endif
		// Auxiliary variables to avoid repeated arithmetic
		q0q0 = mahony->q.q0 * mahony->q.q0;
		q0q1 = mahony->q.q0 * mahony->q.q1;
		q0q2 = mahony->q.q0 * mahony->q.q2;
		q0q3 = mahony->q.q0 * mahony->q.q3;
		q1q1 = mahony->q.q1 * mahony->q.q1;
		q1q2 = mahony->q.q1 * mahony->q.q2;
		q1q3 = mahony->q.q1 * mahony->q.q3;
		q2q2 = mahony->q.q2 * mahony->q.q2;
		q2q3 = mahony->q.q2 * mahony->q.q3;
		q3q3 = mahony->q.q3 * mahony->q.q3;

		// Reference direction of Earth's magnetic field
		hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
		hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
		bx = sqrtf(hx * hx + hy * hy);
		bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));

		// Estimated direction of gravity and magnetic field
		halfvx = q1q3 - q0q2;
		halfvy = q0q1 + q2q3;
		halfvz = q0q0 - 0.5f + q3q3;
		halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
		halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
		halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

		// Error is sum of cross product between estimated direction
		// and measured direction of field vectors
		halfex = (ay * halfvz - az * halfvy) + (my * halfwz - mz * halfwy);
		halfey = (az * halfvx - ax * halfvz) + (mz * halfwx - mx * halfwz);
		halfez = (ax * halfvy - ay * halfvx) + (mx * halfwy - my * halfwx);

		// Compute and apply integral feedback if enabled
		if(mahony->two_ki > 0.0f) {
			// integral error scaled by Ki
			mahony->integral_fb.x += mahony->two_ki * halfex * INV_SAMPLE_RATE;
			mahony->integral_fb.y += mahony->two_ki * halfey * INV_SAMPLE_RATE;
			mahony->integral_fb.z += mahony->two_ki * halfez * INV_SAMPLE_RATE;
			gx += mahony->integral_fb.x;	// apply integral feedback
			gy += mahony->integral_fb.y;
			gz += mahony->integral_fb.z;
		} else {
			mahony->integral_fb.x = 0.0f;	// prevent integral windup
			mahony->integral_fb.y = 0.0f;
			mahony->integral_fb.z = 0.0f;
		}

		//printf("err =  %.3f, %.3f, %.3f\n", halfex, halfey, halfez);

		// Apply proportional feedback
		if (mahony->reset_next_update) {
			gx += 2.0f * halfex;
			gy += 2.0f * halfey;
			gz += 2.0f * halfez;
			mahony->reset_next_update = 0;
		} else {
			gx += mahony->two_kp * halfex;
			gy += mahony->two_kp * halfey;
			gz += mahony->two_kp * halfez;
		}
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * INV_SAMPLE_RATE);		// pre-multiply common factors
	gy *= (0.5f * INV_SAMPLE_RATE);
	gz *= (0.5f * INV_SAMPLE_RATE);
	qa = mahony->q.q0;
	qb = mahony->q.q1;
	qc = mahony->q.q2;
	mahony->q.q0 += (-qb * gx - qc * gy - mahony->q.q3 * gz);
	mahony->q.q1 += (qa * gx + qc * gz - mahony->q.q3 * gy);
	mahony->q.q2 += (qa * gy - qb * gz + mahony->q.q3 * gx);
	mahony->q.q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recipNorm = invSqrt(mahony->q.q0 * mahony->q.q0 + mahony->q.q1 * mahony->q.q1 + mahony->q.q2 * mahony->q.q2 + mahony->q.q3 * mahony->q.q3);
	mahony->q.q0 *= recipNorm;
	mahony->q.q1 *= recipNorm;
	mahony->q.q2 *= recipNorm;
	mahony->q.q3 *= recipNorm;
}


void quaternion_to_euler_ZYX(quaternion_t *q, double output[3])
{
    // Roll (x-axis rotation)
    // double sinr_cosp = +2.0 * (q->q0 * q->q1 + q->q2 * q->q3);
    // double cosr_cosp = +1.0 - 2.0 * (q->q1 * q->q1 + q->q2 * q->q2);
    // output[0] = atan2(sinr_cosp, cosr_cosp);

    // // Pitch (y-axis rotation)
    // double sinp = +2.0 * (q->q0 * q->q2 - q->q3 * q->q1);
    // if (fabs(sinp) >= 1)
    //     output[1] = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    // else
    //     output[1] = asin(sinp);

    // // Yaw (z-axis rotation)
    // double siny_cosp = +2.0 * (q->q0 * q->q3 + q->q1 * q->q2);
    // double cosy_cosp = +1.0 - 2.0 * (q->q2 * q->q2 + q->q3 * q->q3);
    // output[2] = atan2(siny_cosp, cosy_cosp);
	// yaw
	output[0] = atan2(2.0*(q->q2*q->q3 + q->q0*q->q1), q->q0*q->q0 - q->q1*q->q1 - q->q2*q->q2 + q->q3*q->q3);
	output[0] *= 180.0 / M_PI;
	// pitch
	output[1] = asin(-2.0*(q->q1*q->q3 - q->q0*q->q2));
	output[1] *= 180.0 / M_PI;
	// roll
	output[2] = atan2(2.0*(q->q1*q->q2 + q->q0*q->q3), q->q0*q->q0 + q->q1*q->q1 - q->q2*q->q2 - q->q3*q->q3);
	output[2] *= 180.0 / M_PI;
}
