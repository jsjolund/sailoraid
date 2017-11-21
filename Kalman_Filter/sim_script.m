%READ THE README FILE
% variable format
% om_b_ei - angular velocity om described in reference frame b(ody) 
% measuring the angular speed between e(arth) and i(nertial) framess 
% C_b_n - transformation matrix between n(avigation) and b
% f_b - force measured in b



clear
clear functions
clc
tic

 %THERE WILL BE THINGS IN THE BOTTOM WHICH HAS TO BE %HAS TO BE COMMENT OUT

traj_gen %HAS TO BE COMMENT OUT IF ONE USES REAL DATA

%use acquired (real-life) data 

%addpath(genpath('\data'));
%data_script %HAS TO BE COMMENT OUT IF ONE USES REAL DATA

%assign initial values
data(1:3,1,1) = r_n_0;
data_q(:,1) = q_0;
data(1:3,1,3) = v_n_0;

%GPS_acquired=0;
kalman_correction=zeros(9,1);

%iterating through the time list

last_ins = 0;
last_gps = 0;

for i=1:(size_t-1)
    
    ins_del_t = (time(i)-last_ins);
    last_ins=time(i);
    
    [ r_n_ins , v_n_ins, q ] = ...
    INS_mechanisation( acc_b(:,i), om_b_ib(:,i), r_n_0, v_n_0, q_0, ins_del_t,...
    gps_acquired(i), kalman_correction);


    if gps_acquired(i+1)%r_n_gps(:,i+1)~=r_n_gps(:,i)
        gps_del_t = (time(i)-last_gps);
        %debug=[debug gps_del_t];
        last_gps=time(i);
        %GPS_acquired=1;
        [kalman_correction,del_r_n,del_v_n] = kalman_gps_ins(...,
            r_n_gps(:,i+1), v_n_gps(:,i+1), r_n_ins, v_n_ins, acc_b, q,...
            ins_del_t, gps_del_t,acc_noise, acc_bias, gyro_noise,...
            r_gps_noise, v_gps_noise);
    end
    
    if( i~= size_t )
        data(1:3,i+1,1) = r_n_ins;
        data_q(:,i+1) = q;
        data(1:3,i+1,3) = v_n_ins;
        data(1:3,i,9) = del_r_n;  
        data(1:3,i,10) = del_v_n;
        %data(1:3,i,11) = kalman_correction(7:9);%del_eps
    end
    
    data(1:3,i,11)=r_n_gps(:,i+1);
    data(1:3,i,12)=v_n_gps(:,i+1);
end

toc

%post-process for simulated data
postpro %HAS TO BE COMMENT OUT IF ONE USES REAL DATA


%post-process for real - life data
%postpro_data %HAS TO BE COMMENT OUT IF ONE USES REAL DATA
