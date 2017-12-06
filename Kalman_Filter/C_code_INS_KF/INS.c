

#include "INS.h"
#include "Kalman_Filter.h"
struct INSstate INS;
extern SensorState_t sensor;
/*
 * writes the values from accDataRX into the accValues structure
 */
void INS_Init(void){

	INS.ACC.X = &sensor.imu.ax;
	INS.ACC.Y = &sensor.imu.ay;
	INS.ACC.Z = &sensor.imu.az;

	INS.GYR.X = &sensor.imu.gx;
	INS.GYR.Y = &sensor.imu.gy;
	INS.GYR.Z = &sensor.imu.gz;

	INS.MAG.X = &sensor.imu.mx;
	INS.MAG.Y = &sensor.imu.my;
	INS.MAG.Z = &sensor.imu.mz;

	INS.GPS.pos_lat = &sensor.gps.pos.latitude_rad;
	INS.GPS.pos_long = &sensor.gps.pos.longitude_rad;
	INS.GPS.pos_h = &sensor.gps.pos.elevation;

	INS.GPS.vel_lat = &sensor.gps.pos.latitude_vel;
	INS.GPS.vel_long = &sensor.gps.pos.longitude_vel;

	INS.GPS.old_gps[0] = 0;
	INS.GPS.old_gps[1] = 0;

	INS.POS_VEL.r_n[0] = 0.0;	//position in all direction are zero at initial
	INS.POS_VEL.r_n[1] = 0.0;
	INS.POS_VEL.r_n[2] = 0.0;

	INS.POS_VEL.v_n[0] = 0; //Velocity in all direction are set to zero as initial
	INS.POS_VEL.v_n[1] = 0;
	INS.POS_VEL.v_n[2] = 0;

	INS.POS_VEL.del_err[0] = &Kalman.OUT.del_err[0];	//The error between INS and GPS for position.
	INS.POS_VEL.del_err[1] = &Kalman.OUT.del_err[1];
	INS.POS_VEL.del_err[2] = &Kalman.OUT.del_err[2];

	INS.POS_VEL.del_err[3] = &Kalman.OUT.del_err[3]; //The error between INS and GPS for velocity.
	INS.POS_VEL.del_err[4] = &Kalman.OUT.del_err[4];
	INS.POS_VEL.del_err[5] = &Kalman.OUT.del_err[5];

	INS.POS_VEL.del_err[6] = &Kalman.OUT.del_err[6]; //The error in skew matrix.
	INS.POS_VEL.del_err[7] = &Kalman.OUT.del_err[7];
	INS.POS_VEL.del_err[8] = &Kalman.OUT.del_err[8];

	INS.POS_VEL.r_n_old[0] = 0;
	INS.POS_VEL.r_n_old[1] = 0;
	INS.POS_VEL.v_n_old[0] = 0;
	INS.POS_VEL.v_n_old[1] = 0;

	INS.POS_VEL.quat[0] = &sensor.imu.q0;
	INS.POS_VEL.quat[1] = &sensor.imu.q1;
	INS.POS_VEL.quat[2] = &sensor.imu.q2;
	INS.POS_VEL.quat[3] = &sensor.imu.q3;











	/* 0->5873   -0->6187    0->5217
    0->8093    0->4490   -0->3786
         0    0->6447    0->7645*/

	float C_b_n_pre[3][3];
	C_b_n_pre[0][0] = 0.5873f;
	C_b_n_pre[0][1] = -0.6187f;
	C_b_n_pre[0][2] = 0.5217f;
	C_b_n_pre[1][0] = 0.8093f;
	C_b_n_pre[1][1] = 0.4490f;
	C_b_n_pre[1][2] = -0.3786f;
	C_b_n_pre[2][0] = 0.0f;
	C_b_n_pre[2][1] = 0.6447f;
	C_b_n_pre[2][2] = 0.7645;
}

