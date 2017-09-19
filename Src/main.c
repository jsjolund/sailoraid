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

#include "accelerometer.h"
#include "stm32f4xx_nucleo.h"
#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_gyro.h"
#include "x_nucleo_iks01a2_accelero.h"
#include "x_nucleo_iks01a2_magneto.h"
#include "com.h"
#include "MadgwickAHRS.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define BDADDR_SIZE 6
extern volatile uint8_t set_connectable;
extern volatile int connected;
extern AxesRaw_t axes_data;
uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */

/**
 * @brief  Handle DEMO State Machine
 */
typedef enum demoFifoStatus
{
  DEMO_STATUS_IDLE, DEMO_STATUS_SET_FIFO_CONTINUOUS_MODE, DEMO_STATUS_FIFO_RUN, DEMO_STATUS_FIFO_DOWNLOAD, DEMO_STATUS_SET_FIFO_BYPASS_MODE
} DEMO_FIFO_STATUS;

#define FIFO_WATERMARK   11 /*!< FIFO size limit */
#define SAMPLE_LIST_MAX  10 /*!< Max. number of values (X,Y,Z) to be printed to UART */
#define LSM6DSL_SAMPLE_ODR    ODR_LOW /*!< Sample Output Data Rate [Hz] */
#define LSM6DSL_FIFO_MAX_ODR  416    /*!< LSM6DSL FIFO maximum ODR */
#define FIFO_INDICATION_DELAY  100 /*!< When FIFO event ocurs, LED is ON for at least this period [ms] */
#define PATTERN_GYR_X_AXIS  0 /*!< Pattern of gyro X axis */
#define PATTERN_GYR_Y_AXIS  1 /*!< Pattern of gyro Y axis */
#define PATTERN_GYR_Z_AXIS  2 /*!< Pattern of gyro Z axis */
#define PATTERN_ACC_X_AXIS  0 /*!< Pattern of accelero X axis */
#define PATTERN_ACC_Y_AXIS  1 /*!< Pattern of accelero Y axis */
#define PATTERN_ACC_Z_AXIS  2 /*!< Pattern of accelero Z axis */
#define UART_TRANSMIT_TIMEOUT  5000
#define MAX_BUF_SIZE 256
/* This variable MUST be volatile because it could change into a ISR */
static volatile uint8_t memsIntDetected = 0;
static char dataOut[MAX_BUF_SIZE];
static void *LSM6DSL_G_0_handle = NULL;
static void *LSM6DSL_X_0_handle = NULL;
static void *LSM303AGR_M_0_handle = NULL;
/* This variable MUST be volatile because it could change into a ISR */
static volatile DEMO_FIFO_STATUS demoFifoStatus = DEMO_STATUS_SET_FIFO_BYPASS_MODE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);

/* USER CODE BEGIN PFP */
void User_Process(AxesRaw_t* p_axes);

