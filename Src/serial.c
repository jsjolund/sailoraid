/*
 * serial.c
 *
 *  Created on: Jan 28, 2017
 *      Author: jsjolund
 */

#include "serial.h"

typedef struct SerialHandle
{
  UART_HandleTypeDef *huart;

  uint8_t txFifo[TX_BUFFER_MAX];
  volatile int txBack, txFront;
  volatile uint8_t txCplt; // = 1 at start

  volatile int rxIndex;
  uint8_t rxString[RX_BUFFER_MAX];
  uint8_t rxBuffer;

} SerialHandle;

static SerialHandle usbHandle;
static SerialHandle gpsHandle;
static NmeaInfo gpsInfo;
static BOOL gpsDebug = 0;
extern SensorState_t sensor;

void GPSdebugPrint(BOOL onoff)
{
  gpsDebug = onoff;
}

static int QueuePut(SerialHandle *h, uint8_t input)
{
  if (h->txBack == ((h->txFront - 1 + TX_BUFFER_MAX) % TX_BUFFER_MAX))
    return 0;
  h->txFifo[h->txBack] = input;
  h->txBack = (h->txBack + 1) % TX_BUFFER_MAX;
  return 1;
}

static int QueueGetSize(SerialHandle *h)
{
  return (h->txFront <= h->txBack) ? h->txBack - h->txFront : h->txBack - h->txFront + TX_BUFFER_MAX;
}

static void TransmitFromFIFO(SerialHandle *h)
{
  HAL_UART_IRQHandler(h->huart);
  // Sends characters from the FIFO to UART through DMA
  HAL_UART_StateTypeDef uartState = HAL_UART_GetState(h->huart);
  if (uartState == HAL_UART_STATE_READY || uartState == HAL_UART_STATE_BUSY_RX)
  {
    if (h->txCplt)
    {
      h->txCplt = 0;
      int queueSize = QueueGetSize(h);
      if (queueSize > 0)
      {
        int sendSize, newFront;
        if (h->txFront < h->txBack)
        {
          // Dequeue all remaining
          sendSize = h->txBack - h->txFront;
          newFront = h->txBack;
        }
        else
        {
          // Dequeue until end of array. Remaining will dequeue on next invocation.
          sendSize = TX_BUFFER_MAX - h->txFront;
          newFront = 0;
        }
        while (HAL_UART_Transmit_DMA(h->huart, &h->txFifo[h->txFront], sendSize) != HAL_OK)
          ;
        h->txFront = newFront;
      }
      else
      {
        h->txCplt = 1;
      }
    }
  }
}

void SerialInit(UART_HandleTypeDef *usbHuartHandle, UART_HandleTypeDef *gpsHuartHandle)
{
  usbHandle.huart = usbHuartHandle;
  usbHandle.txCplt = 1;
  gpsHandle.huart = gpsHuartHandle;
  gpsHandle.txCplt = 1;

  nmeaInfoClear(&gpsInfo);

  // Initiate automatic receive through DMA one character at a time
  HAL_UART_Receive_DMA(usbHandle.huart, &usbHandle.rxBuffer, sizeof(usbHandle.rxBuffer));
  HAL_UART_Receive_DMA(gpsHandle.huart, &gpsHandle.rxBuffer, sizeof(gpsHandle.rxBuffer));
}

static void SerialTransmit(SerialHandle *h, char *ptr, int len)
{
  int i;
  for (i = 0; i < len; i++)
  {
    QueuePut(h, ptr[i]);
  }
  TransmitFromFIFO(h);
}

void SerialUsbTransmit(char *ptr, int len)
{
  SerialTransmit(&usbHandle, ptr, len);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  SerialHandle *h;
  if (huart == gpsHandle.huart)
  {
    h = &gpsHandle;
    char buffer[30];
    int cx;
    cx = snprintf(buffer, 30, "ERROR CODE %d GPS UART\r\n", (int) huart->ErrorCode);
    SerialUsbTransmit(buffer, cx);
  }
  else
  {
    h = &usbHandle;
  }

  if (h->huart->ErrorCode == HAL_UART_ERROR_PE)
    __HAL_UART_CLEAR_PEFLAG(h->huart);
  if (h->huart->ErrorCode == HAL_UART_ERROR_FE)
    __HAL_UART_CLEAR_FEFLAG(h->huart);
  if (h->huart->ErrorCode == HAL_UART_ERROR_NE)
    __HAL_UART_CLEAR_NEFLAG(h->huart);

  // Clear the buffer since we got invalid data, then start receiving again
  h->rxIndex = 0;
  for (int i = 0; i < RX_BUFFER_MAX; i++)
    h->rxString[i] = 0;
  HAL_UART_Receive_DMA(h->huart, &h->rxBuffer, sizeof(h->rxBuffer));

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huartHandle)
{
  if (huartHandle == usbHandle.huart)
  {
    usbHandle.txCplt = 1;
    TransmitFromFIFO(&usbHandle);
  }
  if (huartHandle == gpsHandle.huart)
  {
    gpsHandle.txCplt = 1;
    TransmitFromFIFO(&gpsHandle);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huartHandle)
{
  SerialHandle *h;
  int i;
  char rxChar;
  if (huartHandle == usbHandle.huart)
  {
    if (usbHandle.rxBuffer == '\n')
      return;
    GPSecho(FALSE);
    IMUecho(FALSE);
    ENVecho(FALSE);
    RangeEcho(FALSE);
    MATLABecho(FALSE);
    ADCecho(FALSE);
    GPSdebugPrint(FALSE);
    h = &usbHandle;
  }
  else if (huartHandle == gpsHandle.huart)
  {
    h = &gpsHandle;
  }
  else
  {
    return;
  }
  rxChar = h->rxBuffer;
  if (rxChar == 127 || rxChar == 8)
  {
    // Backspace or delete
    if (h == &usbHandle && h->rxIndex > 0)
      SerialUsbTransmit("\b \b", 3);

    h->rxIndex = (h->rxIndex > 0) ? h->rxIndex - 1 : 0;
    h->rxString[h->rxIndex] = 0;
  }
  else if (rxChar == '\n')
  {
  }
  else if (rxChar == '\r')
  {
    // Carriage return
    char *row = (char *) &h->rxString;
    // Check if we should update GPS data or execute command
    if ((h == &gpsHandle) && (h->rxIndex < RX_BUFFER_MAX - 2))
    {
      h->rxString[h->rxIndex++] = '\r';
      h->rxString[h->rxIndex++] = '\n';
      if (gpsDebug)
        SerialUsbTransmit(row, strlen(row));
      GPSparse(row, strlen(row), &gpsInfo, &sensor.gps);
    }
    else if (h == &usbHandle)
    {
      SerialUsbTransmit("\r\n", 2);
      ShellExecute(row);
    }
    // Clear the receive buffer
    h->rxIndex = 0;
    for (i = 0; i < RX_BUFFER_MAX; i++)
      h->rxString[i] = '\0';
  }
  else
  {
    // Echo the character
    if (h == &usbHandle && h->rxIndex < RX_BUFFER_MAX - 1)
      SerialUsbTransmit(&rxChar, 1);
    // Append character and increment cursor
    h->rxString[h->rxIndex] = rxChar;
    if (h->rxIndex < RX_BUFFER_MAX - 1)
      h->rxIndex++;
  }
}
