#include "Kalman_Filter.h"
#include "INS.h"

//
//extern struct Attitude attitude;
struct KalmanState Kalman;
extern SensorState_t sensor;

/*
 * initializes Kalman filters
 */
void Kalman_Init(void) {
	// measurement vector

	Kalman.POS.pos_lat = &sensor.gps.pos.latitude_rad;		//longitude in [rad]
	Kalman.POS.pos_long = &sensor.gps.pos.longitude_rad; 	//Latitude in [rad]
	Kalman.POS.pos_h = &sensor.gps.pos.elevation;			//Altitude in [m]

	Kalman.VEL.vel_lat = &sensor.gps.pos.latitude_vel;			//velocity in [rad/s], latitude.
	Kalman.VEL.vel_long = &sensor.gps.pos.longitude_vel;			//velocity in [rad/s], longitude.
	Kalman.VEL.vel_h = 0; 									// Attitude that has to be used in the above

	Kalman.POS.r_n[0] = &INS.POS_VEL.r_n[0]; //Position from INS
	Kalman.POS.r_n[1] = &INS.POS_VEL.r_n[1];
	Kalman.POS.r_n[2] = &INS.POS_VEL.r_n[2];
//
	Kalman.VEL.v_n[0] = &INS.POS_VEL.v_n[0]; //Velocity from INS
	Kalman.VEL.v_n[1] = &INS.POS_VEL.v_n[1];
	Kalman.VEL.v_n[2] = &INS.POS_VEL.v_n[2];

	Kalman.ANG.roll = &sensor.imu.roll;		 //Euler angles
	Kalman.ANG.pitch = &sensor.imu.pitch;
	Kalman.ANG.yaw = &sensor.imu.yaw;

	// state noise
	/*
	 * 	MKariance matrix Q is a diagmatrix 6x6 (continouse time only) with noise from measurement i.e measures from IMU
	 */


	//    /* zero-out matrices */
	//    zeros(ekf.P, n, n);
	//    zeros(ekf.Q, n, n);
	//    zeros(ekf.R, m, m);
	//    zeros(ekf.G, n, m);
	//    zeros(ekf.F, n, n);
	//    zeros(ekf.H, m, n);

	//	static void zeros(double * a, int m, int n)
	//	{
	//	    int j;
	//	    for (j=0; j<m*n; j++)
	//	        a[j] = 0;
	//	}

	//	zeros(ekf.P, n, n);
	Kalman.MK.Q[0][0] = P_KALMAN_ACC_NOISE; //Kalman measurement noise acc_x;
	Kalman.MK.Q[1][1] = P_KALMAN_ACC_NOISE;//Kalman measurement noise acc_y;
	Kalman.MK.Q[2][2] = P_KALMAN_ACC_NOISE;//Kalman measurement noise acc_z;
	Kalman.MK.Q[3][3] = P_KALMAN_GYRO_NOISE;	//Kalman measurement noise gyro_x
	Kalman.MK.Q[4][4] = P_KALMAN_GYRO_NOISE;	//Kalman measurement noise gyro_y
	Kalman.MK.Q[5][5] = P_KALMAN_GYRO_NOISE;	//Kalman measurement noise gyro_z

	/*MKariance matrix R is a diagmatrix 6x6 (in continouse and discrete) with noise from GPS.*/

	Kalman.MK.R[0][0] = P_KALMAN_GPS_pos; //Kalman MKariance matrix, noise gps_pos
	Kalman.MK.R[1][1] = P_KALMAN_GPS_pos; //Kalman MKariance matrix, noise gps_pos
	Kalman.MK.R[2][2] = P_KALMAN_GPS_pos;//Kalman MKariance matrix, noise gps_pos
	Kalman.MK.R[3][3] = P_KALMAN_GPS_vel;//Kalman MKariance matrix, noise gps_vel
	Kalman.MK.R[4][4] = P_KALMAN_GPS_vel;//Kalman MKariance matrix, noise gps_vel
	Kalman.MK.R[5][5] = P_KALMAN_GPS_vel;//Kalman MKariance matrix, noise gps_vel

	/*Setup P_0*/
	Kalman.MK.P[0][0] = sdev_phi;
	Kalman.MK.P[1][1] = sdev_lam;
	Kalman.MK.P[2][2] = sdev_h;
	Kalman.MK.P[3][3] = sdev_vn;
	Kalman.MK.P[4][4] = sdev_ve;
	Kalman.MK.P[5][5] = sdev_h;
	Kalman.MK.P[6][6] = 0.1f;
	Kalman.MK.P[7][7] = 0.1f;
	Kalman.MK.P[8][8] = 0.1f;

	/*Setup initial values for X*/
	Kalman.OUT.X_k_k[0][0] = 0;
	Kalman.OUT.X_k_k[1][0] = 0;
	Kalman.OUT.X_k_k[2][0] = 0;
	Kalman.OUT.X_k_k[3][0] = 0;
	Kalman.OUT.X_k_k[4][0] = 0;
	Kalman.OUT.X_k_k[5][0] = 0;
	Kalman.OUT.X_k_k[6][0] = 0;
	Kalman.OUT.X_k_k[7][0] = 0;
	Kalman.OUT.X_k_k[8][0] = 0;

	Kalman.OUT.P_k_k[0][0] = sdev_phi;
	Kalman.OUT.P_k_k[1][1] = sdev_lam;
	Kalman.OUT.P_k_k[2][2] = sdev_h;
	Kalman.OUT.P_k_k[3][3] = sdev_vn;
	Kalman.OUT.P_k_k[4][4] = sdev_ve;
	Kalman.OUT.P_k_k[5][5] = sdev_h;
	Kalman.OUT.P_k_k[6][6] = 0.1f;
	Kalman.OUT.P_k_k[7][7] = 0.1f;
	Kalman.OUT.P_k_k[8][8] = 0.1f;


//}static void zeros(double * a, int m, int n)
//{
//    int j;
//    for (j=0; j<m*n; j++)
//        a[j] = 0;
//}

//	zeros(ekf.P, n, n);

//	/*Setup initial values for P_k_k*/
//	void zeros(double * p, int m, int n)
//	{
//		for (int i = 0; i < n*m; i++) {
//			p[i] = 0;
//		}
//	}
//
//	int n = 9;
//	int m = 6;
//	/*Putting all Kalman matrices to zero.*/
//	zeros(Kalman.MK.F_t, n, n);
//	zeros(Kalman.MK.G, n, m);
//	zeros(Kalman.MK.H, m, n);
//	zeros(Kalman.MK.P, n, n);
//	zeros(Kalman.MK.Q, m, m);
//	zeros(Kalman.MK.R, m, m);

//	for (int i = 0; i < 9; i++) {
//				for (int j = 0; j < 9; j++) {
//					printf("%3.4f \r\n", Kalman.MK.F_t[i][j]);		//making sure its = 0
//				}
//	}
}

