/*
 * range.h
 *
 *  Created on: Oct 13, 2017
 *      Author: jsjolund
 */

#ifndef RANGE_H_
#define RANGE_H_

#include "stm32f4xx_hal.h"

#include "vl53l0x_def.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

typedef enum
{
  LONG_RANGE = 0, /*!< Long range mode */
  HIGH_SPEED = 1, /*!< High speed mode */
  HIGH_ACCURACY = 2, /*!< High accuracy mode */
} RangingConfig_e;

typedef enum
{
  RANGE_VALUE = 0, /*!< Range displayed in cm */
  BAR_GRAPH = 1, /*!< Range displayed as a bar graph : one bar per sensor */
} DemoMode_e;

void Range_Sensor_Init(VL53L0X_Dev_t *VL53L0XDev);
void Range_Sensor_Setup_Single_Shot(VL53L0X_Dev_t *VL53L0XDev, RangingConfig_e rangingConfig);
void Range_Sensor_Set_New_Range(VL53L0X_Dev_t *pDev, VL53L0X_RangingMeasurementData_t *pRange);


#endif /* RANGE_H_ */
