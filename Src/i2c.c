/*
 * i2c.c
 *
 *  Created on: Nov 1, 2017
 *      Author: jsjolund
 */

#include "i2c.h"
#include "imu.h"
#include "range.h"

extern I2C_HandleTypeDef hi2c1;

// https://community.st.com/thread/35884-cant-reset-i2c-in-stm32f407-to-release-i2c-lines
void I2C_ClearBusyFlagErratum()
{
  I2C_Module _i2c;
  I2C_Module *i2c = & _i2c;
  i2c->instance = &hi2c1;
  i2c->sclPin = I2C_SCL_Pin;
  i2c->sclPort = I2C_SCL_GPIO_Port;
  i2c->sdaPin = I2C_SDA_Pin;
  i2c->sdaPort = I2C_SDA_GPIO_Port;

  GPIO_InitTypeDef GPIO_InitStruct;

  // 1. Clear PE bit.
  i2c->instance->Instance->CR1 &= ~(0x0001);

  //  2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = i2c->sclPin;
  HAL_GPIO_Init(i2c->sclPort, &GPIO_InitStruct);
  HAL_GPIO_WritePin(i2c->sclPort, i2c->sclPin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = i2c->sdaPin;
  HAL_GPIO_Init(i2c->sdaPort, &GPIO_InitStruct);
  HAL_GPIO_WritePin(i2c->sdaPort, i2c->sdaPin, GPIO_PIN_SET);

  // 3. Check SCL and SDA High level in GPIOx_IDR.
  while (GPIO_PIN_SET != HAL_GPIO_ReadPin(i2c->sclPort, i2c->sclPin))
    asm("nop");

  while (GPIO_PIN_SET != HAL_GPIO_ReadPin(i2c->sdaPort, i2c->sdaPin))
    asm("nop");

  // 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
  HAL_GPIO_WritePin(i2c->sdaPort, i2c->sdaPin, GPIO_PIN_RESET);

  //  5. Check SDA Low level in GPIOx_IDR.
  while (GPIO_PIN_RESET != HAL_GPIO_ReadPin(i2c->sdaPort, i2c->sdaPin))
    asm("nop");

  // 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
  HAL_GPIO_WritePin(i2c->sclPort, i2c->sclPin, GPIO_PIN_RESET);

  //  7. Check SCL Low level in GPIOx_IDR.
  while (GPIO_PIN_RESET != HAL_GPIO_ReadPin(i2c->sclPort, i2c->sclPin))
    asm("nop");

  // 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
  HAL_GPIO_WritePin(i2c->sclPort, i2c->sclPin, GPIO_PIN_SET);

  // 9. Check SCL High level in GPIOx_IDR.
  while (GPIO_PIN_SET != HAL_GPIO_ReadPin(i2c->sclPort, i2c->sclPin))
    asm("nop");

  // 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
  HAL_GPIO_WritePin(i2c->sdaPort, i2c->sdaPin, GPIO_PIN_SET);

  // 11. Check SDA High level in GPIOx_IDR.
  while (GPIO_PIN_SET != HAL_GPIO_ReadPin(i2c->sdaPort, i2c->sdaPin))
    asm("nop");

  // 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;

  GPIO_InitStruct.Pin = i2c->sclPin;
  HAL_GPIO_Init(i2c->sclPort, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = i2c->sdaPin;
  HAL_GPIO_Init(i2c->sdaPort, &GPIO_InitStruct);

  // 13. Set SWRST bit in I2Cx_CR1 register.
  i2c->instance->Instance->CR1 |= 0x8000;

  asm("nop");

  // 14. Clear SWRST bit in I2Cx_CR1 register.
  i2c->instance->Instance->CR1 &= ~0x8000;

  asm("nop");

  // 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register
  i2c->instance->Instance->CR1 |= 0x0001;

  // Call initialization function.
  i2c->instance->Init.ClockSpeed = MAIN_I2C_SPEED;
  i2c->instance->Init.DutyCycle = I2C_DUTYCYCLE_2;
  i2c->instance->Init.OwnAddress1 = 0;
  i2c->instance->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  i2c->instance->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  i2c->instance->Init.OwnAddress2 = 0;
  i2c->instance->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c->instance->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(i2c->instance);
  IMUinit();
  VL53L0X_Dev_t rangeDev = { .Id = 0, .I2cHandle = &hi2c1, .I2cDevAddr = 0x52 };
  RangingConfig_e rangingConfig = LONG_RANGE;
  Range_Sensor_Setup_Single_Shot(&rangeDev, rangingConfig);
  Range_Sensor_Init(&rangeDev);
}