void Kalman_Update() {

	int i, j, k;
	float gps_del_t;

	gps_del_t = 1.0f;

	/* Set some variables.*/
	float om_e_ie[3][1] = {{0}};
	om_e_ie[0][0] = 0;
	om_e_ie[1][0] = 0;
	om_e_ie[2][0] = om_e;

	double e_buff = (2 * F) - (F * F);
	double e = sqrt(e_buff);
	//printf("%3.4f \r\n", e);


	float r_n[3] = {0};			//define a vector that hold positions from INS.
	float r_g[3] = {0};			//define a vector that hold positions from GPS.
	float v_n[3] = {0};			//define a vector that hold velocity from INS.
	float v_g[3] = {0};			//define a vector that hold velocity from GPS.

	/*INS should be good as an first estimate*/
	r_n[0] = *Kalman.POS.r_n[0];
	r_n[1] = *Kalman.POS.r_n[1];
	r_n[2] = *Kalman.POS.r_n[2];

	r_g[0] = *Kalman.POS.pos_lat; 	//GPS posistion
	r_g[1] = *Kalman.POS.pos_long;
	r_g[2] = *Kalman.POS.pos_h;


	v_n[0] = *Kalman.VEL.v_n[0];
	v_n[1] = *Kalman.VEL.v_n[1];
	v_n[2] = *Kalman.VEL.v_n[2];

	v_g[0] = *Kalman.VEL.vel_lat;
	v_g[1] = *Kalman.VEL.vel_long;
	v_g[2] = 0;

	/*New variable names so nothing is messed up from report*/
	float phi = r_n[0];
	float lam = r_n[1];
	float h = r_n[2];
//    printf("%3.4f \r\n", r_n[0]);


	/*Define constant to velocity matrix, (NOT assuming Earth to be perfect spherical)*/
	float M_buff1 = (1 - e * e);
	float M_buff2 = (1 - e * e * sin(phi) * sin(phi));
	float M_buff3 = pow(M_buff2, 1.5f);
	float M = a * M_buff1 / M_buff3;

	double N_buff1 = a;
	double N_buff2 = pow(M_buff2, 0.5f);
	double N = N_buff1 / N_buff2;

    //printf("%3.4f \r\n", N);

	double R_buff = M * N;
//		printf("%3.4f %3.4f \r\n",position.horizontalX.x[2],position.horizontalX.x[2]);
//    printf("%3.4f \r\n", R_buff);
	double R = sqrtf(R_buff);

	/*Velocity to Position*/
	float vel2pos[3][3] = {{0}};
	float vel2pos_buff = (N + h) * cos(phi);
	vel2pos[0][0] = 1 / (M + h);
	vel2pos[1][1] = 1 / (vel2pos_buff);
	vel2pos[2][2] = -1.0f;

	/*Direction from velocity*/
	float dr_n[3][3] = {{0}};
	dr_n[0][0] = vel2pos[0][0] * v_n[0];
	dr_n[1][1] = vel2pos[1][1] * v_n[1];
	dr_n[2][2] = vel2pos[2][2] * v_n[2];

	/*Create new constant so nothing is messed up from report.*/
	float dphi = dr_n[0][0];
	float dlam = dr_n[1][1];
	float vn = v_n[0];
	float ve = v_n[1];
	float vd = v_n[2];

	/*Rotation matrix, from  Earth to navigation frame.*/
	float C_n_e[3][3] = {{0}};
	C_n_e[0][0] = -sin(phi) * cos(lam);
	C_n_e[0][1] = -sin(phi) * sin(lam);
	C_n_e[0][2] = cos(phi);
	C_n_e[1][0] = -sin(lam);
	C_n_e[1][1] = cos(lam);
	C_n_e[1][2] = 0;
	C_n_e[2][0] = -cos(phi) * cos(lam);
	C_n_e[2][1] = -cos(phi) * sin(lam);
	C_n_e[2][2] = -sin(phi);

	/*Define omega vectors*/
	float om_n_ie[3][1] = {{0}};
	float om_n_en[3][1] = {{0}};
	float om_n_in[3][1] = {{0}};

	/*Matrix multiplicaion*/
	om_n_ie[0][0] = ((C_n_e[0][0] * om_e_ie[0][0])
			+ (C_n_e[0][1] * om_e_ie[1][0]) + (C_n_e[0][2] * om_e_ie[2][0]));
	om_n_ie[1][0] = ((C_n_e[1][0] * om_e_ie[0][0])
			+ (C_n_e[1][1] * om_e_ie[1][0]) + (C_n_e[1][2] * om_e_ie[2][0]));
	om_n_ie[2][0] = ((C_n_e[2][0] * om_e_ie[0][0])
			+ (C_n_e[2][1] * om_e_ie[1][0]) + (C_n_e[2][2] * om_e_ie[2][0]));


	om_n_en[0][0] = dlam * cos(phi);
	om_n_en[1][0] = -dphi;
	om_n_en[2][0] = -dlam * sin(phi);

	om_n_in[0][0] = om_n_en[0][0] + om_n_ie[0][0];
	om_n_in[1][0] = om_n_en[1][0] + om_n_ie[1][0];
	om_n_in[2][0] = om_n_en[2][0] + om_n_ie[2][0];

	/*Defining the state transition matrix F, 9x9 matrix. We will build F with several small F's*/
	float Frr[3][3] = {{0}};
	Frr[0][0] = 0;
	Frr[0][1] = 0;
	Frr[0][2] = -vn / ((M + h) * (M + h));
	Frr[1][0] = (ve * sin(phi)) / ((N + h) * cos(phi) * cos(phi));
	Frr[1][1] = 0;
	Frr[1][2] = -ve / ((N + h) * (N + h) * cos(phi));
	Frr[2][0] = 0;
	Frr[2][1] = 0;
	Frr[2][2] = 0;

	float Frv[3][3] = {{0}};
	Frv[0][0] = 1 / (M + h);
	Frv[0][1] = 0;
	Frv[0][2] = 0;
	Frv[1][0] = 0;
	Frv[1][1] = 1 / ((N + h) * cos(phi));
	Frv[1][2] = 0;
	Frv[2][0] = 0;
	Frv[2][1] = 0;
	Frv[2][2] = -1;

	float Fvr[3][3] = {{0}};
	Fvr[0][0] = (-2 * ve * om_e * cos(phi))
			- ((ve * ve) / ((N + h) * cos(phi) * cos(phi)));
	Fvr[0][1] = 0;
	Fvr[0][2] = (-vn * vd / ((M + h) * (M + h)))
			+ (ve * ve * tan(phi) / ((N + h) * (N + h)));
	Fvr[1][0] = (2 * om_e * (vn * cos(phi) - vd * sin(phi)))
			+ ve * vn / ((N + h) * cos(phi) * cos(phi));
	Fvr[1][1] = 0;
	Fvr[1][2] = -ve * vd / ((N + h) * (N + h))
			- vn * ve * tan(phi) / ((N + h) * (N + h));
	Fvr[2][0] = 2 * ve * om_e * sin(phi);
	Fvr[2][1] = 0;
	Fvr[2][2] = ((ve * ve) / ((N + h) * (N + h)))
			+ (vn * vn) / ((M + h) * (M + h)) - 2 - (2 * 9.8123) / (R + h);

	float Fvv[3][3] = {{0}};
	Fvv[0][0] = vd / (M + h);
	Fvv[0][1] = -2 * om_e * sin(phi) - 2 * ve * tan(phi) / (N + h);
	Fvv[0][2] = vn / (M + h);
	Fvv[1][0] = 2 * om_e * sin(phi) + ve * tan(phi) / (N + h);
	Fvv[1][1] = (vd + vn * tan(phi)) / (N + h);
	Fvv[1][2] = 2 * om_e * cos(phi) + ve / (N + h);
	Fvv[2][0] = -2 * vn / (M + h);
	Fvv[2][1] = -2 * om_e * cos(phi) - 2 * ve / (N + h);
	Fvv[2][2] = 0;

	float Fer[3][3] = {{0}};
	Fer[0][0] = -om_e * sin(phi);
	Fer[0][1] = 0;
	Fer[0][2] = -ve / ((N + h) * (N + h));
	Fer[1][0] = 0;
	Fer[1][1] = 0;
	Fer[1][2] = vn / ((M + h) * (M + h));
	Fer[2][0] = -om_e * cos(phi) - ve / ((N + h) * (cos(phi) * cos(phi)));
	Fer[2][1] = 0;
	Fer[2][2] = ve * tan(phi) / ((N + h) * (N + h));

	float Fev[3][3] = {{0}};
	Fev[0][0] = 0;
	Fev[0][1] = 1 / (N + h);
	Fev[0][2] = 0;
	Fev[1][0] = -1 / (M + h);
	Fev[1][1] = 0;
	Fev[1][2] = 0;
	Fev[2][0] = 0;
	Fev[2][1] = -tan(phi) / (N + h);
	Fev[2][2] = 0;

	/*Load euler angles to transform body to navigation frame*/
	float roll = DEG_TO_RAD * (*Kalman.ANG.roll);		//phi
	float pitch = DEG_TO_RAD * (*Kalman.ANG.pitch);	//theta
	float yaw = DEG_TO_RAD * (*Kalman.ANG.yaw);		//psi


	double ct = cos(pitch);
	double cpsi = cos(yaw);
	double cphi = cos(roll);
	double st = sin(pitch);
	double spsi = sin(yaw);
	double sphi = sin(roll);

	/*direction cosine matrix, using EA*/
	/*double C_n_b[3][3] = {{0}};
	C_n_b[0][0] = ct * cpsi;
	C_n_b[0][1] = -cphi * spsi + sphi * st * cpsi;
	C_n_b[0][2] = sphi * spsi + cphi * st * cpsi;
	C_n_b[1][0] = ct * spsi;
	C_n_b[1][1] = cphi * cpsi + sphi * st * spsi;
	C_n_b[1][2] = -sphi * cpsi + cphi * st * spsi;
	C_n_b[2][0] = -st;
	C_n_b[2][1] = sphi * ct;
	C_n_b[2][2] = cphi * ct;*/
	float q1, q2, q3, q4;
	q1 = *INS.POS_VEL.quat[0];
	q2 = *INS.POS_VEL.quat[1];
	q3 = *INS.POS_VEL.quat[2];
	q4 = *INS.POS_VEL.quat[3];

	double C_n_b[3][3] = {{0}};
	C_n_b[0][0] = q1*q1 + q2*q2 - q3*q3 - q4*q4;
	C_n_b[0][1] = 2*(q2*q3-q1*q4);
	C_n_b[0][2] = 2*(q2*q4+q1*q3);
	C_n_b[1][0] = 2*(q2*q3+q1*q4);
	C_n_b[1][1] = (q1*q1 - q2*q2 + q3*q3 - q4*q4);
	C_n_b[1][2] = 2*(q3*q4-q1*q2);
	C_n_b[2][0] = 2*(q2*q4-q1*q3);
	C_n_b[2][1] = 2*(q3*q4+q1*q2);
	C_n_b[2][2] = (q1*q1-q2*q2-q3*q3+q4*q4);
	double f_n[3][1] = {{0}};

	/*Load acc values, NOT SURE ABOUT THIS, We might have correlations between samples.*/
	double acc_b[3][1];
	acc_b[0][0] = *Kalman.ACC.acc_b[0];
	acc_b[1][0] = *Kalman.ACC.acc_b[1];
	acc_b[2][0] = *Kalman.ACC.acc_b[2];

	/*Matrix multiplication*/
	f_n[0][0] = - ((C_n_b[0][0] * acc_b[0][0]) + (C_n_b[0][1] * acc_b[1][0])
			+ (C_n_b[0][2] * acc_b[2][0]));
	f_n[1][0] = - ((C_n_b[1][0] * acc_b[0][0]) + (C_n_b[1][1] * acc_b[1][0])
			+ (C_n_b[1][2] * acc_b[2][0]));
	f_n[2][0] = - ((C_n_b[2][0] * acc_b[0][0]) + (C_n_b[2][1] * acc_b[1][0])
			+ (C_n_b[2][2] * acc_b[2][0]));

	/*Now we can implement the real transition matrix*/

	/*
	 * 		Frr 	Frv		zeros(3,3)
	 * F_t=	Fvr		Fvv		skew(f_n)			(9X9 matrix)
	 * 		Fer		Fev 	-skew(om_n_in)
	 * */



	Kalman.MK.F_t[0][0] = Frr[0][0];
	Kalman.MK.F_t[0][1] = Frr[0][1];
	Kalman.MK.F_t[0][2] = Frr[0][2];
	Kalman.MK.F_t[1][0] = Frr[1][0];
	Kalman.MK.F_t[1][1] = Frr[1][1];
	Kalman.MK.F_t[1][2] = Frr[1][2];
	Kalman.MK.F_t[2][0] = Frr[2][0];
	Kalman.MK.F_t[2][1] = Frr[2][1];
	Kalman.MK.F_t[2][2] = Frr[2][2];

	Kalman.MK.F_t[0][3] = Frv[0][0];
	Kalman.MK.F_t[0][4] = Frv[0][1];
	Kalman.MK.F_t[0][5] = Frv[0][2];
	Kalman.MK.F_t[1][3] = Frv[1][0];
	Kalman.MK.F_t[1][4] = Frv[1][1];
	Kalman.MK.F_t[1][5] = Frv[1][2];
	Kalman.MK.F_t[2][3] = Frv[2][0];
	Kalman.MK.F_t[2][4] = Frv[2][1];
	Kalman.MK.F_t[2][5] = Frv[2][2];

	Kalman.MK.F_t[0][6] = 0;
	Kalman.MK.F_t[0][7] = 0;
	Kalman.MK.F_t[0][8] = 0;
	Kalman.MK.F_t[1][6] = 0;
	Kalman.MK.F_t[1][7] = 0;
	Kalman.MK.F_t[1][8] = 0;
	Kalman.MK.F_t[2][6] = 0;
	Kalman.MK.F_t[2][7] = 0;
	Kalman.MK.F_t[2][8] = 0;

	Kalman.MK.F_t[3][0] = Fvr[0][0];
	Kalman.MK.F_t[3][1] = Fvr[0][1];
	Kalman.MK.F_t[3][2] = Fvr[0][2];
	Kalman.MK.F_t[4][0] = Fvr[1][0];
	Kalman.MK.F_t[4][1] = Fvr[1][1];
	Kalman.MK.F_t[4][2] = Fvr[1][2];
	Kalman.MK.F_t[5][0] = Fvr[2][0];
	Kalman.MK.F_t[5][1] = Fvr[2][1];
	Kalman.MK.F_t[5][2] = Fvr[2][2];

	Kalman.MK.F_t[3][3] = Fvv[0][0];
	Kalman.MK.F_t[3][4] = Fvv[0][1];
	Kalman.MK.F_t[3][5] = Fvv[0][2];
	Kalman.MK.F_t[4][3] = Fvv[1][0];
	Kalman.MK.F_t[4][4] = Fvv[1][1];
	Kalman.MK.F_t[4][5] = Fvv[1][2];
	Kalman.MK.F_t[5][3] = Fvv[2][0];
	Kalman.MK.F_t[5][4] = Fvv[2][1];
	Kalman.MK.F_t[5][5] = Fvv[2][2];

	Kalman.MK.F_t[3][6] = 0;
	Kalman.MK.F_t[3][7] = -f_n[2][0];
	Kalman.MK.F_t[3][8] = f_n[1][0];
	Kalman.MK.F_t[4][6] = f_n[2][0];
	Kalman.MK.F_t[4][7] = 0;
	Kalman.MK.F_t[4][8] = -f_n[0][0];
	Kalman.MK.F_t[5][6] = -f_n[1][0];
	Kalman.MK.F_t[5][7] = f_n[0][0];
	Kalman.MK.F_t[5][8] = 0;

	Kalman.MK.F_t[6][0] = Fer[0][0];
	Kalman.MK.F_t[6][1] = Fer[0][1];
	Kalman.MK.F_t[6][2] = Fer[0][2];
	Kalman.MK.F_t[7][0] = Fer[1][0];
	Kalman.MK.F_t[7][1] = Fer[1][1];
	Kalman.MK.F_t[7][2] = Fer[1][2];
	Kalman.MK.F_t[8][0] = Fer[2][0];
	Kalman.MK.F_t[8][1] = Fer[2][1];
	Kalman.MK.F_t[8][2] = Fer[2][2];

	Kalman.MK.F_t[6][3] = Fev[0][0];
	Kalman.MK.F_t[6][4] = Fev[0][1];
	Kalman.MK.F_t[6][5] = Fev[0][2];
	Kalman.MK.F_t[7][3] = Fev[1][0];
	Kalman.MK.F_t[7][4] = Fev[1][1];
	Kalman.MK.F_t[7][5] = Fev[1][2];
	Kalman.MK.F_t[8][3] = Fev[2][0];
	Kalman.MK.F_t[8][4] = Fev[2][1];
	Kalman.MK.F_t[8][5] = Fev[2][2];

	Kalman.MK.F_t[6][6] = 0;
	Kalman.MK.F_t[6][7] = -(-om_n_in[2][0]);
	Kalman.MK.F_t[6][8] = -(om_n_in[1][0]);
	Kalman.MK.F_t[7][6] = -(om_n_in[2][0]);
	Kalman.MK.F_t[7][7] = 0;
	Kalman.MK.F_t[7][8] = -(-om_n_in[0][0]);
	Kalman.MK.F_t[8][6] = -(-om_n_in[1][0]);
	Kalman.MK.F_t[8][7] = -(om_n_in[0][0]);
	Kalman.MK.F_t[8][8] = 0;

//		for (i = 0; i < 3; i++){
//			for (j = 0; j < 3; j++){
//				printf("%3.4f \r\n", Kalman.MK.F_t[i][j]);
//			}
//		}

	/*Implement the design matrix, Its actually unnecessary to define the full matrix.*/
	/*
	 * 		zeros(3,3)	 zeros(3,3)
	 * G=	C_n_b 		 zeros(3,3)		(9X6 matrix)
	 * 		zeros(3,3) 	 -C_n_b
	 * */


	Kalman.MK.G[0][0] = 0;
	Kalman.MK.G[0][1] = 0;
	Kalman.MK.G[0][2] = 0;
	Kalman.MK.G[1][0] = 0;
	Kalman.MK.G[1][1] = 0;
	Kalman.MK.G[1][2] = 0;
	Kalman.MK.G[2][0] = 0;
	Kalman.MK.G[2][1] = 0;
	Kalman.MK.G[2][2] = 0;

	Kalman.MK.G[0][3] = 0;
	Kalman.MK.G[0][4] = 0;
	Kalman.MK.G[0][5] = 0;
	Kalman.MK.G[1][3] = 0;
	Kalman.MK.G[1][4] = 0;
	Kalman.MK.G[1][5] = 0;
	Kalman.MK.G[2][3] = 0;
	Kalman.MK.G[2][4] = 0;
	Kalman.MK.G[2][5] = 0;

	Kalman.MK.G[3][0] = C_n_b[0][0];
	Kalman.MK.G[3][1] = C_n_b[0][1];
	Kalman.MK.G[3][2] = C_n_b[0][2];
	Kalman.MK.G[4][0] = C_n_b[1][0];
	Kalman.MK.G[4][1] = C_n_b[1][1];
	Kalman.MK.G[4][2] = C_n_b[1][2];
	Kalman.MK.G[5][0] = C_n_b[2][0];
	Kalman.MK.G[5][1] = C_n_b[2][1];
	Kalman.MK.G[5][2] = C_n_b[2][2];

	Kalman.MK.G[3][3] = 0;
	Kalman.MK.G[3][4] = 0;
	Kalman.MK.G[3][5] = 0;
	Kalman.MK.G[4][3] = 0;
	Kalman.MK.G[4][4] = 0;
	Kalman.MK.G[4][5] = 0;
	Kalman.MK.G[5][3] = 0;
	Kalman.MK.G[5][4] = 0;
	Kalman.MK.G[5][5] = 0;

	Kalman.MK.G[6][0] = 0;
	Kalman.MK.G[6][1] = 0;
	Kalman.MK.G[6][2] = 0;
	Kalman.MK.G[7][0] = 0;
	Kalman.MK.G[7][1] = 0;
	Kalman.MK.G[7][2] = 0;
	Kalman.MK.G[8][0] = 0;
	Kalman.MK.G[8][1] = 0;
	Kalman.MK.G[8][2] = 0;

	Kalman.MK.G[6][3] = -C_n_b[0][0];
	Kalman.MK.G[6][4] = -C_n_b[0][1];
	Kalman.MK.G[6][5] = -C_n_b[0][2];
	Kalman.MK.G[7][3] = -C_n_b[1][0];
	Kalman.MK.G[7][4] = -C_n_b[1][1];
	Kalman.MK.G[7][5] = -C_n_b[1][2];
	Kalman.MK.G[8][3] = -C_n_b[2][0];
	Kalman.MK.G[8][4] = -C_n_b[2][1];
	Kalman.MK.G[8][5] = -C_n_b[2][2];

	/*Define Q (6X6 matrix)(in continouse time)*/


	//we can scale Q, so that it trusts GPS measurements more
	//this step is still discussable,
	/*
	 float scale;
	 scale = 1;
	 for (i = 0; i < 5; i++){
	 Q[i][i] = scale * Q[i][i];
	 }
	 */

	double aux[3][3] = {{0}};

	aux[0][0] = M + h;
	aux[0][1] = 0;
	aux[0][2] = 0;
	aux[1][0] = 0;
	aux[1][1] = (N + h) * cos(phi);
	aux[1][2] = 0;
	aux[2][0] = 0;
	aux[2][1] = 0;
	aux[2][2] = 1;

	/*Measurement matrix */

	Kalman.MK.H[0][0] = aux[0][0];
	Kalman.MK.H[0][1] = aux[0][1];
	Kalman.MK.H[0][2] = aux[0][2];
	Kalman.MK.H[1][0] = aux[1][0];
	Kalman.MK.H[1][2] = aux[1][1];
	Kalman.MK.H[1][2] = aux[1][2];
	Kalman.MK.H[2][0] = aux[2][0];
	Kalman.MK.H[2][1] = aux[2][1];
	Kalman.MK.H[2][2] = aux[2][2];

	Kalman.MK.H[0][3] = 0;
	Kalman.MK.H[0][4] = 0;
	Kalman.MK.H[0][5] = 0;
	Kalman.MK.H[1][3] = 0;
	Kalman.MK.H[1][4] = 0;
	Kalman.MK.H[1][5] = 0;
	Kalman.MK.H[2][3] = 0;
	Kalman.MK.H[2][4] = 0;
	Kalman.MK.H[2][5] = 0;

	Kalman.MK.H[0][6] = 0;
	Kalman.MK.H[0][7] = 0;
	Kalman.MK.H[0][8] = 0;
	Kalman.MK.H[1][6] = 0;
	Kalman.MK.H[1][7] = 0;
	Kalman.MK.H[1][8] = 0;
	Kalman.MK.H[2][6] = 0;
	Kalman.MK.H[2][7] = 0;
	Kalman.MK.H[2][8] = 0;

	Kalman.MK.H[3][0] = 0;
	Kalman.MK.H[3][1] = 0;
	Kalman.MK.H[3][2] = 0;
	Kalman.MK.H[4][0] = 0;
	Kalman.MK.H[4][1] = 0;
	Kalman.MK.H[4][2] = 0;
	Kalman.MK.H[5][0] = 0;
	Kalman.MK.H[5][1] = 0;
	Kalman.MK.H[5][2] = 0;

	Kalman.MK.H[3][3] = 1;
	Kalman.MK.H[3][4] = 0;
	Kalman.MK.H[3][5] = 0;
	Kalman.MK.H[4][3] = 0;
	Kalman.MK.H[4][4] = 1;
	Kalman.MK.H[4][5] = 0;
	Kalman.MK.H[5][3] = 0;
	Kalman.MK.H[5][4] = 0;
	Kalman.MK.H[5][5] = 1;

	Kalman.MK.H[3][6] = 0;
	Kalman.MK.H[3][7] = 0;
	Kalman.MK.H[3][8] = 0;
	Kalman.MK.H[4][6] = 0;
	Kalman.MK.H[4][7] = 0;
	Kalman.MK.H[4][8] = 0;
	Kalman.MK.H[5][6] = 0;
	Kalman.MK.H[5][7] = 0;
	Kalman.MK.H[5][8] = 0;

//	/*Update measurement vector Z(k+1)(k+1)*/
	double z_kplus_kplus[6][1] = {{0}};
//	z_kplus_kplus[0][0] = (M + h) * (r_n[0] - r_g[0]);
//	z_kplus_kplus[1][0] = cos(phi) * (N + h) * (r_n[1] - r_g[1]);
	z_kplus_kplus[0][0] = (r_n[0] - r_g[0]);
	z_kplus_kplus[1][0] = (r_n[1] - r_g[1]);
	z_kplus_kplus[2][0] = (r_n[2] - r_g[2]);
	z_kplus_kplus[3][0] = (v_n[0] - v_g[0]);
	z_kplus_kplus[4][0] = (v_n[1] - v_g[1]);
	z_kplus_kplus[5][0] = (v_n[2] - v_g[2]);

//	/*Define the discrete transition matrix phik=eye(9)+(F_k)*gps_del_t*/
	double phik[9][9] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			phik[i][j] = Kalman.MK.F_t[i][j] * gps_del_t;
		}
	}

	for (i = 0; i < 9; i++) {
		phik[i][i] = phik[i][i] + 1;
	}

