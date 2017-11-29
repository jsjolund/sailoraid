/*
 * imu.c
 *
 *  Created on: Sep 20, 2017
 *      Author: jsjolund
 */

#include "imu.h"

static void *ACCELERO_handle = NULL;
static void *GYRO_handle = NULL;
static void *MAGNETO_handle = NULL;
static void *HUMIDITY_handle = NULL;
static void *TEMPERATURE_handle = NULL;
static void *PRESSURE_handle = NULL;

/**
 * @brief  Handles the GYRO axes data getting/sending
 * @param  Msg the GYRO part of the stream
 * @retval None
 */
void Gyro_Sensor_Handler(SensorAxes_t *GYR_Value)
{
  uint8_t status = 0;
  uint8_t drdy = 0;
  if (BSP_GYRO_IsInitialized(GYRO_handle, &status) == COMPONENT_OK && status == 1)
  {
    BSP_GYRO_Get_DRDY_Status(GYRO_handle, &drdy);
    if (drdy != 0)
    {
      BSP_GYRO_Get_Axes(GYRO_handle, GYR_Value);
    }
  }
}

/**
 * @brief  Handles the MAGNETO axes data getting/sending
 * @param  Msg the MAGNETO part of the stream
 * @retval None
 */
void Magneto_Sensor_Handler(SensorAxes_t *MAG_Value)
{
  uint8_t status = 0;
  uint8_t drdy = 0;
  if (BSP_MAGNETO_IsInitialized(MAGNETO_handle, &status) == COMPONENT_OK && status == 1)
  {
    BSP_MAGNETO_Get_DRDY_Status(MAGNETO_handle, &drdy);
    if (drdy != 0)
    {
      BSP_MAGNETO_Get_Axes(MAGNETO_handle, MAG_Value);
    }
  }
}

/**
 * @brief  Handles the ACCELERO axes data getting/sending
 * @param  Msg the ACCELERO part of the stream
 * @retval None
 */
void Accelero_Sensor_Handler(SensorAxes_t *ACC_Value)
{
  uint8_t status = 0;
  uint8_t drdy = 0;
  if (BSP_ACCELERO_IsInitialized(ACCELERO_handle, &status) == COMPONENT_OK && status == 1)
  {
    BSP_ACCELERO_Get_DRDY_Status(ACCELERO_handle, &drdy);
    if (drdy != 0)
    {
      BSP_ACCELERO_Get_Axes(ACCELERO_handle, ACC_Value);
    }
  }
}

/**
 * @brief  Handles the PRESSURE sensor data getting/sending
 * @param  Msg the PRESSURE part of the stream
 * @retval None
 */
void Pressure_Sensor_Handler(float *PRESSURE_Value)
{
  uint8_t status = 0;
  uint8_t drdy = 0;
  if (BSP_PRESSURE_IsInitialized(PRESSURE_handle, &status) == COMPONENT_OK && status == 1)
  {
    BSP_PRESSURE_Get_DRDY_Status(PRESSURE_handle, &drdy);
    if (drdy != 0)
    {
      BSP_PRESSURE_Get_Press(PRESSURE_handle, PRESSURE_Value);
    }
  }
}

/**
 * @brief  Handles the HUMIDITY sensor data getting/sending
 * @param  Msg the HUMIDITY part of the stream
 * @retval None
 */
void Humidity_Sensor_Handler(float *HUMIDITY_Value)
{
  uint8_t status = 0;
  uint8_t drdy = 0;
  if (BSP_HUMIDITY_IsInitialized(HUMIDITY_handle, &status) == COMPONENT_OK && status == 1)
  {
    BSP_HUMIDITY_Get_DRDY_Status(HUMIDITY_handle, &drdy);
    if (drdy != 0)
    {
      BSP_HUMIDITY_Get_Hum(HUMIDITY_handle, HUMIDITY_Value);
    }
  }
}

/**
 * @brief  Handles the TEMPERATURE sensor data getting/sending
 * @param  Msg the TEMPERATURE part of the stream
 * @retval None
 */
void Temperature_Sensor_Handler(float *TEMPERATURE_Value)
{
  uint8_t status = 0;
  uint8_t drdy = 0;
  if (BSP_TEMPERATURE_IsInitialized(TEMPERATURE_handle, &status) == COMPONENT_OK && status == 1)
  {
    BSP_TEMPERATURE_Get_DRDY_Status(TEMPERATURE_handle, &drdy);
    if (drdy != 0)
    {
      BSP_TEMPERATURE_Get_Temp(TEMPERATURE_handle, TEMPERATURE_Value);
    }
  }
}

/**
 * @brief  Initialize all sensors
 * @param  None
 * @retval None
 */
void IMUinit(void)
{
  /* Try to use automatic discovery. By default use LSM6DSL on board */
  if (BSP_ACCELERO_Init(ACCELERO_SENSORS_AUTO, &ACCELERO_handle) != COMPONENT_OK)
    printf("ERROR ACC INIT\n");
  /* Try to use automatic discovery. By default use LSM6DSL on board */
  if (BSP_GYRO_Init(GYRO_SENSORS_AUTO, &GYRO_handle) != COMPONENT_OK)
     printf("ERROR GYRO INIT\n");
  /* Try to use automatic discovery. By default use LSM303AGR on board */
  if (BSP_MAGNETO_Init(MAGNETO_SENSORS_AUTO, &MAGNETO_handle) != COMPONENT_OK)
    printf("ERROR MAG INIT\n");
  /* Try to use automatic discovery. By default use HTS221 on board */
//  BSP_HUMIDITY_Init(HUMIDITY_SENSORS_AUTO, &HUMIDITY_handle);
//  /* Try to use automatic discovery. By default use HTS221 on board */
//  BSP_TEMPERATURE_Init(TEMPERATURE_SENSORS_AUTO, &TEMPERATURE_handle);
//  /* Try to use automatic discovery. By default use LPS22HB on board */
//  BSP_PRESSURE_Init(PRESSURE_SENSORS_AUTO, &PRESSURE_handle);

//  BSP_PRESSURE_Sensor_Enable(PRESSURE_handle);
//  BSP_HUMIDITY_Sensor_Enable(HUMIDITY_handle);
//  BSP_TEMPERATURE_Sensor_Enable(TEMPERATURE_handle);
  BSP_ACCELERO_Sensor_Enable(ACCELERO_handle);
  BSP_GYRO_Sensor_Enable(GYRO_handle);
  BSP_MAGNETO_Sensor_Enable(MAGNETO_handle);
}

