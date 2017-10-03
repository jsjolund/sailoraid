%% Start listening to sensor data
function serial_read(serialPort,cbFunc)
if exist('s', 'var')
    try
        fclose(s);
    catch
    end
    delete(s);
    clear s;
end
s = serial(serialPort);
if (s.Status == 'closed')
    s.BaudRate = 115200;
    s.ReadAsyncMode = 'continuous';
    s.InputBufferSize = 1024;
    s.ByteOrder = 'littleEndian';
    s.BytesAvailableFcnMode = 'byte';
    s.BytesAvailableFcnCount = 30*4;
    set(s,'BytesAvailableFcn',{@serial_receive,cbFunc})
    s.Timeout = 2;
    % Open the serial connection
    fprintf('Opening connection.\n')
    fopen(s);
    % Start the sensor value stream
    fprintf(s,sprintf('\r\n\r\nmatlab\r\n'));
    pause(1);
    % Close the serial connection
    fprintf('Closing connection.\n')
    fclose(s);
    delete(s);
    clear s;
end
end

%% Read the serial stream
function serial_receive(obj,~,cbFunc)
% fprintf('receive\n');
while obj.BytesAvailable > 0 
    % Read data as float array
    [sensordata,~] = fread(obj, 30*4, 'float32');
    % If last float is not NaN, we need to sync the stream
    if ~isnan(sensordata(30))
        fprintf('Synchronizing...\n');
        nanCnt = 0;
        while 1
            [sensordata,~] = fread(obj, 1, 'uint8');
            if sensordata(1) == 255
                nanCnt = nanCnt+1;
            else
                nanCnt = 0;
            end
            if nanCnt == 4
                break;
            end
        end
        return;
    end
    % Store in struct with fields
    sensor.imu.ax = sensordata(1);
    sensor.imu.ay = sensordata(2);
    sensor.imu.az = sensordata(3);
    sensor.imu.gx = sensordata(4);
    sensor.imu.gy = sensordata(5);
    sensor.imu.gz = sensordata(6);
    sensor.imu.mx = sensordata(7);
    sensor.imu.my = sensordata(8);
    sensor.imu.mz = sensordata(9);
    sensor.imu.roll = sensordata(10);
    sensor.imu.pitch = sensordata(11);
    sensor.imu.yaw = sensordata(12);
    sensor.env.humidity = sensordata(13);
    sensor.env.pressure = sensordata(14);
    sensor.env.temperature = sensordata(15);
    sensor.gps.time.year = typecast(sensordata(18),'int32');
    sensor.gps.time.month = typecast(sensordata(17),'int32');
    sensor.gps.time.day = typecast(sensordata(16),'int32');
    sensor.gps.time.hour = typecast(sensordata(19),'int32');
    sensor.gps.time.min = typecast(sensordata(20),'int32');
    sensor.gps.time.sec = typecast(sensordata(21),'int32');
    sensor.gps.time.hsec = typecast(sensordata(22),'int32');
    sensor.gps.pos.latitude = sensordata(23);
    sensor.gps.pos.longitude = sensordata(24);
    sensor.gps.pos.elevation = sensordata(25);
    sensor.gps.pos.speed = sensordata(26);
    sensor.gps.pos.direction = sensordata(27);
    sensor.gps.info.satUse = typecast(sensordata(28),'int32');
    sensor.gps.info.satView = typecast(sensordata(29),'int32');
    % User callback
    cbFunc(sensor);
end
end