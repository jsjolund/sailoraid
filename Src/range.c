#include "main.h"
#include "stm32f4xx_hal.h"

#include "bluenrg_interface.h"
#include "stm32f4xx_nucleo.h"
#include "imu.h"
#include "gps.h"
#include "sensor.h"
#include "serial.h"
#include "MadgwickAHRS.h"
#include "state.h"

#include "vl53l0x_def.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

enum XNUCLEO53L0A1_dev_e
{
  XNUCLEO53L0A1_DEV_LEFT = 0,    //!< left satellite device P21 header : 'l'
  XNUCLEO53L0A1_DEV_CENTER = 1, //!< center (built-in) vl053 device : 'c"
  XNUCLEO53L0A1_DEV_RIGHT = 2     //!< Right satellite device P22 header : 'r'
};

/**
 * @defgroup Configuration Static configuration
 * @{
 */
#define HAVE_ALARM_DEMO 0

/** Time the initial 53L0 message is shown at power up */
#define WelcomeTime 660

/** Time the initial 53L0 message is shown at power up */
#define ModeChangeDispTime  500

/**
 * Time considered as  a "long push" on push button
 */
#define PressBPSwicthTime   1000

/** @}  *//* config group */

#ifndef MIN
#   define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC

#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))
#endif

/**
 * @defgroup ErrCode Errors code shown on display
 * @{
 */
#define ERR_DETECT             -1
#define ERR_DEMO_RANGE_ONE     1
#define ERR_DEMO_RANGE_MULTI   2

const char TxtRangeValue[] = "rng";
const char TxtBarGraph[] = "bar";

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

/**
 * Global ranging struct
 */
VL53L0X_RangingMeasurementData_t RangingMeasurementData;

/** leaky factor for filtered range
 *
 * r(n) = averaged_r(n-1)*leaky +r(n)(1-leaky)
 *
 * */
static int LeakyFactorFix8 = (int) (0.6 * 256);
/** How many device detect set by @a DetectSensors()*/
static int nDevPresent = 0;
/** bit is index in VL53L0XDevs that is not necessary the dev id of the BSP */
static int nDevMask;

extern I2C_HandleTypeDef hi2c1;

VL53L0X_Dev_t VL53L0XDevs[] =
{
{ .Id = 0, .DevLetter = 'l', .I2cHandle = &hi2c1, .I2cDevAddr = 0x52 },
{ .Id = 0, .DevLetter = 'c', .I2cHandle = &hi2c1, .I2cDevAddr = 0x00 },
{ .Id = 0, .DevLetter = 'r', .I2cHandle = &hi2c1, .I2cDevAddr = 0x00 } };

/** range low (and high) in @a RangeToLetter()
 *
 * used for displaying  multiple sensor as bar graph
 */
static int RangeLow = 100;

/** range medium in @a RangeToLetter()
 *
 * used for displaying  multiple sensor as bar graph
 */
static int RangeMedium = 300;

/* USER CODE END PV */

//void ResetAndDetectSensor(int SetDisplay);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

#define debug_printf    printf

int BSP_GetPushButton(void)
{
  GPIO_PinState state;
  state = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
  return state;
}

/**
 * When button is already pressed it waits for user to release it.
 * if button remains pressed for a given time it returns true.
 * This is used to detect mode switch by long press on blue Push Button
 *
 * As soon as time is elapsed -rb- is displayed to let user know the mode
 * switch is taken into account
 *
 * @return True if button remains pressed more than specified time
 */
int PusbButton_WaitUnPress(void)
{
  uint32_t TimeStarted;
  TimeStarted = HAL_GetTick();
  while (!BSP_GetPushButton())
  {
    ; /* debounce */
    if (HAL_GetTick() - TimeStarted > PressBPSwicthTime)
    {
//            XNUCLEO53L0A1_SetDisplayString (" rb ");
    }
  }
  return HAL_GetTick() - TimeStarted > PressBPSwicthTime;

}

/**
 * Reset all sensor then do presence detection
 *
 * All present devices are data initiated and assigned to their final I2C address
 * @return
 */
