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
% Supported baud rates 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200,
% 38400, 57600, 115200, 230400, 460800, 921600
s = serial(serialPort);
if (s.Status == 'closed')
    s.BaudRate = 230400;
    s.ReadAsyncMode = 'continuous';
    s.InputBufferSize = 1024;
    s.ByteOrder = 'littleEndian';
    s.BytesAvailableFcnMode = 'byte';
    s.BytesAvailableFcnCount = 31*4;
    s.Timeout = 2;
    % Open the serial connection
    fprintf('Opening connection...\n')
    fprintf('Press Ctrl+C to stop recording.\n')
    fprintf('Recording.');
    fopen(s);
    % Start the sensor value stream
    fprintf(s,sprintf('\r\n\r\n\r\nmatlab\r\n'));
    tic();
    set(s,'BytesAvailableFcn',{@serialReceive,callback});
    % Read
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
[data,~] = fread(obj, 31*4, 'float32');
% If last float is not NaN, we need to sync the stream
if ~isnan(data(31))
    fprintf('\nSynchronizing...\n');
    ffCnt = 0;
    while 1
        [data,~] = fread(obj, 1, 'uint8');
        if data(1) == 255
            ffCnt = ffCnt+1;
        else
            ffCnt = 0;
        end
        if ffCnt == 4
            break;
        end
    end
    fprintf('Recording.');
    return;
end

% Store sensor values in struct with fields
sensor.imu.ax               = data(1);
sensor.imu.ay               = data(2);
sensor.imu.az               = data(3);
sensor.imu.gx               = data(4);
sensor.imu.gy               = data(5);
sensor.imu.gz               = data(6);
sensor.imu.mx               = data(7);
sensor.imu.my               = data(8);
sensor.imu.mz               = data(9);
sensor.imu.roll             = data(10);
sensor.imu.pitch            = data(11);
sensor.imu.yaw              = data(12);
sensor.env.humidity         = data(13);
sensor.env.pressure         = data(14);
sensor.env.temperature      = data(15);
sensor.gps.time.year        = ftoi(data(16));
sensor.gps.time.month       = ftoi(data(17));
sensor.gps.time.day         = ftoi(data(18));
sensor.gps.time.hour        = ftoi(data(19));
sensor.gps.time.min         = ftoi(data(20));
sensor.gps.time.sec         = ftoi(data(21));
sensor.gps.time.hsec        = ftoi(data(22));
sensor.gps.pos.latitude     = data(23);
sensor.gps.pos.longitude    = data(24);
sensor.gps.pos.elevation    = data(25);
sensor.gps.pos.speed        = data(26);
sensor.gps.pos.direction    = data(27);
sensor.gps.info.satUse      = ftoi(data(28));
sensor.gps.info.satView     = ftoi(data(29));
sensor.range.range0         = data(30);

% Add a timestamp
sensor.sys.dateTime = now;

% Store time in milliseconds since last update
sensor.sys.deltaTime = toc()*1000;
tic();

% Absolute time in ms
global absTime
if isempty(absTime)
    absTime = 0;
end
absTime = absTime + sensor.sys.deltaTime;
sensor.sys.absTime = absTime;

% User callback
callback(sensor);
end

%% Cast 32 bit floating point to 32 bit integer
function i = ftoi(f)
    i = typecast(single(f),'int32');
end
