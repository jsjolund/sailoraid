/*
 * i2c.h
 *
 *  Created on: Nov 1, 2017
 *      Author: jsjolund
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f4xx.h"

typedef struct I2C_Module
{
  I2C_HandleTypeDef *instance;
  uint16_t sdaPin;
  GPIO_TypeDef* sdaPort;
  uint16_t sclPin;
  GPIO_TypeDef* sclPort;
} I2C_Module;

void I2C_ClearBusyFlagErratum(I2C_Module* i2c);

#endif /* I2C_H_ */
