%time = ins_del_t:ins_del_t:(ins_del_t*(size_t-1));
tspan=size_t-1;

%plotting r_n trajectory
% subplot(2,2,1)
% plot3(6*10^6*(data(1,1:tspan,1)-data(1,1,6)), ... 
%     6*10^6*(data(2,1:tspan,1)-data(2,1,6)), ...
%     data(3,1:tspan,1)-data(3,1,6),...
%     6*10^6*(data(1,1:tspan,6)-data(1,1,6)),...%+wgn(1,size_t,r_gps_noise^2,'linear'),...
%     6*10^6*(data(2,1:tspan,6)-data(2,1,6)),...
%     data(3,1:tspan,6)-data(3,1,6));%+wgn(1,size_t,r_gps_noise^2,'linear'));
% axis equal
% title('Filtered trajectory')
% ylabel('Distance [m]')
% xlabel('Distance [m]')
% zlabel('Relative altitude [m]')


x_perf=data(1,1:tspan,1)-data(1,1:tspan,6);
x_perf=x_perf*x_perf';

y_perf=data(2,1:tspan,1)-data(2,1:tspan,6);
y_perf=y_perf*y_perf';

kalman_perf=x_perf+y_perf;

disp('Kalman filter performance (the smaller the better):')
disp(kalman_perf);

subplot(2,2,2)
%plot(t_mem,data(2,:,9)*10^6*6)

plot3(6*10^6*(data(1,1:tspan,11)-data(1,1,6)), ... 
    6*10^6*(data(2,1:tspan,11)-data(2,1,6)), ...
    data(3,1:tspan,11)-data(3,1,6),...
    6*10^6*(data(1,1:tspan,6)-data(1,1,6)),...%+wgn(1,size_t,r_gps_noise^2,'linear'),...
    6*10^6*(data(2,1:tspan,6)-data(2,1,6)),...
    data(3,1:tspan,6) - data(3,1,6));%+wgn(1,size_t,r_gps_noise^2,'linear'));
axis equal

title('GPS trajectory')
ylabel('Distance [m]')
xlabel('Distance [m]')
zlabel('Relative altitude [m]')

x_perf=data(1,1:tspan,11)-data(1,1:tspan,6);
x_perf=x_perf*x_perf';

y_perf=data(2,1:tspan,11)-data(2,1:tspan,6);
y_perf=y_perf*y_perf';

gps_perf=x_perf+y_perf;

disp('GPS navigation filtering performance:')
disp(gps_perf);

subplot(2,2,3)

plot(time(1:tspan),data(3,1:tspan,1))
title('Altitude')
ylabel('Distance [m]')
xlabel('Distance [m]')

subplot(2,2,4)

plot(time(1:tspan),data(3,1:tspan,3))
title('Vertical velocity')
ylabel('Distance [m]')
xlabel('Distance [m]')
% 
{
subplot(2,2,1)
plot(6*10^6*(data(1,:,1)-data(1,1,1)), ... 
    6*10^6*(data(2,:,1)-data(2,1,1)), ... 
    6*10^6*(data(1,:,6)-data(1,1,6)),...%+wgn(1,size_t,r_gps_noise^2,'linear'),...
    6*10^6*(data(2,:,6)-data(2,1,6)));%+wgn(1,size_t,r_gps_noise^2,'linear'));
}
% figure ()
% plot(6*10^6*(data(1,1:tspan,1)-data(1,1,6)), ... 
%     6*10^6*(data(2,1:tspan,1)-data(2,1,6)),...
%        6*10^6*(data(1,1:tspan,6)-data(1,1,6)),...%+wgn(1,size_t,r_gps_noise^2,'linear'),...
%     6*10^6*(data(2,1:tspan,6)-data(2,1,6)),...
%     6*10^6*(data(1,1:tspan,11)-data(1,1,6)), ... 
%     6*10^6*(data(2,1:tspan,11)-data(2,1,6)));
%     
% axis equal
% title('Filtered trajectory')
% ylabel('Distance [m]')
% xlabel('Distance [m]')
% 
%Calculating the error between true and GPS and true and Kalman
X_Kalman_x = data(1,1:tspan,1);
X_Kalman_y = data(2,1:tspan,1);

