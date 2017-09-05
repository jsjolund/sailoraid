/**
  ******************************************************************************
  * @file    cube_hal_f4.c
  * @author  CL
  * @version V1.0.0
  * @date    03-November-2014
  * @brief   
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
#include "cube_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
/**
  * @brief  Configure the system clock
  *
  * @note   This API configures
  * 	      - The system clock source
  *           - The AHBCLK, APBCLK dividers
  *           - The flash latency
  *           - The PLL settings (when required)
  *
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /**
   * Enable clock on PWR block
   * This is used to setup registers when entering low power mode
   */
  __PWR_CLK_ENABLE();
    
  /**
   * Set voltage scaling range
   * The voltage scaling allows optimizing the power consumption when the device is 
   * clocked below the maximum system frequency, to update the voltage scaling value 
   * regarding system frequency refer to product datasheet.
   */
#if (HCLK_84MHZ == 1)
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  
  /**
   * Wait voltage scaling to be ready
   */
  while(__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY) == SET);

  /**
  *  Enable HSI oscillator and configure the PLL to reach the max system frequency 
  *  (84MHz) when using HSI oscillator as PLL clock source.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 210;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 15;    /**< As long as PLLQ is unused, it should be reduced to lowest speed */
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /**
   *  Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers.
   *  The SysTick 1 msec interrupt is required for the HAL process (Timeout management); by default
   *  the configuration is done using the HAL_Init() API, and when the system clock configuration
   *  is updated the SysTick configuration will be adjusted by the HAL_RCC_ClockConfig() API.
   */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
#else

  /**
  * Reset value is Range 2
  */
  
  /**
  *  Enable HSI oscillator and configure the system at 8MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /**
  *  Configure the HCLK, PCLK1 and PCLK2 clocks dividers  to get 8Mhz.
  *  The SysTick 1 msec interrupt is required for the HAL process (Timeout management); by default
  *  the configuration is done using the HAL_Init() API, and when the system clock configuration
  *  is updated the SysTick configuration will be adjusted by the HAL_RCC_ClockConfig() API.
  */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
  
#endif /* (HCLK_84MHZ == 1) */

  return;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