static DrvStatusTypeDef Init_All_Sensors(void);
static DrvStatusTypeDef Enable_All_Sensors(void);
static DrvStatusTypeDef LSM6DSL_FIFO_Set_Bypass_Mode(void);
static DrvStatusTypeDef LSM6DSL_FIFO_Set_Continuous_Mode(void);
static DrvStatusTypeDef LSM6DSL_Read_All_FIFO_Data(SensorAxes_t *gyro, SensorAxes_t *acc, SensorAxes_t *mag);
static DrvStatusTypeDef LSM6DSL_Read_Single_FIFO_Pattern_Cycle(uint16_t sampleIndex, SensorAxes_t *acc, SensorAxes_t *gyro, SensorAxes_t *mag);
static DrvStatusTypeDef LSM6DSL_FIFO_Demo_Config(void);
static void LSM303AGR_Read_Magnometer(SensorAxes_t *MAG_Value);
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
  MadgwickInit(LSM6DSL_FIFO_MAX_ODR);

  uint8_t fifo_full_status = 0;
  uint16_t samplesInFIFO = 0;
  uint16_t oldSamplesInFIFO = 0;
  SensorAxes_t gyro, acc, mag;

  /* USER CODE END 1 */

  /* MCU Configuration-------------------------------u _printf_float----------------------------*/

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
  if (Init_All_Sensors() == COMPONENT_ERROR)
  {
    printf("Error initiating sensors\n");
  }

  if (Enable_All_Sensors() == COMPONENT_ERROR)
  {
    printf("Error enabling sensors\n");
  }

  /* Configure LSM6DSL Sensor for the DEMO application */
  if (LSM6DSL_FIFO_Demo_Config() == COMPONENT_ERROR)
  {
    printf("Error configuring sensors\n");
  }

  printf("\r\n------ LSM6DSL FIFO Continuous Mode DEMO ------\r\n");

  while (1)
  {
    HCI_Process();
    User_Process(&axes_data);
#if NEW_SERVICES
    Update_Time_Characteristics();
#endif

    /* Handle DEMO State Machine */
    switch (demoFifoStatus)
    {
    case DEMO_STATUS_IDLE:
      break;

    case DEMO_STATUS_SET_FIFO_CONTINUOUS_MODE:

      if (LSM6DSL_FIFO_Set_Continuous_Mode() == COMPONENT_ERROR)
      {
        printf("Error setting continuous mode\n");
      }
      demoFifoStatus = DEMO_STATUS_FIFO_RUN;
      break;

    case DEMO_STATUS_FIFO_RUN:

      /* Get num of unread FIFO samples before reading data */
      if (BSP_GYRO_FIFO_Get_Num_Of_Samples_Ext(LSM6DSL_G_0_handle, &samplesInFIFO) == COMPONENT_ERROR)
      {
        return COMPONENT_ERROR;
      }

//      /* Print dot realtime when each new data is stored in FIFO */
//      if (samplesInFIFO != oldSamplesInFIFO)
//      {
//        oldSamplesInFIFO = samplesInFIFO;
//        snprintf(dataOut, MAX_BUF_SIZE, ".");
//        HAL_UART_Transmit(&UartHandle, (uint8_t *) dataOut, strlen(dataOut), UART_TRANSMIT_TIMEOUT);
//      }

      if (memsIntDetected)
      {
        demoFifoStatus = DEMO_STATUS_FIFO_DOWNLOAD;
        memsIntDetected = 0;
      }
      break;

    case DEMO_STATUS_FIFO_DOWNLOAD:

      /* Print data if FIFO is full */
      if (BSP_GYRO_FIFO_Get_Full_Status_Ext(LSM6DSL_G_0_handle, &fifo_full_status) == COMPONENT_ERROR)
      {
        printf("Error getting full status ext from FIFO\n");
      }

      if (fifo_full_status == 1)
      {
        BSP_LED_On(LED2);

        if (LSM6DSL_Read_All_FIFO_Data(&gyro, &acc, &mag) == COMPONENT_ERROR)
        {
          printf("Error reading all FIFO data\n");
        }

        BSP_LED_Off(LED2);

        demoFifoStatus = DEMO_STATUS_FIFO_RUN;
      }
      break;

    case DEMO_STATUS_SET_FIFO_BYPASS_MODE:

      if (LSM6DSL_FIFO_Set_Bypass_Mode() == COMPONENT_ERROR)
      {
        printf("Error setting bypass mode\n");
      }

      memsIntDetected = 0;
      samplesInFIFO = 0;
      oldSamplesInFIFO = 0;
      demoFifoStatus = STATUS_IDLE;
      break;

    default:
      break;
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
  __HAL_RCC_GPIOC_CLK_ENABLE()
  ;
  __HAL_RCC_GPIOH_CLK_ENABLE()
  ;
  __HAL_RCC_GPIOA_CLK_ENABLE()
  ;
  __HAL_RCC_GPIOB_CLK_ENABLE()
  ;

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BNRG_SPI_CS_Pin | LED2_Pin | BNRG_SPI_RESET_Pin, GPIO_PIN_RESET);

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
 * @brief  Initialize all sensors
 * @param  None
 * @retval COMPONENT_OK
 * @retval COMPONENT_ERROR
 */
static DrvStatusTypeDef Init_All_Sensors(void)
{
  if (BSP_ACCELERO_Init(LSM6DSL_X_0, &LSM6DSL_X_0_handle) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }
  if (BSP_GYRO_Init(LSM6DSL_G_0, &LSM6DSL_G_0_handle) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }
  if (BSP_MAGNETO_Init(LSM303AGR_M_0, &LSM303AGR_M_0_handle) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }
  return COMPONENT_OK;
}

/**
 * @brief  Enable all sensors
 * @param  None
 * @retval COMPONENT_OK
 * @retval COMPONENT_ERROR
 */
static DrvStatusTypeDef Enable_All_Sensors(void)
{
  if (BSP_GYRO_Sensor_Enable(LSM6DSL_G_0_handle) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }
  if (BSP_ACCELERO_Sensor_Enable(LSM6DSL_X_0_handle) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }
  if (BSP_MAGNETO_Sensor_Enable(LSM303AGR_M_0_handle) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }
  return COMPONENT_OK;
}

/**
 * @brief  Configure FIFO
 * @param  None
 * @retval COMPONENT_OK
 * @retval COMPONENT_ERROR
 */
static DrvStatusTypeDef LSM6DSL_FIFO_Demo_Config(void)
{
  if (BSP_GYRO_Set_ODR(LSM6DSL_G_0_handle, LSM6DSL_SAMPLE_ODR) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  /* Set gyro FIFO decimation */
  if (BSP_GYRO_FIFO_Set_Decimation_Ext(LSM6DSL_G_0_handle, LSM6DSL_ACC_GYRO_DEC_FIFO_G_NO_DECIMATION) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  /* Set FIFO ODR to highest value */
  if (BSP_GYRO_FIFO_Set_ODR_Value_Ext(LSM6DSL_G_0_handle, LSM6DSL_FIFO_MAX_ODR) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  /* Set FIFO_FULL on INT1 */
  if (BSP_GYRO_FIFO_Set_INT1_FIFO_Full_Ext(LSM6DSL_G_0_handle, LSM6DSL_ACC_GYRO_INT1_FULL_FLAG_ENABLED) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  /* Set FIFO watermark */
  if (BSP_GYRO_FIFO_Set_Watermark_Level_Ext(LSM6DSL_G_0_handle, FIFO_WATERMARK) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  /* Set FIFO depth to be limited to watermark threshold level  */
  if (BSP_GYRO_FIFO_Set_Stop_On_Fth_Ext(LSM6DSL_G_0_handle, LSM6DSL_ACC_GYRO_STOP_ON_FTH_ENABLED) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief  Set FIFO bypass mode
 * @param  None
 * @retval COMPONENT_OK
 * @retval COMPONENT_ERROR
 */
static DrvStatusTypeDef LSM6DSL_FIFO_Set_Bypass_Mode(void)
{
  if (BSP_GYRO_FIFO_Set_Mode_Ext(LSM6DSL_G_0_handle, LSM6DSL_ACC_GYRO_FIFO_MODE_BYPASS) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }
  if (BSP_ACCELERO_FIFO_Set_Mode_Ext(LSM6DSL_X_0_handle, LSM6DSL_ACC_GYRO_FIFO_MODE_BYPASS) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  snprintf(dataOut, MAX_BUF_SIZE, "\r\nFIFO is stopped in Bypass mode.\r\n");
  HAL_UART_Transmit(&UartHandle, (uint8_t *) dataOut, strlen(dataOut), UART_TRANSMIT_TIMEOUT);

  snprintf(dataOut, MAX_BUF_SIZE, "\r\nPress USER button to start the DEMO...\r\n");
  HAL_UART_Transmit(&UartHandle, (uint8_t *) dataOut, strlen(dataOut), UART_TRANSMIT_TIMEOUT);

  return COMPONENT_OK;
}

/**
 * @brief  Set FIFO to Continuous mode
 * @param  None
 * @retval COMPONENT_OK
 * @retval COMPONENT_ERROR
 */
static DrvStatusTypeDef LSM6DSL_FIFO_Set_Continuous_Mode(void)
{
  snprintf(dataOut, MAX_BUF_SIZE, "\r\nLSM6DSL starts to store the data into FIFO...\r\n\r\n");
  HAL_UART_Transmit(&UartHandle, (uint8_t *) dataOut, strlen(dataOut), UART_TRANSMIT_TIMEOUT);

  HAL_Delay(1000);

  /* Set FIFO mode to Continuous */
  if (BSP_GYRO_FIFO_Set_Mode_Ext(LSM6DSL_G_0_handle, LSM6DSL_ACC_GYRO_FIFO_MODE_DYN_STREAM_2) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief  Read all unread FIFO data in cycle
 * @param  None
 * @retval COMPONENT_OK
 * @retval COMPONENT_ERROR
 */
static DrvStatusTypeDef LSM6DSL_Read_All_FIFO_Data(SensorAxes_t *gyro, SensorAxes_t *acc, SensorAxes_t *mag)
{
  uint16_t gSamplesToRead = 0;
  uint16_t aSamplesToRead = 0;

  int i = 0;

  /* Get num of unread FIFO samples before reading data */
  if (BSP_GYRO_FIFO_Get_Num_Of_Samples_Ext(LSM6DSL_G_0_handle, &gSamplesToRead) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  /* Get num of unread FIFO samples before reading data */
  if (BSP_ACCELERO_FIFO_Get_Num_Of_Samples_Ext(LSM6DSL_X_0_handle, &aSamplesToRead) == COMPONENT_ERROR)
  {
    return COMPONENT_ERROR;
  }

  /* 'samplesToRead' actually contains number of words in FIFO but each FIFO sample (data set) consists of 3 words
   so the 'samplesToRead' has to be divided by 3 */
  gSamplesToRead /= 3;

//  printf("\r\n\r\n%d samples in FIFO.\r\n\r\nStarted downloading data from FIFO...\r\n\r\n", gSamplesToRead);

//  printf("[DATA ##]     GYR_X     GYR_Y     GYR_Z     ACC_X     ACC_Y     ACC_Z\r\n");

  for (i = 0; i < gSamplesToRead; i++)
  {
    if (LSM6DSL_Read_Single_FIFO_Pattern_Cycle(i, gyro, acc, mag) == COMPONENT_ERROR)
    {
      return COMPONENT_ERROR;
    }
  }

  if (gSamplesToRead > SAMPLE_LIST_MAX)
  {
    printf("\r\nSample list limited to: %d\r\n\r\n", SAMPLE_LIST_MAX);
  }

  return COMPONENT_OK;
}

#define INT32_T_TO_FLOAT(n)                    \
    (float)( (n ) )

/**
 * @brief  Read single FIFO pattern cycle
 * @param  sampleIndex Current sample index.
 * @retval COMPONENT_OK
 * @retval COMPONENT_ERROR
 */
static DrvStatusTypeDef LSM6DSL_Read_Single_FIFO_Pattern_Cycle(uint16_t sampleIndex,  SensorAxes_t *gyro, SensorAxes_t *acc, SensorAxes_t *mag )
{
  uint16_t pattern = 0;
  int32_t acceleration = 0;
  int32_t angular_velocity = 0;
  float gx,gy,gz,ax,ay,az,mx,my,mz;
  int i = 0;

  /* Read one whole FIFO pattern cycle. Pattern: Gx, Gy, Gz */
  for (i = 0; i <= 2; i++)
  {
    /* Read FIFO pattern number */
    if (BSP_GYRO_FIFO_Get_Pattern_Ext(LSM6DSL_G_0_handle, &pattern) == COMPONENT_ERROR)
    {
      return COMPONENT_ERROR;
    }

    /* Read single FIFO data (angular velocity in one axis) */
    if (BSP_GYRO_FIFO_Get_Axis_Ext(LSM6DSL_G_0_handle, &angular_velocity) == COMPONENT_ERROR)
    {
      return COMPONENT_ERROR;
    }

    /* Decide which axis has been read from FIFO based on pattern number */
    switch (pattern)
    {
    case PATTERN_GYR_X_AXIS:
      gyro->AXIS_X = angular_velocity;
      break;

    case PATTERN_GYR_Y_AXIS:
    	gyro->AXIS_Y = angular_velocity;
      break;

    case PATTERN_GYR_Z_AXIS:
    	gyro->AXIS_Z = angular_velocity;
      break;

    default:
      return COMPONENT_ERROR;
    }
  }

  /* Read one whole FIFO pattern cycle. Pattern: XLx, XLy, XLz */
  for (i = 0; i <= 2; i++)
  {
    /* Read FIFO pattern number */
    if (BSP_ACCELERO_FIFO_Get_Pattern_Ext(LSM6DSL_X_0_handle, &pattern) == COMPONENT_ERROR)
    {
      return COMPONENT_ERROR;
    }

    /* Read single FIFO data (acceleration in one axis) */
    if (BSP_ACCELERO_FIFO_Get_Axis_Ext(LSM6DSL_X_0_handle, &acceleration) == COMPONENT_ERROR)
    {
      return COMPONENT_ERROR;
    }

    /* Decide which axis has been read from FIFO based on pattern number */
    switch (pattern)
    {
    case PATTERN_ACC_X_AXIS:
      acc->AXIS_X = acceleration;
      break;

    case PATTERN_ACC_Y_AXIS:
      acc->AXIS_Y = acceleration;
      break;

    case PATTERN_ACC_Z_AXIS:
      acc->AXIS_Z = acceleration;
      break;

    default:
      return COMPONENT_ERROR;
    }
  }

  LSM303AGR_Read_Magnometer(mag);
  mx = mag->AXIS_X;
  my = mag->AXIS_Y;
  mz = mag->AXIS_Z;

  gx = gyro->AXIS_X;
  gy = gyro->AXIS_Y;
  gz = gyro->AXIS_Z;

  ax = acc->AXIS_X;
  ay = acc->AXIS_Y;
  az = acc->AXIS_Z;


  MadgwickUpdate(gx,gy,gz,ax,ay,az,mx,my,mz);

float yaw = MadgwickGetYaw();
float pitch = MadgwickGetPitch();
float roll = MadgwickGetRoll();

  if (sampleIndex < SAMPLE_LIST_MAX)
  {
//    printf("%d %d %d %d %d %d %d %d %d %d %d %d\r\n",
////    		"%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\r\n",
//			roll, pitch, yaw,
//			gyr_x, gyr_y, gyr_z,
//			acc_x, acc_y, acc_z,
//			mag_x, mag_y, mag_z
//    	);
	printf(
			"%3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f %3.4f\r\n",
			roll, pitch, yaw, ax, ay, az, gx, gy, gz, mx, my, mz);
  }

  return COMPONENT_OK;
}

/**
 * @brief  Handles the MAGNETO axes data getting/sending
 * @param  Msg the MAGNETO part of the stream
 * @retval None
 */
static void LSM303AGR_Read_Magnometer(SensorAxes_t *MAG_Value)
{
//  SensorAxes_t MAG_Value;
//  int32_t data[3];
  uint8_t status = 0;
  uint8_t drdy = 0;

  if (BSP_MAGNETO_IsInitialized(LSM303AGR_M_0_handle, &status) == COMPONENT_OK && status == 1)
  {
    BSP_MAGNETO_Get_DRDY_Status(LSM303AGR_M_0_handle, &drdy);

    if (drdy != 0)
    {

      BSP_MAGNETO_Get_Axes(LSM303AGR_M_0_handle, MAG_Value);

//      data[0] = MAG_Value->AXIS_X;
//      data[1] = MAG_Value->AXIS_Y;
//      data[2] = MAG_Value->AXIS_Z;

//      snprintf(dataOut, MAX_BUF_SIZE, "MAG_X: %d, MAG_Y: %d, MAG_Z: %d\r\n", (int) data[0], (int) data[1], (int) data[2]);
//      HAL_UART_Transmit(&UartHandle, (uint8_t*) dataOut, strlen(dataOut), 5000);

    }
  }
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
    if (BSP_PB_GetState(BUTTON_KEY) == GPIO_PIN_RESET)
    {
      switch (demoFifoStatus)
      {
      /* If FIFO is in Bypass mode switch to Continuous mode */
      case STATUS_IDLE:
        demoFifoStatus = DEMO_STATUS_SET_FIFO_CONTINUOUS_MODE;
        break;
        /* If FIFO is in Continuous mode switch to Bypass mode */
      case DEMO_STATUS_FIFO_RUN:
        demoFifoStatus = DEMO_STATUS_SET_FIFO_BYPASS_MODE;
        break;
        /* Otherwise do nothing */
      case DEMO_STATUS_SET_FIFO_CONTINUOUS_MODE:
      case DEMO_STATUS_FIFO_DOWNLOAD:
      case DEMO_STATUS_SET_FIFO_BYPASS_MODE:
        break;
      default:
        break;
      }
    }
  }

  /* FIFO full (available only for LSM6DSL sensor) */
  else if (GPIO_Pin == LSM6DSL_INT1_O_PIN)
  {
    memsIntDetected = 1;
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
