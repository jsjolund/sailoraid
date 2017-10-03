% Example program which plots from serial
clear;

%serialPort = 'COM1'; % Windows
serialPort = '/dev/ttyACM0'; % Linux

global hp step ts
% Create a figure to plot into
nPointsInFigure = 250;  % Number of "sliding points" in your figure
step = 0.01;         % X points spacing
xVals = linspace(-(nPointsInFigure-1)*step,0,nPointsInFigure); % Prepare empty data for the plot
yVals = NaN(nPointsInFigure,1);
fig = figure(1);
hp = plot( xVals , yVals ); % Generate the plot (with empty data) it will be passed to the callback.
ylim([-3 3]);

ts = timeseries()

% Start reading from serial
try
    serial_read(serialPort, @sensor_update);
catch
end
fprintf('Program ended...\n');
    
% This is run when sensor values are updated
function sensor_update(sensor)
    % Sensor values accessed by fields
    fprintf('x=%f y=%f z=%f\n',sensor.imu.roll,sensor.imu.pitch,sensor.imu.yaw)
    
    % Update the plot
    global hp step ts
    xVals = get(hp,'XData');
    yVals = get(hp,'YData');
    yVals = circshift(yVals,-1);
    yVals(end) = sensor.imu.az;
    xVals = circshift(xVals,-1);
    xVals(end) = xVals(end-1) + step;
    set(hp, 'XData', xVals, 'YData', yVals);
    drawnow limitrate
    
    % Log the data
    [~,~,~,hours,minutes,seconds] = datevec(now);
    t = 1000*(3600*hours + 60*minutes + seconds);
    ts = addsample(ts,'Time',t,'Data',[...
        sensor.imu.ax...
        sensor.imu.ay...
        sensor.imu.az...
        sensor.gps.pos.latitude...
        sensor.gps.pos.longitude...
        sensor.gps.pos.elevation...
        ]);
end
