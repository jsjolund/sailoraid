/*
 * state.h
 *
 *  Created on: Sep 24, 2017
 *      Author: jsjolund
 */

#ifndef STATE_H_
#define STATE_H_

typedef struct IMUstate
{
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;
  float roll, pitch, yaw;
} IMUstate;

typedef struct ENVstate
{
  float humidity;
  float pressure;
  float temperature;
} EnvState;

typedef struct GPSdateTime
{
  int year, month, day;
  int hour, min, sec, hsec;
} GPSdateTime;

typedef struct GPScoordinate
{
  float latitude, longitude, elevation;
  float speed;
  float direction;
} GPScoordinate;

typedef struct GPSinfo
{
  int satUse, satView;
} GPSinfo;

typedef struct GPSstate
{
  GPSdateTime time;
  GPScoordinate pos;
  GPSinfo info;
} GPSstate;

typedef struct SensorState
{
  IMUstate imu;
  EnvState env;
  GPSstate gps;
} SensorState;

#endif /* STATE_H_ */
