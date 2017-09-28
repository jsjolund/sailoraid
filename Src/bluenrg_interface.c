/**
 ******************************************************************************
 * @file    bluenrg_interface.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    04-July-2014
 * @brief   This file provides code for the BlueNRG Expansion Board driver
 *          based on STM32Cube HAL for STM32 Nucleo boards.
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
#include "bluenrg_interface.h"
#define BDADDR_SIZE 6
uint8_t bnrg_expansion_board = IDB05A1; /* at startup, suppose the X-NUCLEO-IDB04A1 is used */
void InitBluetoothGattServer(void)
{
  const char *name = "BlueNRG";
  uint8_t SERVER_BDADDR[] = { 0x12, 0x34, 0x00, 0xE1, 0x80, 0x03 };
  uint8_t bdaddr[BDADDR_SIZE];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

  uint8_t hwVersion;
  uint16_t fwVersion;

  int ret;

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

  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
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

  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED, OOB_AUTH_DATA_ABSENT,
  NULL, 7, 16, USE_FIXED_PIN_FOR_PAIRING, 123456, BONDING);
  if (ret == BLE_STATUS_SUCCESS)
  {
    printf("BLE Stack Initialized.\n");
  }

  printf("SERVER: BLE Stack Initialized\n");

  ret = Add_Orientation_Service();

  if (ret == BLE_STATUS_SUCCESS)
    printf("Acc service added successfully.\n");
  else
    printf("Error while adding Acc service.\n");

  ret = Add_GPS_Service();

  if (ret == BLE_STATUS_SUCCESS)
    printf("GPS service added successfully.\n");
  else
    printf("Error while adding GPS service.\n");

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
  printf("Bluetooth OK.\n");
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
