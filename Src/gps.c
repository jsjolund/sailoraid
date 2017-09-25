/*
 * gps.c
 *
 *  Created on: Sep 24, 2017
 *      Author: jsjolund
 */

#include "gps.h"

static nmeaINFO info;
static nmeaPARSER parser;

void GPSinit(void)
{
  nmea_zero_INFO(&info);
  nmea_parser_init(&parser);
//  nmea_parser_destroy(&parser);

  HAL_Delay(1000);
  HAL_GPIO_WritePin(GPS_NRST_GPIO_Port, GPS_NRST_Pin, GPIO_PIN_SET); // low
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPS_NRST_GPIO_Port, GPS_NRST_Pin, GPIO_PIN_SET); // hi-Z
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPS_ON_OFF_GPIO_Port, GPS_ON_OFF_Pin, GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(GPS_ON_OFF_GPIO_Port, GPS_ON_OFF_Pin, GPIO_PIN_RESET);
}

nmeaINFO GPSparse(char *str, int len)
{
  nmea_parse(&parser, str, len, &info);
  return info;
}
