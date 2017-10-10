/*
 * gps.h
 *
 *  Created on: Sep 24, 2017
 *      Author: jsjolund
 */

#ifndef GPS_H_
#define GPS_H_

#include "main.h"
#include "state.h"
#include <nmealib/info.h>
#include <nmealib/parser.h>
#include <nmealib/nmath.h>
#include <string.h>

void GPSinit(void);
void GPSparse(char *str, int len, NmeaInfo *info, GPSstate *gps);
float NMEAtoGPS(float in_coords);

#endif /* GPS_H_ */
