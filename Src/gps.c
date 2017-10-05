/*
 * gps.c
 *
 *  Created on: Sep 24, 2017
 *      Author: jsjolund
 */

#include "gps.h"

void GPSinit(void)
{
  HAL_Delay(1000);
  HAL_GPIO_WritePin(GPS_NRST_GPIO_Port, GPS_NRST_Pin, GPIO_PIN_SET); // low
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPS_NRST_GPIO_Port, GPS_NRST_Pin, GPIO_PIN_SET); // hi-Z
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPS_ON_OFF_GPIO_Port, GPS_ON_OFF_Pin, GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(GPS_ON_OFF_GPIO_Port, GPS_ON_OFF_Pin, GPIO_PIN_RESET);
}

void GPSparse(char *str, int len, nmeaINFO *info)
{
  nmeaPARSER parser;
//  nmea_zero_INFO(&info);
  nmea_parser_init(&parser);
  nmea_parse(&parser, str, len, info);
  nmea_parser_buff_clear(&parser);
  nmea_parser_destroy(&parser);
}

float NMEAtoGPS(float in_coords)
{
  float f = in_coords;
  int firsttwodigits = ((int) f) / 100; //This assumes that f < 10000.
  float nexttwodigits = f - (float) (firsttwodigits * 100);
  float theFinalAnswer = (float) (firsttwodigits + nexttwodigits / 60.0);
  return theFinalAnswer;
}
