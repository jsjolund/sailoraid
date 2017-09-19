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
#include "osal.h"
#include "sensor_service.h"
#include "debug.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"

#include "stm32f4xx_nucleo.h"

#include "sensor.h"
#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_gyro.h"
#include "x_nucleo_iks01a2_accelero.h"
#include "x_nucleo_iks01a2_magneto.h"
#include "x_nucleo_iks01a2_humidity.h"
#include "x_nucleo_iks01a2_pressure.h"
#include "x_nucleo_iks01a2_temperature.h"
#include "MadgwickAHRS.h"

#include "com.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define BDADDR_SIZE 6
extern volatile uint8_t set_connectable;
extern volatile int connected;
extern AxesRaw_t axes_data;
uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */

void *ACCELERO_handle = NULL;
void *GYRO_handle = NULL;
void *MAGNETO_handle = NULL;
void *HUMIDITY_handle = NULL;
void *TEMPERATURE_handle = NULL;
void *PRESSURE_handle = NULL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
void User_Process(AxesRaw_t* p_axes);

void enableDisableSensors(void);
void initializeAllSensors(void);
void Temperature_Sensor_Handler(float *TEMPERATURE_Value);
void Humidity_Sensor_Handler(float *HUMIDITY_Value);
void Pressure_Sensor_Handler(float *PRESSURE_Value);
void Accelero_Sensor_Handler(SensorAxes_t *ACC_Value);
void Magneto_Sensor_Handler(SensorAxes_t *MAG_Value);
void Gyro_Sensor_Handler(SensorAxes_t *GYR_Value);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  const char *name = "BlueNRG";
  uint8_t SERVER_BDADDR[] = { 0x12, 0x34, 0x00, 0xE1, 0x80, 0x03 };
  uint8_t bdaddr[BDADDR_SIZE];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

  uint8_t hwVersion;
  uint16_t fwVersion;

  int ret;

  SensorAxes_t ACC_Value; /*!< Acceleration Value */
  SensorAxes_t GYR_Value; /*!< Gyroscope Value */
  SensorAxes_t MAG_Value; /*!< Magnetometer Value */
  float PRESSURE_Value; /*!< Pressure Value */
  float HUMIDITY_Value; /*!< Humidity Value */
  float TEMPERATURE_Value; /*!< Temperature Value */

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  /* Initialize UART */
  USARTConfig();

  /* Configure LED2 */
  BSP_LED_Init(LED2);

  /* Configure the User Button in GPIO Mode */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize the BlueNRG SPI driver */
  BNRG_SPI_Init();

  /* Initialize the BlueNRG HCI */
  HCI_Init();

  /* Reset BlueNRG hardware */
  BlueNRG_RST();

  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  /*
   * Reset BlueNRG again otherwise we won't
   * be able to change its MAC address.
   * aci_hal_write_config_data() must be the first
   * command after reset otherwise it will fail.
   */
  BlueNRG_RST();

  printf("HWver %d, FWver %d\n", hwVersion, fwVersion);

  if (hwVersion > 0x30)
  { /* X-NUCLEO-IDB05A1 expansion board is used */
    bnrg_expansion_board = IDB05A1;
    /*
     * Change the MAC address to avoid issues with Android cache:
     * if different boards have the same MAC address, Android
     * applications unless you restart Bluetooth on tablet/phone
     */
    SERVER_BDADDR[5] = 0x02;
  }

  /* The Nucleo board must be configured as SERVER */
  Osal_MemCpy(bdaddr, SERVER_BDADDR, sizeof(SERVER_BDADDR));

  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
  CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if (ret)
  {
    printf("Setting BD_ADDR failed.\n");
  }

  ret = aci_gatt_init();
  if (ret)
  {
    printf("GATT_Init failed.\n");
  }

  if (bnrg_expansion_board == IDB05A1)
  {
    ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }
  else
  {
    ret = aci_gap_init_IDB04A1(GAP_PERIPHERAL_ROLE_IDB04A1, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }

  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("GAP_Init failed.\n");
  }

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, strlen(name), (uint8_t *) name);

  if (ret)
  {
    printf("aci_gatt_update_char_value failed.\n");
    while (1)
      ;
  }

  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
  OOB_AUTH_DATA_ABSENT,
  NULL, 7, 16,
  USE_FIXED_PIN_FOR_PAIRING, 123456,
  BONDING);
  if (ret == BLE_STATUS_SUCCESS)
  {
    printf("BLE Stack Initialized.\n");
  }

  printf("SERVER: BLE Stack Initialized\n");

  ret = Add_Acc_Service();

  if (ret == BLE_STATUS_SUCCESS)
    printf("Acc service added successfully.\n");
  else
    printf("Error while adding Acc service.\n");

  ret = Add_Environmental_Sensor_Service();

  if (ret == BLE_STATUS_SUCCESS)
    printf("Environmental Sensor service added successfully.\n");
  else
    printf("Error while adding Environmental Sensor service.\n");

