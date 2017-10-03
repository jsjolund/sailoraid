%serialPort = 'COM1';
serialPort = '/dev/ttyACM1';

if exist('s', 'var')
    try
        fclose(s);
    catch
    end
    delete(s);
    clear s;
end

nPointsInFigure = 1000;  % number of "sliding points" in your figure
step = 0.1;         % X points spacing
xVals = linspace(-(nPointsInFigure-1)*step,0,nPointsInFigure); % prepare empty data for the plot
yVals = NaN(nPointsInFigure,1);

fig = figure(1);

hp = plot( xVals , yVals ); % Generate the plot (with empty data) it will be passed to the callback.
ylim([-3 3]);

s = serial(serialPort','BaudRate',115200);
if (s.Status == 'closed')
    s.ReadAsyncMode = 'continuous';
    s.InputBufferSize = 1000;
    s.Terminator = 'CR/LF';
    s.BytesAvailableFcnMode = 'terminator';
    s.BytesAvailableFcn = {@Serial_OnDataReceived,hp,step}
    s.Timeout = 5;
    fopen(s);
end

function Serial_OnDataReceived(obj,event,hp,step)
    xVals = get(hp,'XData') ; % retrieve the X values from the plot
    yVals = get(hp,'YData') ; % retrieve the Y values from the plot

    while obj.BytesAvailable > 0
        arr = strsplit(fgetl(obj), ' ');
        if length(arr) ~= 29
            return
        end
        data = str2double(arr);

        sensor.imu.ax = (data(1));
        sensor.imu.ay = (data(2));
        sensor.imu.az = (data(3));
        sensor.imu.gx = (data(4));
        sensor.imu.gy = (data(5));
        sensor.imu.gz = (data(6));
        sensor.imu.mx = (data(7));
        sensor.imu.my = (data(8));
        sensor.imu.mz = (data(9));
        sensor.imu.roll = (data(10));
        sensor.imu.pitch = (data(11));
        sensor.imu.yaw = (data(12));
        sensor.env.humidity = (data(13));
        sensor.env.pressure = (data(14));
        sensor.env.temperature = (data(15));
        sensor.gps.time.day = int32(data(16));
        sensor.gps.time.month = int32(data(17));
        sensor.gps.time.year = int32(data(18));
        sensor.gps.time.hour = int32(data(19));
        sensor.gps.time.min = int32(data(20));
        sensor.gps.time.sec = int32(data(21));
        sensor.gps.time.hsec = int32(data(22));
        sensor.gps.pos.latitude = (data(23));
        sensor.gps.pos.longitude = (data(24));
        sensor.gps.pos.elevation = (data(25));
        sensor.gps.pos.speed = (data(26));
        sensor.gps.pos.direction = (data(27));
        sensor.gps.info.satUse = int32(data(28));
        sensor.gps.info.satView = int32(data(29));

        % do stuff with the data
        yVals = circshift(yVals,-1);
        yVals(end) = sensor.imu.az;
        xVals = circshift(xVals,-1);
        xVals(end) = xVals(end-1) + step;
        set(hp, 'XData', xVals, 'YData', yVals); % update the plot with the new values
        drawnow limitrate
    end

end