void INS_Update() {




	float del_INS_time = 0.01f;
	int i, j, k;
	float q1, q2, q3, q4;
	q1 = *INS.POS_VEL.quat[0];
	q2 = *INS.POS_VEL.quat[1];
	q3 = *INS.POS_VEL.quat[2];
	q4 = *INS.POS_VEL.quat[3];


	//printf("%3->4f, %3->4f, %3->4f, %3->4f \r\n ", q1, q2, q3 ,q4);



	float acc_buff[3];
	acc_buff[0] = (*INS.ACC.X) - ACC_X_OFFSET;
	acc_buff[1] = (*INS.ACC.Y) - ACC_Y_OFFSET;
	acc_buff[2] = (*INS.ACC.Z) - ACC_Z_OFFSET;

	float acc_b[3];
	acc_b[0] = acc_buff[0] / (1+ACC_X_SCALE);
	acc_b[1] = acc_buff[1] / (1+ACC_Y_SCALE);
	acc_b[2] = acc_buff[2] / (1+ACC_Z_SCALE);

	float om_b_ib[3];
	om_b_ib[0] = *INS.GYR.X;
	om_b_ib[1] = *INS.GYR.Y;
	om_b_ib[2] = *INS.GYR.Z;

	/*Define some vectors for futher use*/
	float del_r_n[3];	//will be the Kalman correction for position
	float del_v_n[3];	//will be tha Kalman correction for velocity

	/*KOLLA UPP DETTA, EJ SÄKERT DETTA FUNGERAR*/
//	kal->theta_error = GPS_INS->pitch_euler;
//	kal->phi_error = GPS_INS->roll_euler;
//	kal->psi_error = GPS_INS->yaw_euler;


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

	/*See if we have any new update for GPS*/
	//if(*INS.GPS.pos_lat != INS.GPS.old_gps[0] || *INS.GPS.pos_long != INS.GPS.old_gps[1]){
		del_r_n[0] = Kalman.OUT.del_err[0];		//Kalman Correction for position
		del_r_n[1] = Kalman.OUT.del_err[1];
		del_r_n[2] = Kalman.OUT.del_err[2];

		del_v_n[0] = Kalman.OUT.del_err[3];	//Kalman Correction for Velocity
		del_v_n[1] = Kalman.OUT.del_err[4];
		del_v_n[2] = Kalman.OUT.del_err[5];
		for(i = 0 ; i < 3; i++){
			INS.POS_VEL.r_n[i] -= del_r_n[i];
			INS.POS_VEL.v_n[i] -= del_v_n[i];
		}

		INS.GPS.old_gps[0] = *INS.GPS.pos_lat;
		INS.GPS.old_gps[1] = *INS.GPS.pos_long;


		float eps_n = Kalman.OUT.del_err[6];
		float eps_e = Kalman.OUT.del_err[7];
		float eps_d = Kalman.OUT.del_err[8];

		/*   E=[0 -eps_d eps_e;
			eps_d 0 -eps_n;
			eps_e eps_n 0];
		*/
		double E[3][3] = {{0}};
		E[0][0] = 0;
		E[0][1] = -eps_d;
		E[0][2] = eps_e;
		E[1][0] = eps_d;
		E[1][1] = 0;
		E[1][2] = -eps_n;
		E[2][0] = -eps_e;
		E[2][1] = eps_n;
		E[2][2] = 0;

		/*Calculate the error in C_n_b=(eye(3)+C_n_b)*E*/
//		float roll = DEG_TO_RAD * (GPS_INS -> roll_euler);		//phi
//		float pitch = DEG_TO_RAD * (GPS_INS -> pitch_euler);	//theta
//		float yaw = DEG_TO_RAD * (GPS_INS -> yaw_euler);		//psi
//		float ct = cos(pitch);
//		float cpsi = cos(yaw);
//		float cphi = cos(roll);
//		float st = sin(pitch);
//		float spsi = sin(yaw);
//		float sphi = sin(roll);
//


//		C_n_b= [(q1^2-q2^2-q3^2+q4^2), 2*(q1*q2-q3*q4), 2*(q1*q3+q2*q4);
//		    2*(q1*q2+q3*q4), (q2^2-q1^2-q3^2+q4^2), 2*(q2*q3-q1*q4);
//		    2*(q1*q3-q2*q4), 2*(q2*q3+q1*q4), (q3^2-q1^2-q2^2+q4^2)
//		];


//		printf("%3->4f \r\n", q1);
//	 	printf("%3->4f \r\n", q2);
//	 	printf("%3->4f \r\n", q3);
/*
		double C_buff[3][3] = {{0}};

		for (i = 0; i < 3; i++){
			C_buff[i][i] = 1 + C_n_b[i][i];
		}

		for (i = 0; i < 3; i++){
			for (j = 0; j < 3; j++){
				for (k = 0; k < 3; k++){
					 C_n_b[i][j] += C_buff[i][k] * E[k][j];
				}
			}
		}
*/
//		q(4,1) = 0->5*sqrt(abs(1 + C(1,1) + C(2,2) + C(3,3)));
//		q(1,1) = 1/(4*q(4,1))*(C(3,2) - C(2,3));
//		q(2,1) = 1/(4*q(4,1))*(C(1,3) - C(3,1));
//		q(3,1) = 1/(4*q(4,1))*(C(2,1) - C(1,2));

//		q4 = 0->5*sqrt(abs(1 + C_n_b[0][0] + C_n_b[1][1] + C_n_b[2][2]));
////		printf("%3->4f \r\n", q4);
//
//		q1 = 1/(4 * q4) * (C_n_b[2][1] - C_n_b[1][2]);
//		q2 = 1/(4 * q4) * (C_n_b[0][2] - C_n_b[2][0]);
//		q3 = 1/(4 * q4) * (C_n_b[1][0] - C_n_b[0][1]);






		/*Calculate the new roll, pitch and yaw*/
//		double x_buff1 = C_n_b[2][0]/(sqrt(1-C_n_b[2][0]*C_n_b[2][0]));
//		kal->theta_error = atan(x_buff1);
//		kal->phi_error = atan2(C_n_b[2][1], C_n_b[2][2]);
//		kal->psi_error = atan2(C_n_b[1][0], C_n_b[0][0]);

	//} //end if statement

	float phi, lam, h;
	phi = INS.POS_VEL.r_n[0];
	lam = INS.POS_VEL.r_n[1];
 	h = INS.POS_VEL.r_n[2];
	//printf("phi %3->4f, lam %3->4f, h %3->4f \r\n", phi, lam, h);

	/* Set some variables->*/
	float om_e_ie[3][1] = {{0}};
	om_e_ie[0][0] = 0;
	om_e_ie[1][0] = 0;
	om_e_ie[2][0] = om_e;


	//double e_buff = (2*F)-(F*F);
	double e = 0.0818f;//sqrt(e_buff);
//	printf("%3->4f \r\n", e_buff);


	/*Define constant to velocity matrix, (NOT assuming Earth to be perfect spherical)*/
	float M_buff1 = (1-e*e);
	float M_buff2 = (1-e*e*sin(phi)*sin(phi));
	float M_buff3 = pow(M_buff2, 1.5f);
	float M = a*M_buff1/M_buff3;
//	printf("%3->4f \r\n", M);
//	printf("%3->4f \r\n", phi);

	float N_buff1 = a;
	float N_buff2 = sqrt(M_buff2);
	float N = N_buff1/N_buff2;
//	printf("%3->4f \r\n", N);

	/*Define DCM Earth to navigation frame*/
	/*
	 *	C_n_e=[-sin(phi)*cos(lam), -sin(phi)*sin(lam), cos(phi);
	 *   		-sin(lam), cos(lam), 0;
	 *  		-cos(phi)*cos(lam), -cos(phi)*sin(lam), -sin(phi)];
	*/
	double C_n_e[3][3];
	C_n_e[0][0] = -sin(phi) * cos(lam);
	C_n_e[0][1] = -sin(phi) * sin(lam);
	C_n_e[0][2] = cos(phi);
	C_n_e[1][0] = -sin(lam);
	C_n_e[1][1] = cos(lam);
	C_n_e[1][2] = 0;
	C_n_e[2][0] = -cos(phi)*cos(lam);
	C_n_e[2][1] = -cos(phi)*sin(lam);
	C_n_e[2][2] = -sin(phi);
//	for (i = 0; i < 3; i++){
//		for (j = 0; j < 3; j++){
//			printf("%3->4f \r\n", C_n_e[i][j]);
//		}
//	}

	/*Velocity to position*/
	double vel2pos[3][3] = {{0}};

	double vel2pos_buff = (N+h)*cos(phi);
	vel2pos[0][0] = 1/(M + h);
	vel2pos[1][1] = 1/(vel2pos_buff);
	vel2pos[2][2] = 1.0f;
//	printf("%3->4f \r\n", vel2pos[0][0]);
//	printf("%3->4f \r\n", vel2pos[1][1]);
//	printf("%3->4f \r\n", vel2pos[2][2]);

	/*Direction from velocity*/
	double dr_n[3][3] = {{0}};

	dr_n[0][0] = vel2pos[0][0] * INS.POS_VEL.v_n[0];
	dr_n[1][1] = vel2pos[1][1] * INS.POS_VEL.v_n[1];
	dr_n[2][2] = vel2pos[2][2] * INS.POS_VEL.v_n[2];

	/*Create new constants so nothing is messed up from report->*/
	double dphi = dr_n[0][0];
	double dlam = dr_n[1][1];


	double om_n_en[3][1] = {{0}};
	om_n_en[0][0] = dlam*cos(phi);
	om_n_en[1][0] = -dphi;
	om_n_en[2][0] = -dlam*sin(phi);

	double om_n_ie[3][1] = {{0}};
	for (i = 0; i < 3; i++){
		for (j = 0; j < 1; j++){
			for (k = 0; k < 3; k++){
				om_n_ie[i][j] += C_n_e[i][k] * om_e_ie[k][j];
			}
		}
	}
//	for (j = 0; j < 3; j++){
//			printf("%3->4f, %3->4f, %3->4f \r\n", C_n_e[j][0], C_n_e[j][1], C_n_e[j][2]);
//		}
//
//		for (j = 0; j < 3; j++){
//			printf("%3->4f \r\n", om_n_ie[i][j]);
//		}
	double om_n_in[3][1] = {{0}};
	double om_n_in_sing[3][1] = {{0}};
	for (i = 0; i < 3; i++){
		om_n_in[i][0] = om_n_en[i][0] + 2*om_n_ie[i][0];
		om_n_in_sing[i][0] = om_n_en[i][0] + om_n_ie[i][0];
	}



	/*Calculate new DCM from theta error*/
//	float ct = cos(kal->theta_error); 	//pitch
//	float cpsi = cos(kal->psi_error);	//yaw
//	float cphi = cos(kal->phi_error);	//roll
//	float st = sin(kal->theta_error);	//pitch
//	float spsi = sin(kal->psi_error);	//yaw
//	float sphi = sin(kal->phi_error);	//roll


/*
	C_n_b[0][0] = q1*q1 - q2*q2 - q3*q3 + q4*q4;
	C_n_b[0][1] = 2*(q1*q2-q3*q4);
	C_n_b[0][2] = 2*(q1*q3+q2*q4);
	C_n_b[1][0] = 2*(q1*q2+q3*q4);
	C_n_b[1][1] = (q2*q2 - q1*q2 - q3*q3 + q4*q4);
	C_n_b[1][2] = 2*(q2*q3-q1*q4);
	C_n_b[2][0] = 2*(q1*q3-q2*q4);
	C_n_b[2][1] = 2*(q2*q3+q1*q4);
	C_n_b[2][2] = (q3*q3 - q1*q1 - q2*q2 + q4*q4);
*/

//	for (i = 0; i < 3; i++) {
//		for (j = 0; j < 3; j++) {
//			printf("%3->4f \r\n", C_n_b[i][j]);
//		}
//		printf("q0 = %3->4f, q1 =  %3->4f, q3 = %3->4f, q4 = %3->4f \r\n", q1, q2, q3, q4);
//	}


	/*Calculate transpose of C_n_b which is C_b_n*/
	double C_b_n[3][3];
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			C_b_n[i][j] = C_n_b[j][i];
		}
	}



	double om_b_in[3][1] = {{0}};
	for (i = 0; i < 3; i++){
		for (j = 0; j < 1; j++){
			for (k = 0; k < 3; k++){
				om_b_in[i][j] += C_b_n[i][k] * om_n_in_sing[k][j];
			}
		}
	}

	/*Updating values for next iteration*/
	double del_theta_b_nb_buff1[3][1];
	double del_theta_b_nb_buff2[3][1];
	double del_theta_b_nb[3][1];

	for (j = 0; j < 3; j++){
		del_theta_b_nb[j][0] = 0;
		del_theta_b_nb_buff2[j][0] = 0;
		del_theta_b_nb_buff1[j][0] = 0;
	}


	for (i = 0; i < 3; i++){
		for (j = 0; j < 1; j++){
			for (k = 0; k < 3; k++){
				del_theta_b_nb_buff1[i][j] += C_b_n[i][k] * om_n_in_sing[k][j] * del_INS_time;
			}
		}
	}
	//printf("t = %3->4f, t1 =  %3->4f, t2 = %3->4f \r\n",del_theta_b_nb_buff1[0][0],del_theta_b_nb_buff1[1][0], del_theta_b_nb_buff1[2][0]);

	del_theta_b_nb_buff2[0][0] = *INS.GYR.X*del_INS_time;
	del_theta_b_nb_buff2[1][0] = *INS.GYR.Y*del_INS_time;
	del_theta_b_nb_buff2[2][0] = *INS.GYR.Z*del_INS_time;

	//printf("X = %3->4f, Y =  %3->4f, Z = %3->4f \r\n",INS->GYR->X*del_INS_time, INS->GYR->Y*del_INS_time, INS->GYR->Z*del_INS_time );


	for (i = 0; i < 3; i++){
		del_theta_b_nb[i][0] = del_theta_b_nb_buff2[i][0] - del_theta_b_nb_buff1[i][0];
	}
	//printf("T1 = %3->4f, T2 =  %3->4f, T3 = %3->4f \r\n",del_theta_b_nb[0][0], del_theta_b_nb[1][0], del_theta_b_nb[2][0]);

	double del_theta_x = del_theta_b_nb[0][0];
	double del_theta_y = del_theta_b_nb[1][0];
	double del_theta_z = del_theta_b_nb[2][0];
	double del_theta = sqrt(del_theta_b_nb[0][0]*del_theta_b_nb[0][0] +
			del_theta_b_nb[1][0]*del_theta_b_nb[1][0] + del_theta_b_nb[2][0]*del_theta_b_nb[2][0]);
	//printf("t = %3->4f, t1 =  %3->4f, t2 = %3->4f \r\n", del_theta, del_theta_z, del_theta_y);


	double del_v_b_n[3][1] = {{0}};
	for (i = 0; i < 3; i++){
		del_v_b_n[i][0] = - acc_b[i] * del_INS_time;
	}
	//printf("v_b_n0 = %3->4f, v_b_n1 =  %3->4f, v_b_n2 = %3->4f \r\n", del_v_b_n[0][0], del_v_b_n[1][0], del_v_b_n[2][0]);


	double sculling[3][3] = {{0}};
	sculling[0][0] = 1;
	sculling[0][1] = del_theta_z/2 ;
	sculling[0][2] = -del_theta_y/2;
	sculling[1][0] = -del_theta_z/2;
	sculling[1][1] = 1;
	sculling[1][2] = del_theta_x/2;
	sculling[2][0] = del_theta_y/2;
	sculling[2][1] = -del_theta_x/2;
	sculling[2][2] = 1;

