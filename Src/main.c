/**
******************************************************************************
* @file    main.c 
* @author  CL
* @version V1.0.0
* @date    04-July-2014
* @brief   This sample code shows how to use the BlueNRG STM32 expansion board
*          to exchange data between two devices.
*          The communication is done using two STM32 Nucleo boards.
******************************************************************************
* @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#include "cube_hal.h"

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "osal.h"
#include "low_power_conf.h"
#include "sensor_service.h"
#include "debug.h"
#include "bluenrg_interface.h"
#include "stm32xx_hal_app_rtc.h"
#include "bluenrg_utils.h"

#include <stdio.h>

/** @defgroup X-CUBE-BLE1_Applications
 *  @brief Sample applications for X-NUCLEO-IDB04A1/X-NUCLEO-IDB05A1 STM32 expansion boards.
 *  @{
 */

/** @defgroup SensorDemo_DMA_LowPower
 *  @{
 */
 
/** @defgroup MAIN 
 * @{
 */

/** @defgroup MAIN_Private_Defines 
 * @{
 */
/* Private defines ------------------------------------------------------------*/
#define BDADDR_SIZE 6
#define APP_TIMER_VALUE	1000000 /**< Value in us */

/**
* @}
*/

/* Private macros -------------------------------------------------------------*/

/** @defgroup MAIN_Exported_Variables 
* @{
*/

/* RTC handler declaration */
static RTC_HandleTypeDef hrtc;
static uint32_t TaskExecutionRequested = 0;
static uint8_t Hci_Cmd_Lock = FALSE;
uint8_t appTimer_Id;

extern volatile uint8_t set_connectable;
extern volatile int connected;
extern AxesRaw_t axes_data;
extern volatile int button_event;
uint8_t bnrg_expansion_board = IDB04A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */

/**
* @}
*/

/** @defgroup MAIN_Private_Function_Prototypes 
* @{
*/
/* Private function prototypes -----------------------------------------------*/
void BNRG_Init(void);
void Background (void);
void User_Process(AxesRaw_t* p_axes);
static void Init_RTC(void);
/**
* @}
*/

/**
  * @brief  This function is empty to avoid starting the SysTick Timer
  * @param  None
  * @retval None
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return (HAL_OK);
}

/**
  * @brief This function provides accurate delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation, SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note ThiS function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @note NOT USED IN THIS EXAMPLE
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(__IO uint32_t Delay)
{
  return;
}

/**
 * @brief  Main function to show how to use the BlueNRG Bluetooth Low Energy
 *         expansion board to send data from a Nucleo board to a smartphone
 *         with the support BLE and the "BlueNRG" app freely available on both
 *         GooglePlay and iTunes.
 *         The URL to the iTunes for the "BlueNRG" app is
 *         http://itunes.apple.com/app/bluenrg/id705873549?uo=5
 *         The URL to the GooglePlay is
 *         https://play.google.com/store/apps/details?id=com.st.bluenrg
 *         The source code of the "BlueNRG" app, both for iOS and Android, is
 *         freely downloadable from the developer website at
 *         http://software.g-maps.it/
 *         The board will act as Server-Peripheral.
 *
 *         After connection has been established:
 *          - by pressing the USER button on the board, the cube showed by
 *            the app on the smartphone will rotate.
 *          
 *         The communication is done using a vendor specific profile.
 * @param  None
 * @retval None
 */
