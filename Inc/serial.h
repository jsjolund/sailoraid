/*
 * serial.h
 *
 *  Created on: Sep 21, 2017
 *      Author: user
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "stm32f4xx_hal.h"

#define RX_BUFFER_MAX 20
#define TX_BUFFER_MAX 100



void SerialInit(UART_HandleTypeDef *usbHuartHandle, UART_HandleTypeDef *gpsHuartHandle);
void UsbTransmit(char *ptr, int len);

#endif /* SERIAL_H_ */