//	for (j = 0; j < 3; j++){
//		printf("scull0 = %3->4f, scull =  %3->4f, scull = %3->4f \r\n", sculling[0][j], sculling[0][j], sculling[0][j]);
//	}

	double del_v_n_f_buff[3][3] = {{0}};

	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			for (k = 0; k < 3; k++){
				del_v_n_f_buff[i][j] += C_n_b[i][k] * sculling[k][j];
			}
		}
	}
//	for (j = 0; j < 3; j++){
//			printf("del_v_n_f = %3->4f, del_v_n_f =  %3->4f, del_v_n_f = %3->4f \r\n", del_v_n_f_buff[0][j], del_v_n_f_buff[0][j], del_v_n_f_buff[0][j]);
//	}


	double del_v_n_f[3][1] = {{0}};

	for (i = 0; i < 3; i++){
		for (j = 0; j < 1; j++){
			for (k = 0; k < 3; k++){
				del_v_n_f[i][j] += del_v_n_f_buff[i][k] * del_v_b_n[k][j];
			}
		}
	}

//	for (j = 0; j < 3; j++){
//			printf("del_v_n_f = %3->4f, del_v_n_f =  %3->4f, del_v_n_f = %3->4f \r\n", del_v_n_f[0][j], del_v_n_f[1][j], del_v_n_f[2][j]);
//	}
	//cross(2*om_n_ie+om_n_en),v_nINS_del_t)
	double cross_v[3][1] = {{0}};
	cross_v[0][0] = om_n_in[1][0] * INS.POS_VEL.v_n[2]*del_INS_time - om_n_in[2][0] * INS.POS_VEL.v_n[1]*del_INS_time;
	cross_v[1][0] = -om_n_in[0][0] * INS.POS_VEL.v_n[2]*del_INS_time + om_n_in[2][0] * INS.POS_VEL.v_n[0]*del_INS_time;
	cross_v[2][0] = om_n_in[0][0] * INS.POS_VEL.v_n[1]*del_INS_time - om_n_in[1][0] * INS.POS_VEL.v_n[0]*del_INS_time;
