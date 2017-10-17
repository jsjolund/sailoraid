/**
 ******************************************************************************
 * @file    sensor_service.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    04-July-2014
 * @brief   Add a sample service using a vendor specific profile.
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
#include "sensor_service.h"

/** @addtogroup X-CUBE-BLE1_Applications
 *  @{
 */

/** @addtogroup SensorDemo
 *  @{
 */

/** @defgroup SENSOR_SERVICE
 * @{
 */

/** @defgroup SENSOR_SERVICE_Private_Variables
 * @{
 */
/* Private variables ---------------------------------------------------------*/
volatile int connected = FALSE;
volatile uint8_t set_connectable = 1;
volatile uint16_t connection_handle = 0;
volatile uint8_t notification_enabled = FALSE;
extern SensorState_t sensor;
uint16_t orientServHandle, freeFallCharHandle, orientCharHandle;
uint16_t gpsServHandle, gpsCharHandle;
uint16_t rangeServHandle, rangeCharHandle;
uint16_t envSensServHandle, tempCharHandle, pressCharHandle, humidityCharHandle;

/**
 * @}
 */

/** @defgroup SENSOR_SERVICE_Private_Macros
 * @{
 */
/* Private macros ------------------------------------------------------------*/
#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

#define COPY_ORIENT_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_FREE_FALL_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_ORIENT_UUID(uuid_struct)          COPY_UUID_128(uuid_struct,0x34,0x0a,0x1b,0x80, 0xcf,0x4b, 0x11,0xe1, 0xac,0x36, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define COPY_GPS_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0xab,0xcd,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_GPS_UUID(uuid_struct)          COPY_UUID_128(uuid_struct,0xaa,0xbb,0x1b,0x80, 0xcf,0x4b, 0x11,0xe1, 0xac,0x36, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define COPY_ENV_SENS_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x42,0x82,0x1a,0x40, 0xe4,0x77, 0x11,0xe2, 0x82,0xd0, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_TEMP_CHAR_UUID(uuid_struct)         COPY_UUID_128(uuid_struct,0xa3,0x2e,0x55,0x20, 0xe4,0x77, 0x11,0xe2, 0xa9,0xe3, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_PRESS_CHAR_UUID(uuid_struct)        COPY_UUID_128(uuid_struct,0xcd,0x20,0xc4,0x80, 0xe4,0x8b, 0x11,0xe2, 0x84,0x0b, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_HUMIDITY_CHAR_UUID(uuid_struct)     COPY_UUID_128(uuid_struct,0x01,0xc5,0x0b,0x60, 0xe4,0x8c, 0x11,0xe2, 0xa0,0x73, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define COPY_RANGE_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x23,0xcd,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_RANGE_UUID(uuid_struct)          COPY_UUID_128(uuid_struct,0x23,0xbb,0x1b,0x80, 0xcf,0x4b, 0x11,0xe1, 0xac,0x36, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    (((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

#define STORE_LE_32(buf, val)    (((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) , \
                                   ((buf)[2] =  (uint8_t) (val>>16) ) , \
                                   ((buf)[3] =  (uint8_t) (val>>24) ) )
#define GPS_SERVICE_DATA_LEN 20
#define ORIENT_SERVICE_DATA_LEN 12

/**
 * @}
 */

/** @defgroup SENSOR_SERVICE_Exported_Functions 
 * @{
 */
/**
 * @brief  Add an orientation service using a vendor specific profile.
 *
 * @param  None
 * @retval tBleStatus Status
 */