//	/*Define discrete domain for Qk=Phik*G*Q*G'*Phik'*gps_del_t*/

	/*First transpsose G (9X6) -> 6X9 matrix*/
	double G_T[6][9] = {{0}};
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 6; j++) {
			G_T[j][i] = Kalman.MK.G[i][j];
		}
	}

//	/*Second transpsose Phik (9X9) -> 9X9 matrix*/
	double phik_T[9][9] = {{0}};
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			phik_T[i][j] = phik[j][i];
		}
	}
//
//	/*calculate G_T*phik_T*/
	double GPhik_T[6][9] = {{0}};

	for (i = 0; i < 6; i++) {
		for (j = 0; j < 9; j++) {
			for (k = 0; k < 9; k++) {
				GPhik_T[i][j] += G_T[i][k] * phik_T[k][j];
			}
		}
	}

//	/*Calculate Phik*G*/
	double PhikG[9][6] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 6; j++) {
			for (k = 0; k < 9; k++) {
				PhikG[i][j] += phik[i][k] * Kalman.MK.G[k][j];
			}
		}
	}
//
//	/*Calculate Phik*G*Q*/
	double PhikGQ[9][6] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 6; j++) {
			for (k = 0; k < 6; k++) {
				PhikGQ[i][j] += PhikG[i][k] * Kalman.MK.Q[k][j];
			}
		}
	}