//	printf("%3.4f \r\n", cross_v[0][0]);
//	printf("%3->4f \r\n", cross_v[1][0]);
//	printf("%3->4f \r\n", cross_v[2][0]);
	//printf("cross = %3->4f, cross =  %3->4f, cross = %3->4f \r\n", cross_v[0][0], cross_v[1][0], cross_v[2][0]);

	del_v_n[0] = del_v_n_f[0][0] - cross_v[0][0];
	del_v_n[1] = del_v_n_f[1][0] - cross_v[1][0];
	del_v_n[2] = del_v_n_f[2][0] - cross_v[2][0];


	INS.POS_VEL.v_n_old[0] = INS.POS_VEL.v_n[0];
	INS.POS_VEL.v_n_old[1] = INS.POS_VEL.v_n[1];
	INS.POS_VEL.v_n_old[2] = INS.POS_VEL.v_n[2];

	INS.POS_VEL.v_n[0] = INS.POS_VEL.v_n[0] + del_v_n[0];
	INS.POS_VEL.v_n[1] = INS.POS_VEL.v_n[1] + del_v_n[1];
	INS.POS_VEL.v_n[2] = INS.POS_VEL.v_n[2] + del_v_n[2];

	//r_n=r_n+0.5*vel2pos*(v_n_new+v_n)INS_del_t;
	INS.POS_VEL.r_n[0] = INS.POS_VEL.r_n[0] + 0.5*(INS.POS_VEL.v_n[0] + INS.POS_VEL.v_n_old[0])*vel2pos[0][0]*del_INS_time;
	INS.POS_VEL.r_n[1] = INS.POS_VEL.r_n[1] + 0.5*(INS.POS_VEL.v_n[1] + INS.POS_VEL.v_n_old[1])*vel2pos[1][1]*del_INS_time;
	INS.POS_VEL.r_n[2] = INS.POS_VEL.r_n[2] + 0.5*(INS.POS_VEL.v_n[2] + INS.POS_VEL.v_n_old[2])*vel2pos[2][2]*del_INS_time;


