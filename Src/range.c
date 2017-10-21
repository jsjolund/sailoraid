/**
 ******************************************************************************
 *
 * COPYRIGHT(c) 2015 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "range.h"

/** leaky factor for filtered range
 *
 * r(n) = averaged_r(n-1)*leaky +r(n)(1-leaky)
 *
 * */
static int LeakyFactorFix8 = (int) (0.6 * 256);
static VL53L0X_Error Status = VL53L0X_ERROR_RANGE_ERROR;
static VL53L0X_DeviceModes DeviceMode = 0;

/**
 * Reset all sensor then do presence detection
 *
 * All present devices are data initiated and assigned to their final I2C address
 * @return
 */
void Range_Sensor_Init(VL53L0X_Dev_t *VL53L0XDev)
{
  HAL_GPIO_WritePin(RANGE_SHDN_GPIO_Port, RANGE_SHDN_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(RANGE_SHDN_GPIO_Port, RANGE_SHDN_Pin, GPIO_PIN_SET);

  uint16_t Id;
  int status;
  int FinalAddress;

  /* detect all sensors (even on-board)*/
  VL53L0X_Dev_t *pDev;
  pDev = VL53L0XDev;
  pDev->I2cDevAddr = 0x52;
  pDev->Present = 0;

  HAL_Delay(2);
  FinalAddress = 0x52 + (0 + 1) * 2;

  do
  {
    /* Set I2C standard mode (400 KHz) before doing the first register access */
    status = VL53L0X_WrByte(pDev, 0x88, 0x00);

    /* Try to read one register using default 0x52 address */
    status = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
    if (status)
    {
      printf("Read id fail\n");
      break;
    }
    if (Id == 0xEEAA)
    {
      /* Sensor is found => Change its I2C address to final one */
      status = VL53L0X_SetDeviceAddress(pDev, FinalAddress);
      if (status != 0)
      {
        printf("VL53L0X_SetDeviceAddress fail\n");
        break;
      }
      pDev->I2cDevAddr = FinalAddress;
      /* Check all is OK with the new I2C address and initialize the sensor */
      status = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
      if (status != 0)
      {
        printf("VL53L0X_RdWord fail\n");
        break;
      }

      status = VL53L0X_DataInit(pDev);
      if (status == 0)
      {
        pDev->Present = 1;
      }
      else
      {
        printf("VL53L0X_DataInit fail\n");
        break;
      }
      printf("VL53L0X %d Present and initiated to final 0x%x\n", pDev->Id, pDev->I2cDevAddr);
      pDev->Present = 1;
    }
    else
    {
      printf("unknown ID %x\n", Id);
      status = 1;
    }
  } while (0);

}

/**
 *  Setup all detected sensors for single shot mode and setup ranging configuration
 */
void Range_Sensor_Setup_Single_Shot(VL53L0X_Dev_t *VL53L0XDev, RangingConfig_e rangingConfig)
{
  int status;
  uint8_t VhvSettings;
  uint8_t PhaseCal;
  uint32_t refSpadCount;
  uint8_t isApertureSpads;
  FixPoint1616_t signalLimit = (FixPoint1616_t) (0.25 * 65536);
  FixPoint1616_t sigmaLimit = (FixPoint1616_t) (18 * 65536);
  uint32_t timingBudget = 33000;
  uint8_t preRangeVcselPeriod = 14;
  uint8_t finalRangeVcselPeriod = 10;

  if (VL53L0XDev->Present)
  {
    status = VL53L0X_StaticInit(VL53L0XDev);
    if (status)
    {
      printf("VL53L0X_StaticInit failed\n");
    }

    status = VL53L0X_PerformRefCalibration(VL53L0XDev, &VhvSettings, &PhaseCal);
    if (status)
    {
      printf("VL53L0X_PerformRefCalibration failed\n");
    }

    status = VL53L0X_PerformRefSpadManagement(VL53L0XDev, &refSpadCount, &isApertureSpads);
    if (status)
    {
      printf("VL53L0X_PerformRefSpadManagement failed\n");
    }

    status = VL53L0X_SetDeviceMode(VL53L0XDev,
    VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
    if (status)
    {
      printf("VL53L0X_SetDeviceMode failed\n");
    }

    status = VL53L0X_SetLimitCheckEnable(VL53L0XDev,
    VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1); // Enable Sigma limit
    if (status)
    {
      printf("VL53L0X_SetLimitCheckEnable failed\n");
    }

    status = VL53L0X_SetLimitCheckEnable(VL53L0XDev,
    VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1); // Enable Signa limit
    if (status)
    {
      printf("VL53L0X_SetLimitCheckEnable failed\n");
    }
    /* Ranging configuration */
    switch (rangingConfig)
    {
    case LONG_RANGE:
      signalLimit = (FixPoint1616_t) (0.1 * 65536);
      sigmaLimit = (FixPoint1616_t) (60 * 65536);
      timingBudget = 33000;
      preRangeVcselPeriod = 18;
      finalRangeVcselPeriod = 14;
      break;
    case HIGH_ACCURACY:
      signalLimit = (FixPoint1616_t) (0.25 * 65536);
      sigmaLimit = (FixPoint1616_t) (18 * 65536);
      timingBudget = 200000;
      preRangeVcselPeriod = 14;
      finalRangeVcselPeriod = 10;
      break;
    case HIGH_SPEED:
      signalLimit = (FixPoint1616_t) (0.25 * 65536);
      sigmaLimit = (FixPoint1616_t) (32 * 65536);
      timingBudget = 20000;
      preRangeVcselPeriod = 14;
      finalRangeVcselPeriod = 10;
      break;
    default:
      printf("Not Supported");
    }

    status = VL53L0X_SetLimitCheckValue(VL53L0XDev,
    VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
    if (status)
    {
      printf("VL53L0X_SetLimitCheckValue failed\n");
    }

    status = VL53L0X_SetLimitCheckValue(VL53L0XDev,
    VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
    if (status)
    {
      printf("VL53L0X_SetLimitCheckValue failed\n");
    }

    status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(VL53L0XDev, timingBudget);
    if (status)
    {
      printf("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed\n");
    }

    status = VL53L0X_SetVcselPulsePeriod(VL53L0XDev,
    VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
    if (status)
    {
      printf("VL53L0X_SetVcselPulsePeriod failed\n");
    }

    status = VL53L0X_SetVcselPulsePeriod(VL53L0XDev,
    VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
    if (status)
    {
      printf("VL53L0X_SetVcselPulsePeriod failed\n");
    }

    status = VL53L0X_PerformRefCalibration(VL53L0XDev, &VhvSettings, &PhaseCal);
    if (status)
    {
      printf("VL53L0X_PerformRefCalibration failed\n");
    }

    VL53L0XDev->LeakyFirst = 1;
  }
}

/* Store new ranging data into the device structure, apply leaky integrator if needed */
void Range_Sensor_Set_New_Range(VL53L0X_Dev_t *VL53L0XDev, VL53L0X_RangingMeasurementData_t *rangeData)
{
  if (rangeData->RangeStatus == 0)
  {
    if (VL53L0XDev->LeakyFirst)
    {
      VL53L0XDev->LeakyFirst = 0;
      VL53L0XDev->LeakyRange = rangeData->RangeMilliMeter;
    }
    else
    {
      VL53L0XDev->LeakyRange = (VL53L0XDev->LeakyRange * LeakyFactorFix8 + (256 - LeakyFactorFix8) * rangeData->RangeMilliMeter) >> 8;
    }
  }
  else
  {
    VL53L0XDev->LeakyFirst = 1;
  }
}

VL53L0X_Error Range_Sensor_Start_New_Measurement(VL53L0X_Dev_t *VL53L0XDev, VL53L0X_RangingMeasurementData_t *rangeData)
{
  Status = VL53L0X_ERROR_NONE;
  /* This function will do a complete single ranging
   * Here we fix the mode! */
  Status = VL53L0X_SetDeviceMode(VL53L0XDev, VL53L0X_DEVICEMODE_SINGLE_RANGING);

  /* Get Current DeviceMode */
  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_GetDeviceMode(VL53L0XDev, &DeviceMode);

  /* Start immediately to run a single ranging measurement in case of
   * single ranging or single histogram */
  if (Status == VL53L0X_ERROR_NONE && DeviceMode == VL53L0X_DEVICEMODE_SINGLE_RANGING)
    Status = VL53L0X_StartMeasurement(VL53L0XDev);

  return Status;
}

VL53L0X_Error Range_Sensor_Get_Measurement(VL53L0X_Dev_t *VL53L0XDev, VL53L0X_RangingMeasurementData_t *rangeData)
{
  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_measurement_poll_for_completion(VL53L0XDev);

  /* Change PAL State in case of single ranging or single histogram */
  if (Status == VL53L0X_ERROR_NONE && DeviceMode == VL53L0X_DEVICEMODE_SINGLE_RANGING)
    PALDevDataSet(VL53L0XDev, PalState, VL53L0X_STATE_IDLE);

  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_GetRangingMeasurementData(VL53L0XDev, rangeData);

  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_ClearInterruptMask(VL53L0XDev, 0);

  if (Status == VL53L0X_ERROR_NONE)
    Range_Sensor_Set_New_Range(VL53L0XDev, rangeData);

  return Status;
}

