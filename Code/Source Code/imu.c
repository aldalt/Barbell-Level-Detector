/**
 ******************************************************************************
 * @file      imu.c
 * @author    Alexander Alt
 * @brief     Contains functions necessary for initializing and retrieving data
 *            from the inertial measurement unit (IMU).
 *
 ******************************************************************************
 **/

#include "imu.h"
#include <math.h>

#define MPU6050_ADDR         (0x68 << 1) // I2C Address
#define REG_CONFIG           0x1A // Register for built in DLPF to remove spikes
#define REG_GYRO_CONFIG      0x1B // Register for gyroscope sensitivity control
#define REG_ACCEL_CONFIG     0x1C // Register for accelerometer sensitivity control
#define REG_PWR_MGMT_1       0x6B // Register for power control/sleep
#define REG_ACCEL_XOUT_H     0x3B // Register for memory address of most recent x-axis high byte

// I2C Handle Declaration for IMU
static I2C_HandleTypeDef *imu_i2c;

// Offsets for calibration (changed when calibration button is pressed)
static float gyroX_offset = 0.0f;
static float gyroY_offset = 0.0f;
static float gyroZ_offset = 0.0f;
static float accelX_offset = 0.0f;
static float accelY_offset = 0.0f;
static float accelZ_offset = 0.0f;

// Function to initialize the IMU
uint8_t InitializeImu(I2C_HandleTypeDef *hi2c) {

	// Declare correct I2C port
	imu_i2c = hi2c;

	// Declare temporary data variable to use its address for memory addresses
	uint8_t data;

	HAL_StatusTypeDef status;
	status = HAL_I2C_Mem_Write(imu_i2c, MPU6050_ADDR, REG_PWR_MGMT_1, 1, &data, 1, 100);

	if (status != HAL_OK) return 0;

	// Wake up the IMU
	data = 0x00;
	if (HAL_I2C_Mem_Write(imu_i2c, MPU6050_ADDR, REG_PWR_MGMT_1, 1, &data, 1, 100) != HAL_OK) return 0;

	// Set the cutoff frequency for the integrated digital low pass frequency to ~21Hz
	data = 0x04;
	HAL_I2C_Mem_Write(imu_i2c, MPU6050_ADDR, REG_CONFIG, 1, &data, 1, 100);

	// Set the gyroscope sensitivity control to ±4G Range
	data = 0x08;
	HAL_I2C_Mem_Write(imu_i2c, MPU6050_ADDR, REG_ACCEL_CONFIG, 1, &data, 1, 100);

	// Set the accelerometer sensitivity control to ±500°/s Range
	data = 0x08;
	HAL_I2C_Mem_Write(imu_i2c, MPU6050_ADDR, REG_GYRO_CONFIG, 1, &data, 1, 100);

	return 1;
}

void CalibrateImu(void) {

	// Declare a buffer for the raw data from the IMU
	uint8_t buffer[14];

	// Declare integers for the sums and number of accelerometer/gyroscope values measured during calibration
	int32_t sumGX = 0;
	int32_t sumGY = 0;
	int32_t sumGZ = 0;
	int32_t sumAX = 0;
	int32_t sumAY = 0;
	int32_t sumAZ = 0;
	const int samples = 400;

	// Collect measurements while the barbell is on the rack, and reconstruct split values from buffer
	for(int i = 0; i < samples; i++) {
	    if(HAL_I2C_Mem_Read(imu_i2c, MPU6050_ADDR, REG_ACCEL_XOUT_H, 1, buffer, 14, 50) == HAL_OK) {
	            sumAX += (int16_t)((buffer[0] << 8) | buffer[1]);
	            sumAY += (int16_t)((buffer[2] << 8) | buffer[3]);
	            sumAZ += (int16_t)((buffer[4] << 8) | buffer[5]);
	            sumGX += (int16_t)((buffer[8] << 8) | buffer[9]);
	            sumGY += (int16_t)((buffer[10] << 8) | buffer[11]);
	            sumGZ += (int16_t)((buffer[12] << 8) | buffer[13]);
	    }

	    // Delay to match sensor register output rate
	    HAL_Delay(3);
	}

	// Average gyroscope offsets, convert to degrees/second, and scale by ±500°/s scale (65.5 LSB/°/s)
	// 65.5 = ratio of maximum velocity output to physical scale limit (500)
	gyroX_offset = ((float)sumGX / samples) / 65.5f;
	gyroY_offset = ((float)sumGY / samples) / 65.5f;
	gyroZ_offset = ((float)sumGZ / samples) / 65.5f;

	// Average accelerometer offsets, scale via ±4G sensitivity profile (8192 LSB/G)
	// 8192 = ratio of accelerometer output to G's of gravity
	accelX_offset = ((float)sumAX / samples) / 8192.0f;
	accelY_offset = ((float)sumAY / samples) / 8192.0f;

	// For Z-axis, subtract 1.0G of pure Earth gravity so the filter treats this tilt position as 0 error
	accelZ_offset = (((float)sumAZ / samples) / 8192.0f) - 1.0f;
}

uint8_t GetImuData(ImuData *data) {

	// Declare a buffer for raw IMU data
	uint8_t buffer[14];

	// Collect all 6 measurements from IMU and put them in the buffer as split blocks
	if(HAL_I2C_Mem_Read(imu_i2c, MPU6050_ADDR, REG_ACCEL_XOUT_H, 1, buffer, 14, 100) == HAL_OK) {

		//reconstruct split data bytes for the 6 IMU measurements
		int16_t raw_ax = (int16_t)((buffer[0] << 8) | buffer[1]);
		int16_t raw_ay = (int16_t)((buffer[2] << 8) | buffer[3]);
		int16_t raw_az = (int16_t)((buffer[4] << 8) | buffer[5]);
		int16_t raw_gx = (int16_t)((buffer[8] << 8) | buffer[9]);
		int16_t raw_gy = (int16_t)((buffer[10] << 8) | buffer[11]);
		int16_t raw_gz = (int16_t)((buffer[12] << 8) | buffer[13]);

		// Convert raw accelerometer bits to G's and subtract the offset from calibration
        data->ax = ((float)raw_ax / 8192.0f) - accelX_offset;
        data->ay = ((float)raw_ay / 8192.0f) - accelY_offset;
        data->az = ((float)raw_az / 8192.0f) - accelZ_offset;

		// Convert raw gyroscope bits to deg/sec, subtract offset from calibration, and convert to radians for Madgwick
        data->gx = (((float)raw_gx / 65.5f) - gyroX_offset) * (3.14159265f / 180.0f);
        data->gy = (((float)raw_gy / 65.5f) - gyroY_offset) * (3.14159265f / 180.0f);
        data->gz = (((float)raw_gz / 65.5f) - gyroZ_offset) * (3.14159265f / 180.0f);

        // Return successful completion
        return 1;
	}

	// Otherwise return failure
	return 0;
}