/*	double s=2/del_theta*sin(del_theta/2);
	double c=2*(cos(del_theta/2)-1);
	double q_del_theta_mat[4][4];

	q_del_theta_mat[0][0] = c;
	q_del_theta_mat[0][1] = s*del_theta_z;
	q_del_theta_mat[0][2] = -s*del_theta_y;
	q_del_theta_mat[0][3] = s*del_theta_x;;
	q_del_theta_mat[1][0] = -s*del_theta_z;
	q_del_theta_mat[1][1] = c;
	q_del_theta_mat[1][2] = s*del_theta_x;
	q_del_theta_mat[1][3] = s*del_theta_y;
	q_del_theta_mat[2][0] = -s*del_theta_x;
	q_del_theta_mat[2][1] = -s*del_theta_y;
	q_del_theta_mat[2][2] = -s*del_theta_z;
	q_del_theta_mat[2][3] = c;



	double q_buff_1[4][1] = {{0}};
	q_buff_1[0][0] = q1;
	q_buff_1[1][0] = q2;
	q_buff_1[2][0] = q3;
	q_buff_1[3][0] = q4;

	double q_buff_2[4][1] = {{0}};

	for (i = 0; i < 4; i++){
		for (j = 0; j < 1; j++){
			for (k = 0; k < 4; k++){
				q_buff_2[i][j] += q_del_theta_mat[i][k] * q_buff_1[k][j];
			}
		}
	}*/