X_GPS_x = data(1,1:tspan,11);
X_GPS_y = data(2,1:tspan,11);

Y_x = data(1,1:tspan,6);
Y_y = data(2,1:tspan,6);

X_GPS =6*10^6* [data(1,1:tspan,6)-data(1,1,6); data(2,1:tspan,6)-data(2,1,6)];
%Y = 6*10^6* [data(1,1:tspan,1)-data(1,1,6); data(2,1:tspan,1)-data(2,1,6)];
MSE_Kalman_x = ((Y_x - X_Kalman_x).^2).^0.5;
MSE_Kalman_y = ((Y_y - X_Kalman_y).^2).^0.5;
mean_KALMAN_x = mean(MSE_Kalman_x);
mean_KALMAN_y = mean(MSE_Kalman_y);

disp('mean kalman_x:')
disp(mean_KALMAN_x)
disp('mean Kalman_y:')
disp(mean_KALMAN_y)

MSE_GPS_x = ((Y_x - X_GPS_x).^2).^0.5;
MSE_GPS_y = ((Y_y - X_GPS_y).^2).^0.5;
mean_GPS_X = mean(MSE_GPS_x);
mean_GPS_Y = mean(MSE_GPS_y);

disp('mean gps_x:')
disp(mean_GPS_X)
disp('mean gps_y:')
disp(mean_GPS_Y)

figure()
subplot(2,1,1);
plot(1:tspan, MSE_Kalman_x, 1:tspan, MSE_Kalman_y)
title('Mean Squared Error for filtred data, X and Y direction, respectively');
xlabel('time');
ylabel('MSE');
legend('X-direction', 'Y-direction');

subplot(2,1,2);
plot(1:tspan, MSE_GPS_x, 1:tspan, MSE_GPS_y)
title('Mean Squared Error for raw-GPS data, X and Y direction, respectively');
xlabel('time');
ylabel('MSE');
legend('X-direction', 'Y-direction');

figure ()
plot(6*10^6*(data(1,1:tspan,1)-data(1,1,6)), ... 
    6*10^6*(data(2,1:tspan,1)-data(2,1,6)), ...
    6*10^6*(data(1,1:tspan,11)-data(1,1,6)), ... 
    6*10^6*(data(2,1:tspan,11)-data(2,1,6)),...
    6*10^6*(data(1,1:tspan,6)-data(1,1,6)),...%+wgn(1,size_t,r_gps_noise^2,'linear'),...
    6*10^6*(data(2,1:tspan,6)-data(2,1,6)));
title('True value VS raw GPS and Filtred data');
xlabel('position [m]');
ylabel('position [m]');
legend('Filtred data', 'raw GPS data', 'True value');



% 


% {
% plot(6*10^6*(data(1,:,1)-data(1,1,6)), ... 
%     6*10^6*(data(2,:,1)-data(2,1,6)), ... 
%     6*10^6*(data(1,:,6)-data(1,1,6)),...%+wgn(1,size_t,r_gps_noise^2,'linear'),...
%     6*10^6*(data(2,:,6)-data(2,1,6)));%+wgn(1,size_t,r_gps_noise^2,'linear'));
% }
% plot(t_mem, data(1,:,12), t_mem, data(1,:,7))
% 
% plot(del_t:del_t:(del_t*size_t),data(1,:,7),...
%    del_t:del_t:(del_t*size_t),data(2,:,7)) 
% 
% 
% plot(t_mem,data(3,:,1)) %looks ok
% plot(t_mem,data4(1,:)) %looks ok
% plot(t_mem,data4(2,:))  %looks ok

