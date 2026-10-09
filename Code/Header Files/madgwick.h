/**
 ******************************************************************************
 * @file      madgwick.h
 * @author    Alexander Alt
 * @brief     Header file for madgwick.c
 *
 ******************************************************************************
 **/

#ifndef MADGWICK_H
#define MADGWICK_H

#include <math.h>
#include <stdio.h>

extern float beta; // Madgwick Parameter
extern float q0, q1, q2, q3; // Four Quaternion Elements
//extern const float pi = 3.14159265358979323846;

void QuatReset(void);
void InitializeMadgwick(float InitialBeta);
void MadgwickFilter(float gx, float gy, float gz, float ax, float ay, float az, float deltat);
float CalculateRoll(void);

#endif