int main(void)
{
#if (JTAG_SUPPORTED == 1) 
  /*
  * Keep debugger enabled while in any low power mode
  */
#ifdef STM32L053xx
  __DBGMCU_CLK_ENABLE();
  HAL_DBG_LowPowerConfig(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif /* STM32L053xx */

#ifdef STM32L476xx
  HAL_DBGMCU_EnableDBGSleepMode();
  HAL_DBGMCU_EnableDBGStopMode();
  HAL_DBGMCU_EnableDBGStandbyMode();
#endif /* STM32L476xx */

#ifdef STM32F401xE 
  HAL_EnableDBGSleepMode();
  HAL_EnableDBGStopMode();
  HAL_EnableDBGStandbyMode();
#endif /* STM32F401xE */
#endif /* (JTAG_SUPPORTED == 1) */
  
  /* STM32Cube HAL library initialization:
   *  - Configure the Flash prefetch, Flash preread and Buffer caches
   *  - Systick timer is configured by default as source of time base, but user 
   *    can eventually implement his proper time base source (a general purpose 
   *    timer for example or other time source), keeping in mind that Time base 
   *    duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
   *    handled in milliseconds basis.
   *  - Low Level Initialization
   */
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
  /* Configure the system power */
  SystemPower_Config();  
  
#if NEW_SERVICES
  /* Configure LED2 */
  BSP_LED_Init(LED2); 
#endif
  
  /* Configure the User Button in EXTI Mode */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
    
  /* Configure the RTC */
  Init_RTC();
  TIMER_Init(&hrtc);
  TIMER_Create(eTimerModuleID_BlueNRG_Profile_App, &(appTimer_Id), eTimerMode_Repeated, 0);
     
  /* Set request for Low Power STOP mode */
  LPM_Mode_Request(eLPM_MAIN_LOOP_PROCESSES, eLPM_Mode_LP_Stop);
  
  /* Initialize the BlueNRG SPI driver */
  BNRG_SPI_Init();
  
  /**
   * Initialize the BlueNRG
   * (init the HCI, set the tx power, ...)
   */
  BNRG_Init();
    
  while(1)
  {    
    Background();    
  }
}

/**
 * @brief  Initialize the BlueNRG (initialize the HCI, set the tx power, ...)
 *
 * @param  None
 * @retval None
 */
void BNRG_Init(void)
{ 
  const char *name = "BlueNRG";
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  uint8_t SERVER_BDADDR[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x03};
  uint8_t bdaddr[BDADDR_SIZE];
  
  uint8_t  hwVersion;
  uint16_t fwVersion;
  
  int ret;
  
  /* Initialize the BlueNRG HCI */
  HCI_Init();
  
  /* Reset BlueNRG hardware */
  BlueNRG_RST();
  
  Hal_Init_Event_Request();
  
  /* get the BlueNRG HW and FW versions */
  getBlueNRGVersion(&hwVersion, &fwVersion);

  /* 
   * Reset BlueNRG again otherwise we won't
   * be able to change its MAC address.
   * aci_hal_write_config_data() must be the first
   * command after reset otherwise it will fail.
   */
  BlueNRG_RST();
  
  PRINTF("HWver %d, FWver %d\n", hwVersion, fwVersion);
  
  if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
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
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);
  if(ret){
    PRINTF("Setting BD_ADDR failed.\n");
  }
  
  ret = aci_gatt_init();    
  if(ret){
    PRINTF("GATT_Init failed.\n");
  }

  if (bnrg_expansion_board == IDB05A1) {
    ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }
  else {
    ret = aci_gap_init_IDB04A1(GAP_PERIPHERAL_ROLE_IDB04A1, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }

  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GAP_Init failed.\n");
  }

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   strlen(name), (uint8_t *)name);

  if(ret){
    PRINTF("aci_gatt_update_char_value failed.\n");            
    while(1);
  }
  
  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL,
                                     7,
                                     16,
                                     USE_FIXED_PIN_FOR_PAIRING,
                                     123456,
                                     BONDING);
  if (ret == BLE_STATUS_SUCCESS) {
    PRINTF("BLE Stack Initialized.\n");
  }
  
  PRINTF("SERVER: BLE Stack Initialized\n");
  
  ret = Add_Acc_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Acc service added successfully.\n");
  else
    PRINTF("Error while adding Acc service.\n");
  
  ret = Add_Environmental_Sensor_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Environmental Sensor service added successfully.\n");
  else
    PRINTF("Error while adding Environmental Sensor service.\n");

#if NEW_SERVICES
  /* Instantiate Timer Service with two characteristics:
   * - seconds characteristic (Readable only)
   * - minutes characteristics (Readable and Notifiable )
   */
  ret = Add_Time_Service(); 
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Time service added successfully.\n");
  else
    PRINTF("Error while adding Time service.\n");  
  
  /* Instantiate LED Button Service with one characteristic:
   * - LED characteristic (Readable and Writable)
   */  
  ret = Add_LED_Service();

  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("LED service added successfully.\n");
  else
    PRINTF("Error while adding LED service.\n");  
