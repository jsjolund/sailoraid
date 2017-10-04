% Example program which plots from serial
clear;

%serialPort = 'COM1'; % Windows
serialPort = '/dev/ttyACM3'; % Linux

global hp step sensorLog

% % Create a figure to plot into
% nPointsInFigure = 250;  % Number of "sliding points" in your figure
% step = 0.01;         % X points spacing
% xVals = linspace(-(nPointsInFigure-1)*step, 0, nPointsInFigure); % Prepare empty data for the plot
% yVals = NaN(nPointsInFigure, 1);
% fig = figure(1);
% hp = plot(xVals , yVals); % Generate the plot (with empty data) it will be passed to the callback.
% ylim([-3 3]);

sensorLog = {};

% Start reading from serial
try
    serialRead(serialPort, @sensorUpdate);
catch err
    rethrow(err)
end

% fprintf('Logged %d samples\n', height(sensorLog));
fprintf('Program ended...\n');
    
% This is run when sensor values are updated
function sensorUpdate(sensor)
%     fprintf('x=%f y=%f z=%f\n',sensor.imu.roll,sensor.imu.pitch,sensor.imu.yaw)
%     plotUpdate(sensor);
    logUpdate(sensor);
end

% % Update the plot
% function plotUpdate(sensor)
% global hp step
% xVals = get(hp,'XData');
% yVals = get(hp,'YData');
% yVals = circshift(yVals,-1);
% yVals(end) = sensor.imu.az;
% xVals = circshift(xVals,-1);
% xVals(end) = xVals(end-1) + step;
% set(hp, 'XData', xVals, 'YData', yVals);
% drawnow limitrate
% end

% Log the data
function logUpdate(sensor)
global sensorLog
[~,~,~,hours,minutes,seconds] = datevec(now);
timeNow = 1000*(3600*hours + 60*minutes + seconds);
row = {uint64(timeNow)};
row{2}  = struct2cell(sensor);
sensorLog = [sensorLog; row];
end