#if NEW_SERVICES
  /* Instantiate Timer Service with two characteristics:
   * - seconds characteristic (Readable only)
   * - minutes characteristics (Readable and Notifiable )
   */
  ret = Add_Time_Service();

  if(ret == BLE_STATUS_SUCCESS)
  printf("Time service added successfully.\n");
  else
  printf("Error while adding Time service.\n");

  /* Instantiate LED Button Service with one characteristic:
   * - LED characteristic (Readable and Writable)
   */
  ret = Add_LED_Service();

  if(ret == BLE_STATUS_SUCCESS)
  printf("LED service added successfully.\n");
  else
  printf("Error while adding LED service.\n");
#endif

  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1, 4);

  /* IMU */
  initializeAllSensors();
  enableDisableSensors();

  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_Base_Start(&htim2);

  int serialDebugRate = 100;
  int imuSampleRate = 416;
  int envSensorRate = 10;

  MadgwickInit(imuSampleRate);

  uint32_t imuPeriod = 1000000 / imuSampleRate;
  uint32_t imuPrevious = htim2.Instance->CNT;
  uint32_t envSensorPeriod = 1000000 / envSensorRate;
  uint32_t envSensorPrevious = htim2.Instance->CNT;
  uint32_t serialPeriod = 1000000 / serialDebugRate;
  uint32_t serialPrevious = htim2.Instance->CNT;

  float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0, mx = 0, my = 0, mz = 0, yaw = 0, pitch = 0, roll = 0;

  while (1)
  {
    HCI_Process();
    User_Process(&axes_data);

#if NEW_SERVICES
    Update_Time_Characteristics();
#endif

    uint32_t microsNow = htim2.Instance->CNT;

    if (microsNow - imuPrevious >= imuPeriod)
    {
      Accelero_Sensor_Handler(&ACC_Value);
      Gyro_Sensor_Handler(&GYR_Value);
      Magneto_Sensor_Handler(&MAG_Value);
      gx = ((float) GYR_Value.AXIS_X)/1000;
      gy = ((float) GYR_Value.AXIS_Y)/1000;
      gz = ((float) GYR_Value.AXIS_Z)/1000;
      ax = ((float) ACC_Value.AXIS_X)/1000;
      ay = ((float) ACC_Value.AXIS_Y)/1000;
      az = ((float) ACC_Value.AXIS_Z)/1000;
      mx = ((float) MAG_Value.AXIS_X)/1000;
      my = ((float) MAG_Value.AXIS_Y)/1000;
      mz = ((float) MAG_Value.AXIS_Z)/1000;
      MadgwickUpdate(gx, gy, gz, ax, ay, az, mx, my, mz);
      imuPrevious += imuPeriod;
    }
    if (microsNow - serialPrevious >= serialPeriod)
    {
      roll = MadgwickGetRoll();
      pitch = MadgwickGetPitch();
      yaw = MadgwickGetYaw();
//      printf("%3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f\r\n", roll, pitch, yaw, gx, gy, gz, ax, ay, az, mx, my, mz);
      printf("%3.4f %3.4f %3.4f\r\n", roll, pitch, yaw);
      serialPrevious += serialPeriod;
    }
    if (microsNow - envSensorPrevious >= envSensorPeriod)
    {
      Pressure_Sensor_Handler(&PRESSURE_Value);
      Humidity_Sensor_Handler(&HUMIDITY_Value);
      Temperature_Sensor_Handler(&TEMPERATURE_Value);
      envSensorPrevious += envSensorPeriod;
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
  __HAL_RCC_PWR_CLK_ENABLE();

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
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
  huart2.Init.BaudRate = 115200;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BNRG_SPI_CS_Pin|LED2_Pin|BNRG_SPI_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

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
  GPIO_InitStruct.Pin = LED2_Pin|BNRG_SPI_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LPS22H_INT1_O_Pin LSM6DSL_INT2_O_Pin LSM6DSL_INT1_O_Pin */
  GPIO_InitStruct.Pin = LPS22H_INT1_O_Pin|LSM6DSL_INT2_O_Pin|LSM6DSL_INT1_O_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send the updated acceleration data to the remote client.
 *
 * @param  AxesRaw_t* p_axes
 * @retval None
 */
void User_Process(AxesRaw_t* p_axes)
{
  if (set_connectable)
  {
    setConnectable();
    set_connectable = FALSE;
  }

  /* Check if the user has pushed the button */
  if (BSP_PB_GetState(BUTTON_KEY) == RESET)
  {
    while (BSP_PB_GetState(BUTTON_KEY) == RESET)
      ;

    //BSP_LED_Toggle(LED2); //used for debugging (BSP_LED_Init() above must be also enabled)

    if (connected)
    {
      /* Update acceleration data */
      p_axes->AXIS_X += 100;
      p_axes->AXIS_Y += 100;
      p_axes->AXIS_Z += 100;
      //printf("ACC: X=%6d Y=%6d Z=%6d\r\n", p_axes->AXIS_X, p_axes->AXIS_Y, p_axes->AXIS_Z);
      Acc_Update(p_axes);
    }
  }
}

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
void initializeAllSensors(void)
{
  /* Try to use automatic discovery. By default use LSM6DSL on board */
  BSP_ACCELERO_Init(ACCELERO_SENSORS_AUTO, &ACCELERO_handle);
  /* Try to use automatic discovery. By default use LSM6DSL on board */
  BSP_GYRO_Init(GYRO_SENSORS_AUTO, &GYRO_handle);
  /* Try to use automatic discovery. By default use LSM303AGR on board */
  BSP_MAGNETO_Init(MAGNETO_SENSORS_AUTO, &MAGNETO_handle);
  /* Try to use automatic discovery. By default use HTS221 on board */
  BSP_HUMIDITY_Init(HUMIDITY_SENSORS_AUTO, &HUMIDITY_handle);
  /* Try to use automatic discovery. By default use HTS221 on board */
  BSP_TEMPERATURE_Init(TEMPERATURE_SENSORS_AUTO, &TEMPERATURE_handle);
  /* Try to use automatic discovery. By default use LPS22HB on board */
  BSP_PRESSURE_Init(PRESSURE_SENSORS_AUTO, &PRESSURE_handle);
}

/**
 * @brief  Enable/disable desired sensors
 * @param  None
 * @retval None
 */
void enableDisableSensors(void)
{
  BSP_PRESSURE_Sensor_Enable(PRESSURE_handle);
  BSP_HUMIDITY_Sensor_Enable(HUMIDITY_handle);
  BSP_TEMPERATURE_Sensor_Enable(TEMPERATURE_handle);
  BSP_ACCELERO_Sensor_Enable(ACCELERO_handle);
  BSP_GYRO_Sensor_Enable(GYRO_handle);
  BSP_MAGNETO_Sensor_Enable(MAGNETO_handle);
}

/**
 * @brief  EXTI line detection callback.
 * @param  Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  HCI_Isr();

  /* User button pressed */
  if (GPIO_Pin == USER_BUTTON_Pin)
  {
  }
}

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