//
//	/*Calculate Phik*G*Q*phik_T*G_T	*/
	double PhikGQ_T[9][9] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			for (k = 0; k < 6; k++) {
				PhikGQ_T[i][j] += PhikGQ[i][k] * GPhik_T[k][j];
			}
		}
	}
//
//	/*Calculate */
	double Q_k[9][9] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			Q_k[i][j] = PhikGQ_T[i][j] * gps_del_t;
		}
	}
//
	double P_kplus_k_buff[9][9] = {{0}};
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			for (k = 0; k < 9; k++) {
				P_kplus_k_buff[i][j] += Kalman.OUT.P_k_k[i][k] * phik_T[k][j];
			}
		}
	}

	double P_kplus_k[9][9] = {{0}};
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			for (k = 0; k < 9; k++) {
				P_kplus_k[i][j] += phik[i][k] * P_kplus_k_buff[k][j];
			}
		}
	}

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			P_kplus_k[i][j] = P_kplus_k[i][j] + Q_k[i][j];
		}
	}

	/*Calculate Kalman Gain K, K_kplus=P_kplus_k*H'*(H*P_kplus_k*H'+Rk)^-1*/

	//Calculate H^T first
	double H_T[9][6] = {{0}};
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 6; j++) {
			H_T[i][j] = Kalman.MK.H[j][i];
		}
	}

	//Calculate P_kplus_k*H'
	double K_kplus_k_buff_1[9][6] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 6; j++) {
			for (k = 0; k < 9; k++) {
				K_kplus_k_buff_1[i][j] += P_kplus_k[i][k] * H_T[k][j];
			}
		}
	}

	//Calculate H*P_kplus_k*H', H*K_kplus_k_buff_1
	double K_kplus_k_buff_2[6][6] = {{0}};
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 6; j++) {
			for (k = 0; k < 9; k++) {
				K_kplus_k_buff_2[i][j] += Kalman.MK.H[i][k] * K_kplus_k_buff_1[k][j];
			}
		}
	}

	//Calculate H*P_kplus_k*H', K_kplus_k_buff_2*H'+Rk
	double K_kplus_k_buff_3[6][6] = {{0}};
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 6; j++) {
			K_kplus_k_buff_3[i][j] = K_kplus_k_buff_2[i][j] + Kalman.MK.R[i][j];
		}
	}

