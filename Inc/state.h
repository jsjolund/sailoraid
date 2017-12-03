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
  float ax, ay, az; // Acceleration in g
  float gx, gy, gz; // Angular velocity in degrees/s
  float mx, my, mz; // Magnetic field strength in gauss
  float roll, pitch, yaw; // Orientation in degrees
} IMUstate_t;

typedef struct ENVstate
{
  float humidity; // Humidity in percent
  float pressure; // Pressure in mBar
  float temperature; // Temperature in C
} EnvState_t;

typedef struct GPSdateTime
{
  int year, month, day;
  int hour, min, sec, hsec;
} GPSdateTime_t;

typedef struct GPScoordinate
{
  float latitude, longitude; // Position in degrees
  float elevation; // Elevation in meters above sea level
  float speed; // Speed in m/s
  float direction; // Direction in degrees
} GPScoordinate_t;

typedef struct GPSinfo
{
  int satUse, satView;
  float battery;
} GPSinfo_t;

typedef struct GPSstate
{
  GPSdateTime_t time;
  GPScoordinate_t pos;
  GPSinfo_t info;
} GPSstate_t;

typedef struct RangeState
{
  float range0; // Range in cm
} RangeState_t;

typedef struct LoadCellState
{
  float cell0, cell1;
} LoadCellState_t;

typedef struct SensorState
{
  IMUstate_t imu;
  EnvState_t env;
  GPSstate_t gps;
  RangeState_t range;
  LoadCellState_t load;
} SensorState_t;

#endif /* STATE_H_ */
