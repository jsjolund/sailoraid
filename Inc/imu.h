/*
 * imu.h
 *
 *  Created on: Sep 20, 2017
 *      Author: user
 */

#ifndef IMU_H_
#define IMU_H_

#include "sensor.h"
#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_gyro.h"
#include "x_nucleo_iks01a2_accelero.h"
#include "x_nucleo_iks01a2_magneto.h"
#include "x_nucleo_iks01a2_humidity.h"
#include "x_nucleo_iks01a2_pressure.h"
#include "x_nucleo_iks01a2_temperature.h"

#define MAG_X_BIAS 8.5f
#define MAG_Y_BIAS -66.0f
#define MAG_Z_BIAS 13.5f
#define MAG_X_SCL 0.999592
#define MAG_Y_SCL 1.0057472
#define MAG_Z_SCL 0.9947219

void enableDisableSensors(void);
void initializeAllSensors(void);
void Temperature_Sensor_Handler(float *TEMPERATURE_Value);
void Humidity_Sensor_Handler(float *HUMIDITY_Value);
void Pressure_Sensor_Handler(float *PRESSURE_Value);
void Accelero_Sensor_Handler(SensorAxes_t *ACC_Value);
void Magneto_Sensor_Handler(SensorAxes_t *MAG_Value);
void Gyro_Sensor_Handler(SensorAxes_t *GYR_Value);

#endif /* IMU_H_ */
