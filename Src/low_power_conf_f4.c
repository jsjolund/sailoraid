/**
******************************************************************************
* @file    low_power_conf_f4.c
* @author  AAS / CL
* @version V1.0.0
* @date    04-July-2014
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
#include "low_power_conf.h"
#include "main.h"

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Configure the system for power optimization
  *
  * @note	This API configures the system to be ready for low power mode
  * 		- Unused GPIO in Analog mode
  * 		- Fast Wakeup
  * 		- Verefint disabled in low power mode
  * 		- FLASH in low power mode
  *
  * @param  None
  * @retval None
  */
void SystemPower_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /**
  * Disable FLASH in SLEEP Mode
  */
  HAL_PWREx_EnableFlashPowerDown();

  /*
  * Disable all GPIOs clock in SLEEP mode
  */
#if (JTAG_SUPPORTED == 0)
  __GPIOA_CLK_SLEEP_DISABLE();
#endif
  __GPIOB_CLK_SLEEP_DISABLE();
  __GPIOC_CLK_SLEEP_DISABLE();
  __GPIOD_CLK_SLEEP_DISABLE();
  __GPIOE_CLK_SLEEP_DISABLE();
  __GPIOH_CLK_SLEEP_DISABLE();

  /*
  * Enable GPIOs clock
  */
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();

  /*
  * Configure all GPIO port pins in Analog mode with no pull
  */
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

#if (JTAG_SUPPORTED == 1)
  GPIO_InitStructure.Pin = (GPIO_PIN_All ^ (GPIO_PIN_13 | GPIO_PIN_14));
#endif
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*
  * Disable GPIOs clock
  */
  __GPIOA_CLK_DISABLE();
  __GPIOB_CLK_DISABLE();
  __GPIOC_CLK_DISABLE();
  __GPIOD_CLK_DISABLE();
  __GPIOE_CLK_DISABLE();
  __GPIOH_CLK_DISABLE();

  return;
}

/**
 * @brief  Call procedure for configuring the system clock after wake-up
 *         from STOP
 * @param  None
 * @retval None
 */
void LPM_ExitStopMode(void)
{
#if (HCLK_84MHZ == 1)
  __HAL_RCC_PLL_ENABLE();                                     /**< Enable PLL */
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET);        /**< Wait till PLL is ready */
  __HAL_APP_RCC_SET_SYSCLK_SRC(RCC_CFGR_SW_PLL);              /**< Select PLL as system clock source */
  while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL);  /**< Wait till PLL is used as system clock source */
#endif

  return;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