tBleStatus Add_Orientation_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  COPY_ORIENT_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 7, &orientServHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  COPY_FREE_FALL_UUID(uuid);
  ret = aci_gatt_add_char(orientServHandle, UUID_TYPE_128, uuid, 1,
  CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0, 16, 0, &freeFallCharHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  COPY_ORIENT_UUID(uuid);
  ret = aci_gatt_add_char(orientServHandle, UUID_TYPE_128, uuid, ORIENT_SERVICE_DATA_LEN,
  CHAR_PROP_NOTIFY | CHAR_PROP_READ,
  ATTR_PERMISSION_NONE,
  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 16, 0, &orientCharHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  printf("Service ORIENT added. Handle 0x%04X, Free fall Charac handle: 0x%04X, Ori Charac handle: 0x%04X\n", orientServHandle, freeFallCharHandle,
      orientCharHandle);
  return BLE_STATUS_SUCCESS;

  fail: printf("Error while adding ORIENT service.\n");
  return BLE_STATUS_ERROR;

}

/**
 * @brief  Update orientation characteristic value.
 *
 * @param  Structure containing Euler angles in degrees
 * @retval Status
 */
tBleStatus Orientation_Update(float x, float y, float z)
{
  i32_t* xi = (i32_t*) &x;
  i32_t* yi = (i32_t*) &y;
  i32_t* zi = (i32_t*) &z;

  tBleStatus ret;
  uint8_t buff[12];

  STORE_LE_32(buff, *xi);
  STORE_LE_32(buff + 4, *yi);
  STORE_LE_32(buff + 8, *zi);

  ret = aci_gatt_update_char_value(orientServHandle, orientCharHandle, 0, ORIENT_SERVICE_DATA_LEN, buff);

  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error 0x%02x while updating ORIENT characteristic.\n", ret);
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

tBleStatus Add_GPS_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  COPY_GPS_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 7, &gpsServHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  COPY_GPS_UUID(uuid);
  ret = aci_gatt_add_char(gpsServHandle, UUID_TYPE_128, uuid, GPS_SERVICE_DATA_LEN,
  CHAR_PROP_NOTIFY | CHAR_PROP_READ,
  ATTR_PERMISSION_NONE,
  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 16, 0, &gpsCharHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  printf("Service GPS added. Handle 0x%04X, GPS Charac handle: 0x%04X\n", gpsServHandle, gpsCharHandle);
  return BLE_STATUS_SUCCESS;

  fail: printf("Error while adding GPS service.\n");
  return BLE_STATUS_ERROR;

}

tBleStatus GPS_Update(float lon, float lat, float elv, float spd, float dir)
{
  i32_t* loni = (i32_t*) &lon;
  i32_t* lati = (i32_t*) &lat;
  i32_t* elvi = (i32_t*) &elv;
  i32_t* spdi = (i32_t*) &spd;
  i32_t* diri = (i32_t*) &dir;

  tBleStatus ret;
  uint8_t buff[20];

  STORE_LE_32(buff, *loni);
  STORE_LE_32(buff + 4, *lati);
  STORE_LE_32(buff + 8, *elvi);
  STORE_LE_32(buff + 12, *spdi);
  STORE_LE_32(buff + 16, *diri);

  ret = aci_gatt_update_char_value(gpsServHandle, gpsCharHandle, 0, GPS_SERVICE_DATA_LEN, buff);

  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error 0x%02x while updating GPS characteristic.\n", ret);
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;
}

tBleStatus Add_Range_Service(void)
{
  tBleStatus ret;

  uint8_t uuid[16];

  COPY_RANGE_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 7, &rangeServHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  COPY_RANGE_UUID(uuid);
  ret = aci_gatt_add_char(rangeServHandle, UUID_TYPE_128, uuid, 4,
  CHAR_PROP_NOTIFY | CHAR_PROP_READ,
  ATTR_PERMISSION_NONE,
  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 16, 0, &rangeCharHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  printf("Service Range added. Handle 0x%04X, Range Charac handle: 0x%04X\n", rangeServHandle, rangeCharHandle);
  return BLE_STATUS_SUCCESS;

  fail: printf("Error while adding Range service.\n");
  return BLE_STATUS_ERROR;

}

tBleStatus Range_Update(float range)
{
  i32_t* v = (i32_t*) &range;
  tBleStatus ret;

  ret = aci_gatt_update_char_value(rangeServHandle, rangeCharHandle, 0, 4, (uint8_t*) v);

  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error 0x%02x while updating Range characteristic.\n", ret);
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;

}

/**
 * @brief  Add the Environmental Sensor service.
 *
 * @param  None
 * @retval Status
 */
tBleStatus Add_Environmental_Sensor_Service(void)
{
  tBleStatus ret;
  uint8_t uuid[16];

  COPY_ENV_SENS_SERVICE_UUID(uuid);
  ret = aci_gatt_add_serv(UUID_TYPE_128, uuid, PRIMARY_SERVICE, 10, &envSensServHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  /* Temperature Characteristic */
  COPY_TEMP_CHAR_UUID(uuid);
  ret = aci_gatt_add_char(envSensServHandle, UUID_TYPE_128, uuid, 4,
  CHAR_PROP_NOTIFY | CHAR_PROP_READ, ATTR_PERMISSION_NONE,
  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 16, 0, &tempCharHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  /* Pressure Characteristic */
  COPY_PRESS_CHAR_UUID(uuid);
  ret = aci_gatt_add_char(envSensServHandle, UUID_TYPE_128, uuid, 4,
  CHAR_PROP_NOTIFY | CHAR_PROP_READ, ATTR_PERMISSION_NONE,
  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 16, 0, &pressCharHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  /* Humidity Characteristic */
  COPY_HUMIDITY_CHAR_UUID(uuid);
  ret = aci_gatt_add_char(envSensServHandle, UUID_TYPE_128, uuid, 4,
  CHAR_PROP_NOTIFY | CHAR_PROP_READ, ATTR_PERMISSION_NONE,
  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 16, 0, &humidityCharHandle);
  if (ret != BLE_STATUS_SUCCESS)
    goto fail;

  printf("Service ENV_SENS added. Handle 0x%04X, TEMP Charac handle: 0x%04X, PRESS Charac handle: 0x%04X, HUMID Charac handle: 0x%04X\n", envSensServHandle,
      tempCharHandle, pressCharHandle, humidityCharHandle);
  return BLE_STATUS_SUCCESS;

  fail: printf("Error while adding ENV_SENS service.\n");
  return BLE_STATUS_ERROR;

}

/**
 * @brief  Update temperature characteristic value.
 * @param  Temperature in tenths of degree 
 * @retval Status
 */
tBleStatus Temp_Update(float temp)
{
  i32_t* v = (i32_t*) &temp;
  tBleStatus ret;

  ret = aci_gatt_update_char_value(envSensServHandle, tempCharHandle, 0, 4, (uint8_t*) v);

  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error 0x%02x while updating TEMP characteristic.\n", ret);
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;

}

/**
 * @brief  Update pressure characteristic value.
 * @param  int32_t Pressure in mbar 
 * @retval tBleStatus Status
 */
tBleStatus Press_Update(float press)
{
  i32_t* v = (i32_t*) &press;
  tBleStatus ret;

  ret = aci_gatt_update_char_value(envSensServHandle, pressCharHandle, 0, 4, (uint8_t*) v);

  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error 0x%02x while updating PRESS characteristic.\n", ret);
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;

}

/**
 * @brief  Update humidity characteristic value.
 * @param  uint16_thumidity RH (Relative Humidity) in tenths of %
 * @retval tBleStatus      Status
 */
tBleStatus Humidity_Update(float humidity)
{
  i32_t* v = (i32_t*) &humidity;
  tBleStatus ret;

  ret = aci_gatt_update_char_value(envSensServHandle, humidityCharHandle, 0, 4, (uint8_t*) v);

  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error 0x%02x while updating HUM characteristic.\n", ret);
    return BLE_STATUS_ERROR;
  }
  return BLE_STATUS_SUCCESS;

}

/**
 * @brief  Puts the device in connectable mode.
 *         If you want to specify a UUID list in the advertising data, those data can
 *         be specified as a parameter in aci_gap_set_discoverable().
 *         For manufacture data, aci_gap_update_adv_data must be called.
 * @param  None 
 * @retval None
 */
/* Ex.:
 *
 *  tBleStatus ret;    
 *  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G'};    
 *  const uint8_t serviceUUIDList[] = {AD_TYPE_16_BIT_SERV_UUID,0x34,0x12};    
 *  const uint8_t manuf_data[] = {4, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x05, 0x02, 0x01};
 *  
 *  ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
 *                                 8, local_name, 3, serviceUUIDList, 0, 0);    
 *  ret = aci_gap_update_adv_data(5, manuf_data);
 *
 */
void setConnectable(void)
{
  tBleStatus ret;

  const char local_name[] = { AD_TYPE_COMPLETE_LOCAL_NAME, 'S','a','i','l','o','r','A','i','d' };

  /* disable scan response */
  hci_le_set_scan_resp_data(0, NULL);
  printf("General Discoverable Mode.\n");

  ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE, sizeof(local_name), local_name, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error while setting discoverable mode (%d)\n", ret);
  }
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{
  connected = TRUE;
  connection_handle = handle;

  printf("Connected to device:");
  for (int i = 5; i > 0; i--)
  {
    printf("%02X-", addr[i]);
  }
  printf("%02X\n", addr[0]);
}

/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None 
 * @retval None
 */
void GAP_DisconnectionComplete_CB(void)
{
  connected = FALSE;
  printf("Disconnected\n");
  /* Make the device connectable again. */
  set_connectable = TRUE;
  notification_enabled = FALSE;
}

/**
 * @brief  Read request callback.
 * @param  uint16_t Handle of the attribute
 * @retval None
 */
void Read_Request_CB(uint16_t handle)
{

  if (handle == orientCharHandle + 1)
  {
    Orientation_Update(sensor.imu.roll, sensor.imu.pitch, sensor.imu.yaw);
  }
  else if (handle == gpsCharHandle + 1)
  {
    GPS_Update(sensor.gps.pos.longitude, sensor.gps.pos.latitude, sensor.gps.pos.elevation, sensor.gps.pos.speed, sensor.gps.pos.direction);
  }
  else if (handle == tempCharHandle + 1)
  {
    Temp_Update(sensor.env.temperature);
  }
  else if (handle == pressCharHandle + 1)
  {
    Press_Update(sensor.env.pressure);
  }
  else if (handle == humidityCharHandle + 1)
  {
    Humidity_Update(sensor.env.humidity);
  }

  //EXIT:
  if (connection_handle != 0)
    aci_gatt_allow_read(connection_handle);
}

/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = pckt;
  /* obtain event packet */
  hci_event_pckt *event_pckt = (hci_event_pckt*) hci_pckt->data;

  if (hci_pckt->type != HCI_EVENT_PKT)
    return;

  switch (event_pckt->evt)
  {

  case EVT_DISCONN_COMPLETE:
  {
    GAP_DisconnectionComplete_CB();
  }
    break;

  case EVT_LE_META_EVENT:
  {
    evt_le_meta_event *evt = (void *) event_pckt->data;

    switch (evt->subevent)
    {
    case EVT_LE_CONN_COMPLETE:
    {
      evt_le_connection_complete *cc = (void *) evt->data;
      GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
    }
      break;
    }
  }
    break;

  case EVT_VENDOR:
  {
    evt_blue_aci *blue_evt = (void*) event_pckt->data;
    switch (blue_evt->ecode)
    {
    case EVT_BLUE_GATT_READ_PERMIT_REQ:
    {
      evt_gatt_read_permit_req *pr = (void*) blue_evt->data;
      Read_Request_CB(pr->attr_handle);
    }
      break;
    }
  }
    break;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
