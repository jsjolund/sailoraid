


load('sensorLog.mat')

val_imu = [sensorLog(1:end).imu]';
acc_x = [val_imu(1:end).ax]';
acc_y = [val_imu(1:end).ay]';
acc_z = [val_imu(1:end).az]';

gyr_x = [val_imu(1:end).gx]';
gyr_y = [val_imu(1:end).gy]';
gyr_z = [val_imu(1:end).gz]';

mag_x = [val_imu(1:end).mx]';
mag_y = [val_imu(1:end).my]';
mag_z = [val_imu(1:end).mz]';

val_baro = [sensorLog(1:end).env]';
val_baro = [val_baro(1:end).pressure]';

val_time=[sensorLog(1:end).sys]';
time=[val_time(1:end).deltaTime]';

val_gps = [sensorLog(1:end).gps]';
val_gps = [val_gps(1:end).pos]';
lat_gps = [val_gps(1:end).latitude]';
long_gps = [val_gps(1:end).longitude]';
elevation_gps = [val_gps(1:end).elevation]';
speed_gps = [val_gps(1:end).speed]';
direction_gps = [val_gps(1:end).direction]';





