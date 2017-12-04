This was program was used for a dinghy (small boat), to achive a more accurate  

This is a program that uses an Inertial Navigation System (INS) and takes the difference between the INS and GPS data and feeds that to a Kalman filter. The error is then sent back to the INS to correct the next output. 

You can just run the code, using sim_script. But you have to link the folder "data", this since the data provided by the IMU and GPS is in that folder.
 
If you want to test with new data, open folder SERIAL_READ and run the main file. New data will be collected from the stm32,(assumed it has correct setup).
Then open folder DATA and run test_serial.m,
new vectors containing the logged data will be created, acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, longitude, latitude, velocity(gps), height and time. Don't forget to link the sensorLog.mat file which is created by the main file in SERIAL_READ. 
Moreover in folder DATA running data_script.m will generate calibration 
results which should be stored in calibration_res.mat.
 
Things that should be changed according to the hardware that the user uses. is the standard deviation in kalman_gps_ins.m file
 

  