int DetectSensors(int SetDisplay)
{
  int i;
  uint16_t Id;
  int status;
  int FinalAddress;

//  char PresentMsg[5] = "    ";
  /* Reset all */
  nDevPresent = 0;
//  for (i = 0; i < 3; i++)
//    status = XNUCLEO53L0A1_ResetId(i, 0);

  /* detect all sensors (even on-board)*/
  for (i = 0; i < 3; i++)
  {
    VL53L0X_Dev_t *pDev;
    pDev = &VL53L0XDevs[i];
    pDev->I2cDevAddr = 0x52;
    pDev->Present = 0;
//    status = XNUCLEO53L0A1_ResetId(pDev->Id, 1);
    HAL_Delay(2);
    FinalAddress = 0x52 + (i + 1) * 2;

    do
    {
      /* Set I2C standard mode (400 KHz) before doing the first register access */
//          if (status == VL53L0X_ERROR_NONE)
//            char WelcomeMsg[] = "Hi I am Ranging VL53L0X mcu " MCU_NAME "\n";
      status = VL53L0X_WrByte(pDev, 0x88, 0x00);

      /* Try to read one register using default 0x52 address */
      status = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
      if (status)
      {
        debug_printf("#%d Read id fail\n", i);
        break;
      }
      if (Id == 0xEEAA)
      {
        /* Sensor is found => Change its I2C address to final one */
        status = VL53L0X_SetDeviceAddress(pDev, FinalAddress);
        if (status != 0)
        {
          debug_printf("%d VL53L0X_SetDeviceAddress fail\n", i);
          break;
        }
        pDev->I2cDevAddr = FinalAddress;
        /* Check all is OK with the new I2C address and initialize the sensor */
        status = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
        if (status != 0)
        {
          debug_printf("%d VL53L0X_RdWord fail\n", i);
          break;
        }

        status = VL53L0X_DataInit(pDev);
        if (status == 0)
        {
          pDev->Present = 1;
        }
        else
        {
          debug_printf("VL53L0X_DataInit %d fail\n", i);
          break;
        }
        trace_printf("VL53L0X %d Present and initiated to final 0x%x\n",
            pDev->Id, pDev->I2cDevAddr);
        nDevPresent++;
        nDevMask |= 1 << i;
        pDev->Present = 1;
      }
      else
      {
        debug_printf("#%d unknown ID %x\n", i, Id);
        status = 1;
      }
    } while (0);
    /* if fail r can't use for any reason then put the  device back to reset */
    if (status)
    {
//      XNUCLEO53L0A1_ResetId(i, 0);
    }
  }

  return nDevPresent;
}

/**
 *  Setup all detected sensors for single shot mode and setup ranging configuration
 */
