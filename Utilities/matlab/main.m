%% Script which logs sensor values from serial port and can plot real-time
% Press Ctrl+C in the Command Window to stop reading and recording.
clear -global;
clear;
%serialPort = 'COM1'; % Windows
serialPort = '/dev/ttyACM3'; % Linux

% Can be removed for faster logging
% realTimePlot();

% Start reading from serial
serialRead(serialPort, @sensorUpdateCallback);

% Plot from sensor log after serial read stopped
% global sensorLog
% imuLog = [sensorLog.imu];
% timeLog = [sensorLog.sys];
% plot([timeLog.absTime],[imuLog.az])
fprintf('Program terminated.\n');
    
%% This callback is run when sensor values are updated
function sensorUpdateCallback(sensor)
global hp sensorLog
sensorLog = [sensorLog, sensor];
if ishghandle(hp)
    realTimePlotUpdate(sensor);
end
if mod(length(sensorLog), 25) == 0
    fprintf('.');
end
end

%% Real time plot functions
% Create a figure to plot into
function realTimePlot()
global hp step
nPointsInFigure = 300;  % Number of "sliding points" in your figure
step = 0.01;         % X points spacing
xVals = linspace(-(nPointsInFigure-1)*step, 0, nPointsInFigure); % Prepare empty data for the plot
yVals = NaN(nPointsInFigure, 1);
figure(1);
hp = plot(xVals , yVals); % Generate the plot (with empty data) it will be passed to the callback.
ylim([-3 3]);
end

% Update the plot by circle shifting the values as needed
function realTimePlotUpdate(sensor)
global hp step
xVals = get(hp,'XData');
yVals = get(hp,'YData');
yVals = circshift(yVals,-1);
yVals(end) = sensor.imu.az; % Plot variable
xVals = circshift(xVals,-1);
xVals(end) = xVals(end-1) + step;
set(hp, 'XData', xVals, 'YData', yVals);
drawnow limitrate
end