//	//Now calculate (H*P_kplus_k*H'+Rk)^-1, the inverse of K_kplus_k_buff_3
	double matrix[6][12] = {{0}};
	double inv_matrix[6][6] = {{0}};
	int n = 6;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			matrix[i][j] = K_kplus_k_buff_3[i][j];
		}
	}

	for (i = 0; i < n; i++) {
		for (j = n; j < 2 * n; j++) {
			if (i == (j - n))
				matrix[i][j] = 1.0;
			else
				matrix[i][j] = 0.0;
		}
	}

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (i != j) {
				float ratio = matrix[j][i] / matrix[i][i];
				for (k = 0; k < 2 * n; k++) {
					matrix[j][k] -= ratio * matrix[i][k];
				}
			}
		}
	}

	float l;
	for (i = 0; i < n; i++) {
		l = matrix[i][i];
		for (j = 0; j < 2 * n; j++) {
			matrix[i][j] /= l;
		}
	}

	for (i = 0; i < n; i++) {
		k = 0;
		for (j = n; j < 2 * n; j++) {
			inv_matrix[i][k] = matrix[i][j];
			k += 1;
		}

	}

	/*Calculate K_kplus = K_kplus_k_buff_1 * inv_matrix */
	double K_kplus[9][6] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 6; j++) {
			for (k = 0; k < 6; k++) {
				K_kplus[i][j] += K_kplus_k_buff_1[i][k] * inv_matrix[k][j];
			}
		}
	}

	/*Estimate the MKariance matrix P_kplus_kplus=(eye(9)-K_kplus*H)*P_kpl 	us_k;*/

	//Calculate K_kplus*H
	double P_kplus_kplus_buff1[9][9] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			for (k = 0; k < 6; k++) {
				P_kplus_kplus_buff1[i][j] += K_kplus[i][k] * Kalman.MK.H[k][j];
			}
		}
	}

	//Calculate eye(9)-K_kplus*H
	for (i = 0; i < 9; i++) {
		P_kplus_kplus_buff1[i][i] = 1 - P_kplus_kplus_buff1[i][i];
	}

	//Calculate (eye(9)-K_kplus*H)*P_kplus_k, P_kplus_kplus_buff1*P_kplus_k
	double P_kplus_kplus[9][9] = {{0}};

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			for (k = 0; k < 9; k++) {
				P_kplus_kplus[i][j] += P_kplus_kplus_buff1[i][k]
						* P_kplus_k[k][j];
			}
		}
	}

	/*Now estimate x, x_kplus_k = phik  x_k_k */
	double x_kplus_k[9][1] = {{0}};
