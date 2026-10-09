/**
 ******************************************************************************
 * @file      imu.h
 * @author    Alexander Alt
 * @brief     Header file for imu.c
 *
 ******************************************************************************
 **/

#ifndef IMU_H
#define IMU_H

#include "main.h"

typedef struct {
    float ax, ay, az; // In G's (or m/s^2)
    float gx, gy, gz; // In Radians/Second
} ImuData;

uint8_t InitializeImu(I2C_HandleTypeDef *hi2c);
void CalibrateImu(void);
uint8_t GetImuData(ImuData *data);

#endif // IMU_H
