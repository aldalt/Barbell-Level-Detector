/**
 ******************************************************************************
 * @file      madgwick.c
 * @author    Alexander Alt
 * @brief     Contains functions for the Madgwick filtering algorithm used to
 *            filter noise from the IMU data.
 *
 ******************************************************************************
 **/

#include "madgwick.h"

// Define the Madgwick Parameter (Beta) and the Quaternion (and pi)
float beta = 0.05f;
float q0 = 1.0f;
float q1 = 0.0f;
float q2 = 0.0f;
float q3 = 0.0f;
float pi = 3.14159265358979323846;

// Function to reset the quaternion to its default (1,0,0,0)
void QuatReset(void) {
	q0 = 1.0f;
	q1 = 0.0f;
	q2 = 0.0f;
	q3 = 0.0f;
}

// Function to completely reset the madgwick function
void InitializeMadgwick(float InitialBeta) {
	beta = InitialBeta;
	QuatReset();
}

// Function to perform the Madgwick Filter Calculations
void MadgwickFilter(float gx, float gy, float gz, float ax, float ay, float az, float deltat) {

	// Calculate quaternion derivatives
	float qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	float qDot2 = 0.5f * ( q0 * gx + q2 * gz - q3 * gy);
	float qDot3 = 0.5f * ( q0 * gy - q1 * gz + q3 * gx);
	float qDot4 = 0.5f * ( q0 * gz + q1 * gy - q2 * gx);

	// Declare a variable for magnitude for normalizations
	float OneOverMagnitude = 0;

	// Now only proceed if there are active changes in the accelerometer readings
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalize the accelerometer data (to get a unit vector)
		OneOverMagnitude = 1 / sqrt(ax * ax + ay * ay + az * az);
		ax *= OneOverMagnitude;
		ay *= OneOverMagnitude;
		az *= OneOverMagnitude;

		// Calculate some repeatedly used math terms to maintain simplicity in equations
		float _2q0 = 2.0f * q0;
		float _2q1 = 2.0f * q1;
		float _2q2 = 2.0f * q2;
		float _2q3 = 2.0f * q3;
		float _4q0 = 4.0f * q0;
		float _4q1 = 4.0f * q1;
		float _4q2 = 4.0f * q2;
		float _8q1 = 8.0f * q1;
		float _8q2 = 8.0f * q2;
		float q0q0 = q0 * q0;
		float q1q1 = q1 * q1;
		float q2q2 = q2 * q2;
		float q3q3 = q3 * q3;

		// Jacobian Matrix Multiplication to find difference between calculated orientation and gravity vector
		float s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		float s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		float s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		float s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;

		// Normalize the error gradient vector (to get a unit vector)
		OneOverMagnitude = 1 / sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
		s0 *= OneOverMagnitude;
		s1 *= OneOverMagnitude;
		s2 *= OneOverMagnitude;
		s3 *= OneOverMagnitude;

		// Sensor Fusion: Scale directional states by beta and merge into gyroscope change
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Now adjust global orientation based on calculated change outputs
	q0 += qDot1 * deltat;
	q1 += qDot2 * deltat;
	q2 += qDot3 * deltat;
	q3 += qDot4 * deltat;

	// Finally, normalize the resulting final quaternion (to get a unit vector)
	OneOverMagnitude = 1 / sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= OneOverMagnitude;
	q1 *= OneOverMagnitude;
	q2 *= OneOverMagnitude;
	q3 *= OneOverMagnitude;
}

// Function to calculate roll using trigonometry
float CalculateRoll(void) {
	return atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2)) * (180.0f / pi);
}