//	printf("q_buff_2 = %3->4f, q_buff_2 =  %3->4f, q_buff_2 = %3->4f, q_buff_2 = %3->4f  \r\n", q_buff_2[0][0], q_buff_2[1][0], q_buff_2[2][0], q_buff_2[3][0]);


//	q1 += 0->5*q_buff_2[0][0];
//	q2 += 0->5*q_buff_2[1][0];
//	q3 += 0->5*q_buff_2[2][0];
//	q4 += 0->5*q_buff_2[3][0];
//	printf("%3->4f \r\n", q1);
// 	printf("%3->4f \r\n", q2);
// 	printf("%3->4f \r\n", q3);
// 	printf("%3->4f \r\n", q4);


	//float q_norm = sqrt(q1*q1 + q2*q2 + q3*q3 + q4*q4 );
//	printf("%3->4f \r\n", q_norm);

//	q1 = q1/q_norm;
//	q2 = q2/q_norm;
//	q3 = q3/q_norm;
//	q4 = q4/q_norm;

/*
	INS.POS_VEL.del_r_n[0][0] = del_r_n[0];
	INS.POS_VEL.del_r_n[1][0] = del_r_n[1];;
	INS.POS_VEL.del_r_n[2][0] = del_r_n[2];;

	INS.POS_VEL.del_v_n[0][0] = del_v_n[0];
	INS.POS_VEL.del_v_n[1][0] = del_v_n[1];
	INS.POS_VEL.del_v_n[2][0] = del_v_n[2];
*/
	//printf("r_n_x %3.4f, r_n_y %3.4f, r_n_z %3.4f \r\n", INS.POS_VEL.r_n[0], INS.POS_VEL.r_n[1], INS.POS_VEL.r_n[2]);
	//printf("v_n_x %3.4f, v_n_y %3.4f, v_n_z %3.4f \r\n", INS.POS_VEL.v_n[0], INS.POS_VEL.v_n[1], INS.POS_VEL.v_n[2]);
// 	printf("%3->4f \r\n", INS->POS_VEL->del_v_n[1][0]);
// 	printf("%3->4f \r\n", INS->POS_VEL->del_v_n[2][0]);
//
//
//	printf("%3->4f \r\n", q1);
// 	printf("%3->4f \r\n", INS->POS_VEL->quat[1]);
// 	printf("%3->4f \r\n", INS->POS_VEL->quat[2]);
}


