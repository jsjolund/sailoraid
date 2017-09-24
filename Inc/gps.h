/*
 * gps.h
 *
 *  Created on: Sep 24, 2017
 *      Author: user
 */

#ifndef GPS_H_
#define GPS_H_

#include "main.h"
#include "nmea/nmea.h"
#include <string.h>

void GPSinit(void);
nmeaINFO GPSparse(char *str, int len);

#endif /* GPS_H_ */