#endif

  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,4);
  
}

/**
  * @brief  Request action to the scheduler in background
  *
  * @note
  * @retval None
  */
void TaskExecutionRequest(eMAIN_Background_Task_Id_t eMAIN_Background_Task_Id)
{
  __disable_irq();
  TaskExecutionRequested |= (1 << eMAIN_Background_Task_Id);
  __enable_irq();
  
  return;
}

/**
  * @brief  Notify the action in background has been completed
  *
  * @note
  * @param  eMAIN_Background_Task_Id: Id of the request
  * @retval None
  */
void TaskExecuted(eMAIN_Background_Task_Id_t eMAIN_Background_Task_Id)
{
  __disable_irq();
  TaskExecutionRequested &= ~(1 << eMAIN_Background_Task_Id);
  __enable_irq();
  
  return;
}

/**
  * @brief  HCI process notification request
  *
  * @param  None
  * @retval None
  */
void HCI_Process_Notification_Request(void)
{
  TaskExecutionRequest(eMAIN_HCI_Process_Request_Id);
}

/**
  * @brief  User process notification request
  *
  * @param  None
  * @retval None
  */
void User_Process_Notification_Request(void)
{
  TaskExecutionRequest(eMAIN_User_Process_Request_Id);
}

/**
  * @brief  Background task
  *
  * @note
  * @param  None
  * @retval None
  */
void Background (void)
{   
  if(TaskExecutionRequested & (1<< eMAIN_User_Process_Request_Id))
  {
    if(Hci_Cmd_Lock == FALSE)
    {   
      TaskExecuted(eMAIN_User_Process_Request_Id);
      User_Process(&axes_data);
    }
  }
  
  if(TaskExecutionRequested & (1<< eMAIN_HCI_Process_Request_Id))
  {
    if(Hci_Cmd_Lock == FALSE)
    {
      TaskExecuted(eMAIN_HCI_Process_Request_Id);
      HCI_Process();
    }
  }
  
  /**
  * Power management
  */
#if LOW_POWER_MODE
  __disable_irq();
  if((TaskExecutionRequested == 0) || (((TaskExecutionRequested & EVENT_NOT_REQUIRING_SENDING_HCI_COMMAND) == 0) && (Hci_Cmd_Lock == TRUE)))
  {
    LPM_Enter_Mode();
  }
  __enable_irq();
#endif /* LOW_POWER_MODE */
  
  return;  
}

/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send the updated acceleration data to the remote client.
 * @param  AxesRaw_t* p_axes
 * @retval None
 */
void User_Process(AxesRaw_t* p_axes)
{
  if(set_connectable){
    /* stop the application timer if already started */
    TIMER_Stop(appTimer_Id);
    
    /* Establish connection with remote device */
    setConnectable();
    set_connectable = FALSE;
    
    /* start the application timer */
    TIMER_Start(appTimer_Id, (uint16_t)(APP_TIMER_VALUE/TIMERSERVER_TICK_VALUE));
    
    PRINTF("Connection set\n");
  }

  /* Check if the user has pushed the button */
  if(button_event)
  {
    button_event = 0;
    
    //BSP_LED_Toggle(LED2); //used for debugging (BSP_LED_Init() above must be also enabled)
    
    if(connected)
    {
      /* Update acceleration data */
      p_axes->AXIS_X += 100;
      p_axes->AXIS_Y += 100;
      p_axes->AXIS_Z += 100;
      //PRINTF("ACC: X=%6d Y=%6d Z=%6d\r\n", p_axes->AXIS_X, p_axes->AXIS_Y, p_axes->AXIS_Z);
      Acc_Update(p_axes);
    }
    LPM_Mode_Request(eLPM_MAIN_LOOP_PROCESSES, eLPM_Mode_LP_Stop);
  }

}

