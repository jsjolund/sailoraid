/*
 * serial.c
 *
 *  Created on: Sep 21, 2017
 *      Author: user
 */

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huartHandle) {
//  if (huartHandle == &huart1) {
//    uint8_t data = huartHandle->Instance->DR;
//
//  } else if (huartHandle == &huart2) {
//
//
//  }
//}
/*
 * serial.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#include "serial.h"
#include "main.h"

typedef struct SerialHandle
{
  volatile int rxIndex;
  volatile uint8_t rxString[RX_BUFFER_MAX];
  volatile uint8_t txString[TX_BUFFER_MAX];
  volatile int txIndex, txOutdex;
  uint8_t rxBuffer;
  UART_HandleTypeDef *huart;
  volatile uint8_t txCplt; // = 1 at start
} SerialHandle;

static SerialHandle usbHandle;
static SerialHandle gpsHandle;

int isGlyph(char c)
{
  return (c >= ' ' && c <= '~');
}

int UsbQueuePut(SerialHandle *h, uint8_t input)
{
  if (h->txIndex == ((h->txOutdex - 1 + TX_BUFFER_MAX) % TX_BUFFER_MAX))
    return 0;
  h->txString[h->txIndex] = input;
  h->txIndex = (h->txIndex + 1) % TX_BUFFER_MAX;
  return 1;
}

int UsbQueueGet(SerialHandle *h, uint8_t *output)
{
  if (h->txIndex == h->txOutdex)
    return 0;
  *output = h->txString[h->txOutdex];
  h->txOutdex = (h->txOutdex + 1) % TX_BUFFER_MAX;
  return 1;
}

int UsbQueueGetSize(SerialHandle *h)
{
  return (h->txOutdex > h->txIndex) ?
  TX_BUFFER_MAX - h->txOutdex + h->txIndex :
                                      h->txIndex - h->txOutdex;
}

int UsbQueueGetAll(SerialHandle *h, uint8_t *output, int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    if (!UsbQueueGet(h, output + i))
      return i;
  }
  return count;
}

void SerialInit(UART_HandleTypeDef *usbHuartHandle, UART_HandleTypeDef *gpsHuartHandle)
{
  usbHandle.huart = usbHuartHandle;
  usbHandle.txCplt = 1;
  gpsHandle.huart = gpsHuartHandle;
  gpsHandle.txCplt = 1;
  // Initiate automatic receive through DMA one character at a time
  HAL_UART_Receive_DMA(usbHandle.huart, &usbHandle.rxBuffer, sizeof(usbHandle.rxBuffer));
  HAL_UART_Receive_DMA(gpsHandle.huart, &gpsHandle.rxBuffer, sizeof(gpsHandle.rxBuffer));
}

void UsbSendFromFIFO(SerialHandle *h)
{
//  // Sends a character from the FIFO through DMA
//  HAL_UART_StateTypeDef uartState = HAL_UART_GetState(usbHuart);
//  if (uartState == HAL_UART_STATE_READY || uartState == HAL_UART_STATE_BUSY_RX)
//  {
//    if (usbTxCplt)
//    {
//      usbTxCplt = 0;
//      int size = UsbQueueGetSize();
//      if (size > 0)
//      {
//        uint8_t c[size];
//        UsbQueueGetAll(c, size);
//        while (HAL_UART_Transmit_DMA(usbHuart, c, size) != HAL_OK)
//          ;
//      }
//      else
//      {
//        usbTxCplt = 1;
//      }
//    }
//  }

  HAL_UART_StateTypeDef uartState = HAL_UART_GetState(h->huart);
  if ((uartState == HAL_UART_STATE_READY) || (uartState == HAL_UART_STATE_BUSY_RX))
  {
    uint8_t c;
    if (UsbQueueGet(h, &c))
      while (HAL_UART_Transmit_DMA(h->huart, &c, 1) != HAL_OK)
        ;
  }
}

void SerialTransmit(SerialHandle *h, char *ptr, int len)
{
  int i;
  for (i = 0; i < len; i++)
  {
    UsbQueuePut(h, ptr[i]);
  }
  UsbSendFromFIFO(h);

}

void UsbTransmit(char *ptr, int len)
{
  SerialTransmit(&usbHandle, ptr, len);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  SerialTransmit(&usbHandle, "ERROR\r\n", 7);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huartHandle)
{
  if (huartHandle == usbHandle.huart)
  {
    usbHandle.txCplt = 1;
    UsbSendFromFIFO(&usbHandle);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huartHandle)
{
  int echo = 1;
  SerialHandle *h;
  int i;
  char rxChar;
  if (huartHandle == usbHandle.huart)
  {
    rxChar = (char) usbHandle.rxBuffer;
  }
  else if (huartHandle == gpsHandle.huart)
  {
    rxChar = (char) gpsHandle.rxBuffer;
  }
  else
  {
    return;
  }
  h = &usbHandle;

  if (rxChar == 127 || rxChar == 8)
  {
    // Backspace or delete
    SerialTransmit(h, "\b \b", 3);
    h->rxIndex = (h->rxIndex > 0) ? h->rxIndex - 1 : 0;
    h->rxString[h->rxIndex] = 0;
  }
  else if (rxChar == '\r')
  {
    // Echo carriage return
    SerialTransmit(h, "\r\n", 2);
    // Add null terminator
    h->rxString[h->rxIndex] = '\0';
//    ShellExecute((char *) &rxString);
    // Clear the buffer
    h->rxIndex = 0;
    for (i = 0; i < RX_BUFFER_MAX; i++)
      h->rxString[i] = 0;
//    HAL_GPIO_WritePin(GPS_ON_OFF_GPIO_Port, GPS_ON_OFF_Pin, GPIO_PIN_SET);
//    HAL_Delay(200);
//    HAL_GPIO_WritePin(GPS_ON_OFF_GPIO_Port, GPS_ON_OFF_Pin, GPIO_PIN_RESET);

  }
  else if (isGlyph(rxChar))
  {
    // Echo the character
    SerialTransmit(h, &rxChar, 1);
    // Append character and increment cursor
    h->rxString[h->rxIndex] = rxChar;
    if (h->rxIndex < RX_BUFFER_MAX - 1)
      h->rxIndex++;
  }

}