void SetupSingleShot(RangingConfig_e rangingConfig)
{
  int i;
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

  for (i = 0; i < 3; i++)
  {
    if (VL53L0XDevs[i].Present)
    {
      status = VL53L0X_StaticInit(&VL53L0XDevs[i]);
      if (status)
      {
        debug_printf("VL53L0X_StaticInit %d failed\n", i);
      }

      status = VL53L0X_PerformRefCalibration(&VL53L0XDevs[i], &VhvSettings,
          &PhaseCal);
      if (status)
      {
        debug_printf("VL53L0X_PerformRefCalibration failed\n");
      }

      status = VL53L0X_PerformRefSpadManagement(&VL53L0XDevs[i], &refSpadCount,
          &isApertureSpads);
      if (status)
      {
        debug_printf("VL53L0X_PerformRefSpadManagement failed\n");
      }

      status = VL53L0X_SetDeviceMode(&VL53L0XDevs[i],
      VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
      if (status)
      {
        debug_printf("VL53L0X_SetDeviceMode failed\n");
      }

      status = VL53L0X_SetLimitCheckEnable(&VL53L0XDevs[i],
      VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1); // Enable Sigma limit
      if (status)
      {
        debug_printf("VL53L0X_SetLimitCheckEnable failed\n");
      }

      status = VL53L0X_SetLimitCheckEnable(&VL53L0XDevs[i],
      VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1); // Enable Signa limit
      if (status)
      {
        debug_printf("VL53L0X_SetLimitCheckEnable failed\n");
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
        debug_printf("Not Supported");
      }

      status = VL53L0X_SetLimitCheckValue(&VL53L0XDevs[i],
      VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
      if (status)
      {
        debug_printf("VL53L0X_SetLimitCheckValue failed\n");
      }

      status = VL53L0X_SetLimitCheckValue(&VL53L0XDevs[i],
      VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
      if (status)
      {
        debug_printf("VL53L0X_SetLimitCheckValue failed\n");
      }

      status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&VL53L0XDevs[i],
          timingBudget);
      if (status)
      {
        debug_printf("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed\n");
      }

      status = VL53L0X_SetVcselPulsePeriod(&VL53L0XDevs[i],
      VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
      if (status)
      {
        debug_printf("VL53L0X_SetVcselPulsePeriod failed\n");
      }

      status = VL53L0X_SetVcselPulsePeriod(&VL53L0XDevs[i],
      VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
      if (status)
      {
        debug_printf("VL53L0X_SetVcselPulsePeriod failed\n");
      }

      status = VL53L0X_PerformRefCalibration(&VL53L0XDevs[i], &VhvSettings,
          &PhaseCal);
      if (status)
      {
        debug_printf("VL53L0X_PerformRefCalibration failed\n");
      }

      VL53L0XDevs[i].LeakyFirst = 1;
    }
  }
}

char RangeToLetter(VL53L0X_Dev_t *pDev,
    VL53L0X_RangingMeasurementData_t *pRange)
{
  char c;
  if (pRange->RangeStatus == 0)
  {
    if (pDev->LeakyRange < RangeLow)
    {
      c = '_';
    }
    else if (pDev->LeakyRange < RangeMedium)
    {
      c = '=';
    }
    else
    {
      c = '~';
    }

  }
  else
  {
    c = '-';
  }
  return c;
}

/* Store new ranging data into the device structure, apply leaky integrator if needed */
void Sensor_SetNewRange(VL53L0X_Dev_t *pDev,
    VL53L0X_RangingMeasurementData_t *pRange)
{
  if (pRange->RangeStatus == 0)
  {
    if (pDev->LeakyFirst)
    {
      pDev->LeakyFirst = 0;
      pDev->LeakyRange = pRange->RangeMilliMeter;
    }
    else
    {
      pDev->LeakyRange = (pDev->LeakyRange * LeakyFactorFix8
          + (256 - LeakyFactorFix8) * pRange->RangeMilliMeter) >> 8;
    }
  }
  else
  {
    pDev->LeakyFirst = 1;
  }
}

/**
 * Implement the ranging demo with all modes managed through the blue button (short and long press)
 * This function implements a while loop until the blue button is pressed
 * @param UseSensorsMask Mask of any sensors to use if not only one present
 * @param rangingConfig Ranging configuration to be used (same for all sensors)
 */
int RangeDemo(int UseSensorsMask, RangingConfig_e rangingConfig)
{
  int over = 0;
  int status;
  char StrDisplay[5];

  char c;
  int i;
  int nSensorToUse;
  int SingleSensorNo = 0;

  /* Setup all sensors in Single Shot mode */
  SetupSingleShot(rangingConfig);

  /* Which sensor to use ? */
  for (i = 0, nSensorToUse = 0; i < 3; i++)
  {
    if ((UseSensorsMask & (1 << i)) && VL53L0XDevs[i].Present)
    {
      nSensorToUse++;
      if (nSensorToUse == 1)
        SingleSensorNo = i;
    }
  }
  if (nSensorToUse == 0)
  {
    return -1;
  }

  /* Start ranging until blue button is pressed */
  do
  {
    if (nSensorToUse > 1)
    {
      /* Multiple devices */
      strcpy(StrDisplay, "    ");
      for (i = 0; i < 3; i++)
      {
        if (!VL53L0XDevs[i].Present || (UseSensorsMask & (1 << i)) == 0)
          continue;
        /* Call All-In-One blocking API function */
        status = VL53L0X_PerformSingleRangingMeasurement(&VL53L0XDevs[i],
            &RangingMeasurementData);
        if (status)
        {
//          HandleError(ERR_DEMO_RANGE_MULTI); // TODO
        }
        /* Push data logging to UART */
        trace_printf("%d,%u,%d,%d,%d\n", VL53L0XDevs[i].Id, TimeStamp_Get(),
            RangingMeasurementData.RangeStatus,
            RangingMeasurementData.RangeMilliMeter,
            RangingMeasurementData.SignalRateRtnMegaCps);
        /* Store new ranging distance */
        Sensor_SetNewRange(&VL53L0XDevs[i], &RangingMeasurementData);
        /* Translate distance in bar graph (multiple device) */
        c = RangeToLetter(&VL53L0XDevs[i], &RangingMeasurementData);
        StrDisplay[i + 1] = c;
      }
    }
    else
    {
      /* only one sensor */
      /* Call All-In-One blocking API function */
      status = VL53L0X_PerformSingleRangingMeasurement(
          &VL53L0XDevs[SingleSensorNo], &RangingMeasurementData);
      if (status == 0)
      {
        /* Push data logging to UART */
        trace_printf("%d,%u,%d,%d,%d\n", VL53L0XDevs[SingleSensorNo].Id,
            TimeStamp_Get(), RangingMeasurementData.RangeStatus,
            RangingMeasurementData.RangeMilliMeter,
            RangingMeasurementData.SignalRateRtnMegaCps);

        Sensor_SetNewRange(&VL53L0XDevs[SingleSensorNo],
            &RangingMeasurementData);
        /* Display distance in cm */
        if (RangingMeasurementData.RangeStatus == 0)
        {
          sprintf(StrDisplay, "%3dc",
              (int) VL53L0XDevs[SingleSensorNo].LeakyRange / 10);
        }
        else
        {
          sprintf(StrDisplay, "----");
          StrDisplay[0] = VL53L0XDevs[SingleSensorNo].DevLetter;
        }
      }
      else
      {
//        HandleError(ERR_DEMO_RANGE_ONE);
      }
    }
//    XNUCLEO53L0A1_SetDisplayString(StrDisplay);
    /* Check blue button */
    if (!BSP_GetPushButton())
    {
      over = 1;
      break;
    }
  } while (!over);
  /* Wait button to be un-pressed to decide if it is a short or long press */
  status = PusbButton_WaitUnPress();
  return status;
}

void ResetAndDetectSensor(int SetDisplay)
{
  int nSensor;
  nSensor = DetectSensors(SetDisplay);
  /* at least one sensor and if one it must be the built-in one  */
  if ((nSensor <= 0) || (nSensor == 1 && VL53L0XDevs[1].Present == 0))
  {
//    HandleError(ERR_DETECT);
  }
}

/* USER CODE END 0 */

int main2(void)
{

  /* USER CODE BEGIN 1 */
  int UseSensorsMask = 1 << XNUCLEO53L0A1_DEV_CENTER;
  int ExitWithLongPress;
  RangingConfig_e RangingConfig = LONG_RANGE;
  DemoMode_e DemoMode = RANGE_VALUE;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
//  SystemClock_Config();
  /* Initialize all configured peripherals */
//  MX_GPIO_Init();
  /* Initialize timestamping for UART logging */
//  TimeStamp_Init();
  /* USER CODE BEGIN 2 */
//  XNUCLEO53L0A1_Init();
//  uart_printf(WelcomeMsg);
//  XNUCLEO53L0A1_SetDisplayString("53L0");
//  HAL_Delay(WelcomeTime);
  ResetAndDetectSensor(1);

  /* Set VL53L0X API trace level */
  VL53L0X_trace_config(NULL, TRACE_MODULE_NONE, TRACE_LEVEL_NONE,
      TRACE_FUNCTION_NONE); // No Trace
  //VL53L0X_trace_config(NULL,TRACE_MODULE_ALL, TRACE_LEVEL_ALL, TRACE_FUNCTION_ALL); // Full trace

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* Display demo mode */
//    XNUCLEO53L0A1_SetDisplayString(DemoModeTxt[DemoMode]);
//    HAL_Delay(ModeChangeDispTime);
    /* Display Ranging config */
//    XNUCLEO53L0A1_SetDisplayString(RangingConfigTxt[RangingConfig]);
//    HAL_Delay(ModeChangeDispTime);
    /* Reset and Detect all sensors */
    ResetAndDetectSensor(0);

    /* Start Ranging demo */
    ExitWithLongPress = RangeDemo(UseSensorsMask, RangingConfig);

    /* Blue button has been pressed (long or short press) */
    if (ExitWithLongPress)
    {
      /* Long press : change demo mode if multiple sensors present*/
      if (nDevPresent > 1)
      {
        /* If more than one sensor is present then toggle demo mode */
        DemoMode = (DemoMode == RANGE_VALUE) ? BAR_GRAPH : RANGE_VALUE;
        UseSensorsMask =
            (DemoMode == BAR_GRAPH) ? 0x7 : 1 << XNUCLEO53L0A1_DEV_CENTER;
      }
    }
    else
    {
      /* Short press : change ranging config */
      RangingConfig =
          (RangingConfig == LONG_RANGE) ?
              HIGH_SPEED :
              ((RangingConfig == HIGH_SPEED) ? HIGH_ACCURACY : LONG_RANGE);
    }
    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

