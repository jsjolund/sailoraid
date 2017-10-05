%% Start listening to sensor data
function serialRead(serialPort,callback)
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
    set(s,'BytesAvailableFcn',{@serialReceive,callback})
    s.Timeout = 2;
    % Open the serial connection
    fprintf('Opening connection...\n')
    fopen(s);
    pause(1);
    % Start the sensor value stream
    fprintf(s,sprintf('\r\n\r\n\r\nmatlab\r\n'));
    pause(1);
    % Close the serial connection
    fprintf('Closing connection...\n')
    fclose(s);
    delete(s);
    clear s;
end
end

%% Read the serial stream
function serialReceive(obj,~,callback)
% Read data as float array
[data,~] = fread(obj, 30*4, 'float32');
% If last float is not NaN, we need to sync the stream
if ~isnan(data(30))
    fprintf('Synchronizing...\n');
    nanCnt = 0;
    while 1
        [data,~] = fread(obj, 1, 'uint8');
        if data(1) == 255
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
sensor.imu.ax = data(1);
sensor.imu.ay = data(2);
sensor.imu.az = data(3);
sensor.imu.gx = data(4);
sensor.imu.gy = data(5);
sensor.imu.gz = data(6);
sensor.imu.mx = data(7);
sensor.imu.my = data(8);
sensor.imu.mz = data(9);
sensor.imu.roll = data(10);
sensor.imu.pitch = data(11);
sensor.imu.yaw = data(12);
sensor.env.humidity = data(13);
sensor.env.pressure = data(14);
sensor.env.temperature = data(15);
sensor.gps.time.year = typecast(single(data(16)),'int32');
sensor.gps.time.month = typecast(single(data(17)),'int32');
sensor.gps.time.day = typecast(single(data(18)),'int32');
sensor.gps.time.hour = typecast(single(data(19)),'int32');
sensor.gps.time.min = typecast(single(data(20)),'int32');
sensor.gps.time.sec = typecast(single(data(21)),'int32');
sensor.gps.time.hsec = typecast(single(data(22)),'int32');
sensor.gps.pos.latitude = data(23);
sensor.gps.pos.longitude = data(24);
sensor.gps.pos.elevation = data(25);
sensor.gps.pos.speed = data(26);
sensor.gps.pos.direction = data(27);
sensor.gps.info.satUse = typecast(single(data(28)),'int32');
sensor.gps.info.satView = typecast(single(data(29)),'int32');
% Add a timestamp
[~,~,~,hours,minutes,seconds] = datevec(now);
sensor.timestamp = uint64(1000*(3600*hours + 60*minutes + seconds));
% User callback
callback(sensor);
end