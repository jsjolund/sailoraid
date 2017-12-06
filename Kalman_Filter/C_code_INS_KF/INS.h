/*
 * INS, Inertial Navigation System
 *
 *  Created on: Oct 23, 2017
 *      Author: Oskar
 *      Email: axelsson.11@gmail.com
 */

#ifndef INS_H_
#define INS_H_

#include "hal.h"
#include <math.h>
#include "state.h"
#include "serial.h"


#define ACC_SCALE			(1.0f/4096.0f)

#define	om_e									0.000072921158f 	//angular velocity (e-frame)
#define	a										6378137.0f			//Earth radius
#define	F										1/298.257223563f	//flattening, NOT assuming earth to be perfect spherical
#define gamma									9.8123f;			//Gravity, assuming constant over Earth.
#define deg_to_rad								3.14f/180.0f

/*VALUES FROM MATLAB SCRIPT*/
#define ACC_X_OFFSET		0.01//0.0141f
#define ACC_Y_OFFSET		-0.02//-0.6293f
#define ACC_Z_OFFSET		1
#define ACC_X_SCALE			0.0081f
#define ACC_Y_SCALE			-0.0046f
#define ACC_Z_SCALE			-0.0161f

struct Accelerometer
{
  float *X, *Y, *Z, *magnitude;
  float roll, pitch, yaw;
} ACCstate;

struct Gyrometer
{
 float *X, *Y, *Z, *magnitude;
 int16_t rawX, rawY, rawZ;
 uint8_t rawValues[6];
 uint32_t timestampUpdate, timestampChange;
 FlagStatus valid;
} GYRstate;

struct Gps
{
 float *pos_lat, *pos_long, *pos_h;
 float *vel_lat, *vel_long, *vel_h;
 float old_gps[2];
 uint8_t rawValues[6];
 uint32_t timestampUpdate, timestampChange;
 FlagStatus valid;
};

struct Magnometer
{
 float *X, *Y, *Z, *magnitude;
 int16_t rawX, rawY, rawZ;
 uint8_t rawValues[6];
 uint32_t timestampUpdate, timestampChange;
 FlagStatus valid;
} ;

struct Pos_Vel
{
 double r_n[3], v_n[3];
 double r_n_old[2], v_n_old[3];
 double del_r_n[3][1];
 double del_v_n[3][1];
 double *del_err[9];
 float *quat[4];
 float acc_b[3];
 float phi_error, theta_error, psi_error;
};

struct INSstate
{
  struct Accelerometer ACC;
  struct Gps GPS;
  struct Gyrometer GYR;
  struct Magnometer MAG;
  struct Pos_Vel POS_VEL;
};

extern struct INSstate INS;
/*
 *
 */

/*
 */
void INS_Init(void);
/*

 */
void INS_Update(void);

#endif /* INS_H_ */
