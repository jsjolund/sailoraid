/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2017 STMicroelectronics
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "bluenrg_interface.h"
#include "stm32f4xx_nucleo.h"
#include "imu.h"
#include "gps.h"
#include "sensor.h"
#include "serial.h"
#include "MadgwickAHRS.h"
#include "state.h"
#include "range.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

// System sample rates and USB/BT output rates
// Lowest rate possible is 1/(0xffffffff/10^6) = 0.0002 Hz
#define IMU_SAMPLE_RATE 100.0
#define ENV_SAMPLE_RATE 1.0
#define ADC_SAMPLE_RATE 1.0
#define RANGE_SAMPLE_RATE 10.0
#define USB_ENV_OUTPUT_RATE 0.1
#define USB_GPS_OUTPUT_RATE 1.0
#define USB_IMU_OUTPUT_RATE 60.0
#define USB_RANGE_OUTPUT_RATE 10.0
#define USB_MATLAB_OUTPUT_RATE 100.0
#define BT_ENV_OUTPUT_RATE 1.0
#define BT_GPS_OUTPUT_RATE 1.0
#define BT_IMU_OUTPUT_RATE 50.0
#define BT_RANGE_OUTPUT_RATE 10.0

extern volatile uint8_t set_connectable;
extern volatile int connected;
volatile uint8_t adcFinished = 1;
volatile uint8_t adcValues[] = { 0, 0, 0, 0 };
SensorState_t sensor;

typedef struct Task_Data
{
  uint32_t period;
  uint32_t previous;
  BOOL echo;
} Task_Data;

/**
 * Check if it is time to run a task
 */
static uint32_t taskTimeout(Task_Data *data, TIM_HandleTypeDef *tim)
{
  uint32_t deltaTime = tim->Instance->CNT - data->previous;
  if (deltaTime >= data->period)
  {
    // If deadline has passed more than once add the period multiple times
    // to avoid running the task many times. We don't want to add deltaTime,
    // since this would result in drift
    int k = deltaTime / data->period;
    data->previous += k * data->period;
    return deltaTime;
  }
  return 0;
}

static Task_Data imuSampleTask = { .period = (uint32_t) (1000000.0 / IMU_SAMPLE_RATE) };
static Task_Data envSampleTask = { .period = (uint32_t) (1000000.0 / ENV_SAMPLE_RATE) };
static Task_Data adcSampleTask = { .period = (uint32_t) (1000000.0 / ADC_SAMPLE_RATE) };
static Task_Data rangeSampleTask = { .period = (uint32_t) (1000000.0 / RANGE_SAMPLE_RATE) };
static Task_Data usbEnvOutputTask = { .period = (uint32_t) (1000000.0 / USB_ENV_OUTPUT_RATE) };
static Task_Data usbGpsOutputTask = { .period = (uint32_t) (1000000.0 / USB_GPS_OUTPUT_RATE) };
static Task_Data usbImuOutputTask = { .period = (uint32_t) (1000000.0 / USB_IMU_OUTPUT_RATE) };
static Task_Data usbRangeOutputTask = { .period = (uint32_t) (1000000.0 / USB_RANGE_OUTPUT_RATE) };
static Task_Data usbMatlabOutputTask = { .period = (uint32_t) (1000000.0 / USB_MATLAB_OUTPUT_RATE) };
static Task_Data btEnvOutputTask = { .period = (uint32_t) (1000000.0 / BT_ENV_OUTPUT_RATE) };
static Task_Data btGpsOutputTask = { .period = (uint32_t) (1000000.0 / BT_GPS_OUTPUT_RATE) };
static Task_Data btImuOutputTask = { .period = (uint32_t) (1000000.0 / BT_IMU_OUTPUT_RATE) };
static Task_Data btRangeOutputTask = { .period = (uint32_t) (1000000.0 / BT_RANGE_OUTPUT_RATE) };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  // ADC has finished reading and converting the values on the four pins
  adcValues[0] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
//  adcValues[1] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
//  adcValues[2] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
//  adcValues[3] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_4);
  adcFinished = 1;
}

void User_Process(void)
{
  if (set_connectable)
  {
    setConnectable();
    set_connectable = FALSE;
  }
}

static void SetEcho(BOOL echo, Task_Data *task)
{
  task->echo = echo;
  task->previous = htim2.Instance->CNT - task->period;
}

void IMUecho(BOOL echo)
{
  SetEcho(echo, &usbImuOutputTask);
}

void GPSecho(BOOL echo)
{
  SetEcho(echo, &usbGpsOutputTask);
}

void ENVecho(BOOL echo)
{
  SetEcho(echo, &usbEnvOutputTask);
}

void RangeEcho(BOOL echo)
{
  SetEcho(echo, &usbRangeOutputTask);
}

void MATLABecho(BOOL echo)
{
  SetEcho(echo, &usbMatlabOutputTask);
}

