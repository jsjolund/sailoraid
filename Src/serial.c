/*
 * serial.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#include "serial.h"

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
static BOOL gpsEcho = FALSE;

static int IsGlyph(char c)
{
  return (c >= ' ' && c <= '~');
}

static int QueuePut(SerialHandle *h, uint8_t input)
{
  if (h->txIndex == ((h->txOutdex - 1 + TX_BUFFER_MAX) % TX_BUFFER_MAX))
    return 0;
  h->txString[h->txIndex] = input;
  h->txIndex = (h->txIndex + 1) % TX_BUFFER_MAX;
  return 1;
}

static int QueueGet(SerialHandle *h, uint8_t *output)
{
  if (h->txIndex == h->txOutdex)
    return 0;
  *output = h->txString[h->txOutdex];
  h->txOutdex = (h->txOutdex + 1) % TX_BUFFER_MAX;
  return 1;
}

static int QueueGetSize(SerialHandle *h)
{
  return (h->txOutdex > h->txIndex) ?
  TX_BUFFER_MAX - h->txOutdex + h->txIndex :
                                      h->txIndex - h->txOutdex;
}

static int QueueGetAll(SerialHandle *h, uint8_t *output, int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    if (!QueueGet(h, output + i))
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

static void SendFromFIFO(SerialHandle *h)
{
  HAL_UART_StateTypeDef uartState = HAL_UART_GetState(h->huart);
  if ((uartState == HAL_UART_STATE_READY) || (uartState == HAL_UART_STATE_BUSY_RX))
  {
    uint8_t c;
    if (QueueGet(h, &c))
      while (HAL_UART_Transmit_DMA(h->huart, &c, 1) != HAL_OK)
        ;
  }
}

static void SerialTransmit(SerialHandle *h, char *ptr, int len)
{
  int i;
  for (i = 0; i < len; i++)
  {
    QueuePut(h, ptr[i]);
  }
  SendFromFIFO(h);

}

void SerialUsbTransmit(char *ptr, int len)
{
  SerialTransmit(&usbHandle, ptr, len);
}

void GPSecho(BOOL echo)
{
    gpsEcho = echo;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart == gpsHandle.huart)
  {
    char buffer [30];
    int cx;
    cx = snprintf(buffer, 30, "ERROR CODE %d GPS UART\r\n", (int) huart->ErrorCode);
    SerialTransmit(&usbHandle, buffer, cx);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huartHandle)
{
  if (huartHandle == usbHandle.huart)
  {
    usbHandle.txCplt = 1;
    SendFromFIFO(&usbHandle);
  }
}

float NMEAtoGPS(float in_coords)
{
  float f = in_coords;
  int firsttwodigits = ((int) f) / 100; //This assumes that f < 10000.
  float nexttwodigits = f - (float) (firsttwodigits * 100);
  float theFinalAnswer = (float) (firsttwodigits + nexttwodigits / 60.0);
  return theFinalAnswer;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huartHandle)
{
  SerialHandle *h;
  int i;
  char rxChar;
  if (huartHandle == usbHandle.huart)
  {
    gpsEcho = FALSE;
    IMUecho(FALSE);
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
    if (h == &usbHandle)
      SerialTransmit(h, "\b \b", 3);

    h->rxIndex = (h->rxIndex > 0) ? h->rxIndex - 1 : 0;
    h->rxString[h->rxIndex] = 0;
  }
  else if (rxChar == '\n')
  {

  }
  else if (rxChar == '\r')
  {
    // Carriage return
    if (h == &usbHandle)
      SerialTransmit(h, "\r\n", 2);
    // Add null terminator
    h->rxString[h->rxIndex] = '\0';
    char *row = (char *) &h->rxString;
    if (h == &gpsHandle)
    {
      h->rxString[h->rxIndex] = '\r';
      if (h->rxIndex < RX_BUFFER_MAX - 1)
        h->rxIndex++;
      h->rxString[h->rxIndex] = '\n';
      if (h->rxIndex < RX_BUFFER_MAX - 1)
        h->rxIndex++;
      nmeaINFO info = GPSparse(row, strlen(row));
      if (gpsEcho)
      {
        printf(row);
        float lat = NMEAtoGPS(info.lat);
        float lon = NMEAtoGPS(info.lon);
        printf("date %d/%d %d, time %d:%d, lat %f, lon %f, elv %f, satuse %d, satview %d\n", info.utc.day, info.utc.mon + 1, info.utc.year + 1900,
            info.utc.hour, info.utc.min, lat, lon, info.elv, info.satinfo.inuse, info.satinfo.inview);
      }
    }
    else if (h == &usbHandle)
    {
      ShellExecute(row);
    }
    // Clear the buffer
    h->rxIndex = 0;
    for (i = 0; i < RX_BUFFER_MAX; i++)
      h->rxString[i] = 0;
  }
  else
  {
    // Echo the character
    if (h == &usbHandle)
      SerialTransmit(h, &rxChar, 1);
    // Append character and increment cursor
    h->rxString[h->rxIndex] = rxChar;
    if (h->rxIndex < RX_BUFFER_MAX - 1)
      h->rxIndex++;
  }
}