/*
	for (j = 0; j < 9; j++) {
				for (k = 0; k < 1; k++) {
					Kalman.OUT.X_k_k[j][k] = 0;
				}
			}
*/
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 1; j++) {
			for (k = 0; k < 9; k++) {
				x_kplus_k[i][j] += phik[i][k] * Kalman.OUT.X_k_k[k][j];
			}
		}
	}

//		for (i = 0; i < 3; i++){
//			for (j = 0; j < 3; j++){
//				printf("%3.4f \r\n", phik[i][j]);
//			}
//		}

	/*Estimate Z, z_kplus = H*x_kplus_k*/

	double z_kplus[6][1] = {{0}};

	for (i = 0; i < 6; i++) {
		for (j = 0; j < 1; j++) {
			for (k = 0; k < 9; k++) {
				z_kplus[i][j] += Kalman.MK.H[i][k] * x_kplus_k[k][j];
			}
		}
	}

	/*Calculate the delta z_kplus_k = i.e our measurement vector.*/
	double del_z_kplus_k[6][1] = {{0}};
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 1; j++) {
			del_z_kplus_k[i][j] = z_kplus_kplus[i][j] - z_kplus[i][j];
		}
	}

	/*Calculate the x_kplus_kplus = i.e our Kalman correction vector. x_kplus_kplus = x_kplus_k+K_kplus*del_z_kplus_k*/

	//calculate K_kplus*del_z_kplus_k
	double x_kplus_kplus_buff[9][1] = {{0}};
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 1; j++) {
			for (k = 0; k < 6; k++) {
				x_kplus_kplus_buff[i][j] += K_kplus[i][k] * del_z_kplus_k[k][j];
			}
		}
	}

	//Calculate x_kplus_kplus = x_kplus_k+K_kplus*del_z_kplus_k,	x_kplus_k+x_kplus_kplus_buff
	double x_kplus_kplus[9][1] = {{0}};
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 1; j++) {
			x_kplus_kplus[i][j] = x_kplus_k[i][j] + x_kplus_kplus_buff[i][j];
		}
	}

	/*Store values for next iteration*/
	//Copy values into new x_k_k
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 1; j++) {
			if(abs(x_kplus_kplus[i][j]) > 1){
				Kalman.OUT.X_k_k[i][j] = 0;
			}else{
				Kalman.OUT.X_k_k[i][j] = x_kplus_kplus[i][j]/10;
			}
		}
	}
	//Copy values into new P
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (abs(P_kplus_kplus[i][j]) > 1){
				Kalman.OUT.P_k_k[i][j] = 0;
			} else{
				Kalman.OUT.P_k_k[i][j] = P_kplus_kplus[i][j]/10;
			}
		}
	}

	//delta for position
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 1; j++) {
			if(abs(x_kplus_kplus[i][j]) > 1){
				Kalman.OUT.del_err[i] = 0;
			}else{
				Kalman.OUT.del_err[i] = x_kplus_kplus[i][j]/10;
			}

		}
	}

	//delta for velocity
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 1; j++) {
			if(abs(x_kplus_kplus[i + 3][j]) > 1){
				Kalman.OUT.del_err[i + 3] = 0;
			}{
				Kalman.OUT.del_err[i + 3] = x_kplus_kplus[i + 3][j]/10;
			}
		}
	}

	//delta for error.
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 1; j++) {
			if(abs(x_kplus_kplus[i + 6][j]) > 1){
				Kalman.OUT.del_err[i + 6] = 0;
			} else{
				Kalman.OUT.del_err[i + 6] = x_kplus_kplus[i + 6][j]/10;
			}
		}
	}
}