/**
  * @brief  Timer notification
  *
  * @param  Timer Process ID
  * @param  Timer ID
  * @param  Timer Callback
  * @retval None
  */
void TIMER_Notification(eTimerProcessID_t eTimerProcessID, uint8_t ubTimerID, pf_TIMER_TimerCallBack_t pfTimerCallBack)
{
  switch (eTimerProcessID)
  {
  case eTimerModuleID_BlueNRG_Profile_App:
    /**
    * The code shall be executed in the background as aci command may be sent
    * The background is the only place where the application can make sure a new
    * aci command is not sent if there is a pending one
    */
    TaskExecutionRequest(eMAIN_User_Process_Request_Id);
    break;  
  
  case eTimerModuleID_BlueNRG_HCI:
    /**
    * The code is very short and there is no aci command to be send so it is
    * better to call it in the interrupt handler
    */
    pfTimerCallBack();
    break;
    
  case eTimerModuleID_Interrupt:
    /*
    * This is expected to be called in the Timer Interrupt Handler to limit 
    * latency. Calling the callback in the background task may impact power
    * performance
    */
    pfTimerCallBack();
    break;

  default:
    if (pfTimerCallBack != 0)
    {
      pfTimerCallBack();
    }
    break;
  }
}

/**
 * @brief  Wait For HCI Response
 *
 * @param  None
 * @retval None
 */
void HCI_Wait_For_Response(void)
{
  Background();
  
  return;
}

/**
 * @brief  Lock/Unlock the HCI Command
 *
 * @param  Hci Command Status
 * @retval None
 */
void HCI_Cmd_Status(HCI_CMD_STATUS_t Hci_Cmd_Status)
{
  if(Hci_Cmd_Status == BUSY)
  {
    Hci_Cmd_Lock = TRUE;
  }
  else
  {
    Hci_Cmd_Lock = FALSE;
  }
  return;
}

/**
  * @brief  Initialize RTC block
  *
  * @note
  * @param  None
  * @retval None
  */
static void Init_RTC(void)
{
  
  /* Initialize the HW - 37Khz LSI being used*/
  /* Enable the LSI clock */
  __HAL_RCC_LSI_ENABLE();
  
  /* Enable power module clock */
  __PWR_CLK_ENABLE();
  
  /* Enable acces to the RTC registers */
  HAL_PWR_EnableBkUpAccess();
  
  /**
   *  Write twice the value to flush the APB-AHB bridge
   *  This bit shall be written in the register before writing the next one
   */
  HAL_PWR_EnableBkUpAccess();
  
  /* Select LSI as RTC Input */
  __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);
  
  /* Enable RTC */
  __HAL_RCC_RTC_ENABLE();
  
  hrtc.Instance = RTC;                  /**< Define instance */
  hrtc.Lock = HAL_UNLOCKED;             /**< Initialize lock */
  hrtc.State = HAL_RTC_STATE_READY;     /**< Initialize state */
  
  /**
  * Bypass the shadow register
  */
  HAL_RTCEx_EnableBypassShadow(&hrtc);
  
  /**
  * Set the Asynchronous prescaler
  */
  hrtc.Init.AsynchPrediv = RTC_ASYNCH_PRESCALER;
  hrtc.Init.SynchPrediv = RTC_SYNCH_PRESCALER;
  HAL_RTC_Init(&hrtc);
  
  /* Disable Write Protection */
  __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc) ;
  
  HAL_APP_RTC_Set_Wucksel(&hrtc, WUCKSEL_DIVIDER);  /**< Tick timer is 55us */
  
  /* Wait for LSI to be stable */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == 0);
  
  return;
}

/**
 * When the critical section is used #define TIMER_USE_PRIMASK_AS_CRITICAL_SECTION in stm32xx_timerserver.h
 * the BNRG_Timer_Start_Allowed() may be called here
 * otherwise it shall be call from the background as BNRG_Request_Timer_Start() is called from DMA interrupt
 * and may result in nested call of the timer interface (which is not supported
 */
void BNRG_Request_Timer_Start(void)
{
  BNRG_Timer_Start_Allowed();

  return;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
