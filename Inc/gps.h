/*
 * gps.h
 *
 *  Created on: Sep 24, 2017
 *      Author: jsjolund
 */

#ifndef GPS_H_
#define GPS_H_

#include "main.h"
#include "nmea/nmea.h"
#include <string.h>

void GPSinit(void);
nmeaINFO GPSparse(char *str, int len);
float NMEAtoGPS(float in_coords);

#endif /* GPS_H_ */
