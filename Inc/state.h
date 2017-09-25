/*
 * state.h
 *
 *  Created on: Sep 24, 2017
 *      Author: user
 */

#ifndef STATE_H_
#define STATE_H_

typedef struct SensorState
{
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;
  float yaw, pitch, roll;
  float temperature;
  float humidity;
  float pressure;
  int year, month, day;
  int hour, min, sec, hsec;
  float latitude, longitude, elevation;
  float speed;
  float direction;
  int satUse, satView;
} SensorState;

#endif /* STATE_H_ */
