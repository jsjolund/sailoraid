/*
 * 		Kalman Filter
 *		Created on: Oct 15, 2017
 *      Author: Oskar
 *      Email: axelsson.11@gmail.com
 *
*/

#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

#include <stdint.h>
#include <math.h>
#include "gps.h"
#include "state.h"
//#include "sensor_calc.h"

// IMU sensors
#define SENSOR_ACCELEROMETER					0
#define	SENSOR_GPS								1
#define SENSOR_BAROMETER						2
#define X_calc									0
#define Y_calc									1
#define DEG_TO_RAD								3.14f/180.0f


//Kalman Noise from hardware.
#define P_KALMAN_ACC_NOISE						0.001f
#define P_KALMAN_GYRO_NOISE						0.000012f
#define P_KALMAN_GPS_pos						1.0f
#define P_KALMAN_GPS_vel						0.250f
#define sdev_phi								1.0f
#define sdev_lam								1.0f
#define sdev_h									1.0f
#define sdev_vn									0.25f
#define sdev_ve									0.25f

//Some constants
#define	om_e									0.000072921158f 	//angular velocity (e-frame)
#define	a										6378137.0f			//Earth radius
#define	F										1/298.257223563f	//flattening, NOT assuming earth to be perfect spherical
#define gamma									9.8123f;			//Gravity, assuming constant over Earth.
#define q										1.0f;



#define P_KALMAN_GPS_VELOCITY_NOISE				20.0f
#define P_KALMAN_BAROMETER_NOISE				30.0f

#define P_KALMAN_STATE_ACC_NOISE				50.0f
#define P_KALMAN_STATE_GPS_VERTICAL_NOISE		1.0f
#define P_KALMAN_STATE_GPS_HORIZONTAL_NOISE		10.0f
#define P_KALMAN_STATE_GPS_VELOCITY_NOISE		5.0f
#define P_KALMAN_STATE_VERTICAL_VELOCITY_NOISE	50.0f
#define P_KALMAN_STATE_BARO_OFFSET_NOISE		0.01f
#define one 									1.00f



struct Pos_Kalman
{
  float *pos_lat, *pos_long, *pos_h, *magnitude;
  float roll, pitch, yaw;
  double *r_n[3];
};

struct Vel_Kalman
{
 float *vel_lat, *vel_long, *vel_h, *magnitude;
 double *v_n[3];
};

struct Acc_Kalman
{
  float *acc_b[3];
};

struct Angle_Kalman
{
	float *roll, *pitch, *yaw;
};

struct Output_Kalman
{
 double del_err[9];
 double P_k_k[9][9];
 double X_k_k[9][1];
};

struct Matrices_Kalman
{
 double Q[6][6];
 double R[6][6];
 double P[9][9];
 double F_t[9][9]; //state transition matrix
 double G[9][6];
 double H[6][9];
};

struct KalmanState
{
  struct Pos_Kalman POS;
  struct Vel_Kalman VEL;
  struct Acc_Kalman ACC;
  struct Output_Kalman OUT;
  struct Angle_Kalman ANG;
  struct Matrices_Kalman MK;
};

extern struct KalmanState Kalman;

// */
void Kalman_Init(void);

void Kalman_Update(void);

#endif /* KALMAN_FILTER_H_ */