/**
 * @brief  EXTI line detection callback.
 * @param  Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
  case BNRG_SPI_EXTI_Pin:
    HCI_Isr();
    break;
  case USER_BUTTON_Pin:
    break;
  default:
    break;
  }
}

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  memset(&sensor, 0, sizeof(SensorState_t));
  SensorAxes_t ACC_Value; /*!< Acceleration Value */
  SensorAxes_t GYR_Value; /*!< Gyroscope Value */
  SensorAxes_t MAG_Value; /*!< Magnetometer Value */
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();

  /* USER CODE BEGIN 2 */
  SerialInit(&huart2, &huart1);

  /* Configure Bluetooth GATT server */
  BTinit();

  /* ADC (not currently used) */
  HAL_ADC_Start(&hadc1);

  /* Configure LED2 */
  BSP_LED_Init(LED2);

  /* Configure the User Button in GPIO Mode */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

  /* Configure the system clock */
  SystemClock_Config();

  /* GPS */
  GPSinit();

  /* IMU */
  IMUinit();
  MadgwickInit(IMU_SAMPLE_RATE);

  /* Range */
  VL53L0X_Dev_t rangeDev = { .Id = 0, .I2cHandle = &hi2c1, .I2cDevAddr = 0x52 };
  Range_Sensor_Init(&rangeDev);
  RangingConfig_e rangingConfig = LONG_RANGE;
  Range_Sensor_Setup_Single_Shot(&rangeDev, rangingConfig);
  VL53L0X_RangingMeasurementData_t rangeData;

  // Start the task timer
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_Base_Start(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HCI_Process();
    User_Process();

    /*** INPUTS ***/
    if (taskTimeout(&imuSampleTask, &htim2))
    {
      // Update the IMU
      Accelero_Sensor_Handler(&ACC_Value);
      Gyro_Sensor_Handler(&GYR_Value);
      Magneto_Sensor_Handler(&MAG_Value);
      sensor.imu.gx = ((float) GYR_Value.AXIS_X) / 1000;
      sensor.imu.gy = ((float) GYR_Value.AXIS_Y) / 1000;
      sensor.imu.gz = ((float) GYR_Value.AXIS_Z) / 1000;
      sensor.imu.ax = ((float) ACC_Value.AXIS_X) / 1000;
      sensor.imu.ay = ((float) ACC_Value.AXIS_Y) / 1000;
      sensor.imu.az = ((float) ACC_Value.AXIS_Z) / 1000;
      sensor.imu.mx = (((float) MAG_Value.AXIS_X) - MAG_X_BIAS) * MAG_X_SCL / 1000;
      sensor.imu.my = -(((float) MAG_Value.AXIS_Y) - MAG_Y_BIAS) * MAG_Y_SCL / 1000; // LSM6DSL has opposite y-axis compared to LSM303 on IMU board
      sensor.imu.mz = (((float) MAG_Value.AXIS_Z) - MAG_X_BIAS) * MAG_Z_SCL / 1000;
      MadgwickUpdate(sensor.imu.gx, sensor.imu.gy, sensor.imu.gz, sensor.imu.ax, sensor.imu.ay, sensor.imu.az, sensor.imu.mx, sensor.imu.my, sensor.imu.mz);
      sensor.imu.roll = MadgwickGetRoll();
      sensor.imu.pitch = -MadgwickGetPitch();
      sensor.imu.yaw = -MadgwickGetYaw();
    }
    if (taskTimeout(&envSampleTask, &htim2))
    {
      // Update environment sensors
      Pressure_Sensor_Handler(&sensor.env.pressure);
      Humidity_Sensor_Handler(&sensor.env.humidity);
      Temperature_Sensor_Handler(&sensor.env.temperature);
    }
    if (adcFinished && (taskTimeout(&adcSampleTask, &htim2)))
    {
      // Update ADC
      HAL_ADCEx_InjectedStart_IT(&hadc1);
      adcFinished = 0;
    }
    if (taskTimeout(&rangeSampleTask, &htim2))
    {
      // Get previous range measurement
      if (Range_Sensor_Get_Measurement(&rangeDev, &rangeData) == VL53L0X_ERROR_NONE)
      {
        // Filtered distance in cm
        sensor.range.range0 = (rangeData.RangeStatus == 0) ? (int) rangeDev.LeakyRange / 10 : INFINITY;
      }
      // Start new range measurement
      Range_Sensor_Start_New_Measurement(&rangeDev, &rangeData);
    }

    /*** OUTPUTS ***/
    if (taskTimeout(&btImuOutputTask, &htim2))
    {
      // Update orientation, GPS and environmental data on Bluetooth GATT server
      Orientation_Update(sensor.imu.roll, sensor.imu.pitch, sensor.imu.yaw);
    }
    if (taskTimeout(&btGpsOutputTask, &htim2))
    {
      // To avoid race conditions, disable GPS update while reading
      HAL_NVIC_DisableIRQ(GPS_USART_IRQn);
      GPS_Update(sensor.gps.pos.longitude, sensor.gps.pos.latitude, sensor.gps.pos.elevation, sensor.gps.pos.speed, sensor.gps.pos.direction);
      HAL_NVIC_EnableIRQ(GPS_USART_IRQn);
    }
    if (taskTimeout(&btEnvOutputTask, &htim2))
    {
      Temp_Update(sensor.env.temperature);
      Humidity_Update(sensor.env.humidity);
      Press_Update(sensor.env.pressure);
    }
    if (taskTimeout(&btRangeOutputTask, &htim2))
    {
      Range_Update(sensor.range.range0);
    }
    if (usbImuOutputTask.echo && taskTimeout(&usbImuOutputTask, &htim2))
    {
      printf("%3.4f %3.4f %3.4f\r\n", sensor.imu.roll, sensor.imu.pitch, sensor.imu.yaw);
    }
    if (usbGpsOutputTask.echo && taskTimeout(&usbGpsOutputTask, &htim2))
    {
      // To avoid race conditions, disable GPS update while reading
      HAL_NVIC_DisableIRQ(GPS_USART_IRQn);
      printf("date %d/%d %d, time %d:%d, lat %f, lon %f, elv %f, speed %f, dir %f satuse %d, satview %d\r\n", sensor.gps.time.day, sensor.gps.time.month,
          sensor.gps.time.year, sensor.gps.time.hour, sensor.gps.time.min, sensor.gps.pos.latitude, sensor.gps.pos.longitude, sensor.gps.pos.elevation,
          sensor.gps.pos.speed, sensor.gps.pos.direction, sensor.gps.info.satUse, sensor.gps.info.satView);
      HAL_NVIC_EnableIRQ(GPS_USART_IRQn);
    }
    if (usbEnvOutputTask.echo && taskTimeout(&usbEnvOutputTask, &htim2))
    {
      printf("humidity %3.4f, pressure %3.4f, temperature %3.4f\r\n", sensor.env.humidity, sensor.env.pressure, sensor.env.temperature);
    }
    if (usbRangeOutputTask.echo && taskTimeout(&usbRangeOutputTask, &htim2))
    {
      if (sensor.range.range0 < INFINITY)
        printf("range %f cm\n", sensor.range.range0);
      else
        printf("range ∞\n");
    }
    if (usbMatlabOutputTask.echo && taskTimeout(&usbMatlabOutputTask, &htim2))
    {
      HAL_NVIC_DisableIRQ(GPS_USART_IRQn);
      SerialUsbTransmit((char*) &sensor, sizeof(sensor));
      HAL_NVIC_EnableIRQ(GPS_USART_IRQn);
      char sync[] = { 0xff, 0xff, 0xff, 0xff };
      SerialUsbTransmit(sync, 4);
    }
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /**Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE()
  ;

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure the Systick interrupt time
   */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  /**Configure the Systick
   */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;
  ADC_InjectionConfTypeDef sConfigInjected;

  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
   */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
   */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_14;
  sConfigInjected.InjectedRank = 1;
  sConfigInjected.InjectedNbrOfConversion = 1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_112CYCLES;
  sConfigInjected.ExternalTrigInjecConvEdge =
  ADC_EXTERNALTRIGINJECCONVEDGE_NONE;
  sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 99;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffffffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 4800;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 230400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE()
  ;
  __HAL_RCC_DMA1_CLK_ENABLE()
  ;

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE()
  ;
  __HAL_RCC_GPIOH_CLK_ENABLE()
  ;
  __HAL_RCC_GPIOA_CLK_ENABLE()
  ;
  __HAL_RCC_GPIOB_CLK_ENABLE()
  ;

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPS_NRST_Pin | GPS_ON_OFF_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BNRG_SPI_CS_Pin | LED2_Pin | BNRG_SPI_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RANGE_SHDN_GPIO_Port, RANGE_SHDN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GPS_NRST_Pin */
  GPIO_InitStruct.Pin = GPS_NRST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPS_NRST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : M_INT2_O_Pin */
  GPIO_InitStruct.Pin = M_INT2_O_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(M_INT2_O_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BNRG_SPI_IRQ_Pin */
  GPIO_InitStruct.Pin = BNRG_SPI_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BNRG_SPI_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BNRG_SPI_CS_Pin */
  GPIO_InitStruct.Pin = BNRG_SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BNRG_SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_Pin BNRG_SPI_RESET_Pin */
  GPIO_InitStruct.Pin = LED2_Pin | BNRG_SPI_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LPS22H_INT1_O_Pin LSM6DSL_INT2_O_Pin LSM6DSL_INT1_O_Pin */
  GPIO_InitStruct.Pin = LPS22H_INT1_O_Pin | LSM6DSL_INT2_O_Pin | LSM6DSL_INT1_O_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : GPS_ON_OFF_Pin RANGE_SHDN_Pin */
  GPIO_InitStruct.Pin = GPS_ON_OFF_Pin | RANGE_SHDN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
