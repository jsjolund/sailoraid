clear
%CALIBRATION_SCRIPT This script computes the coefficients necessary to correct the data
%	The correction applies only for the accelerometers. It requires nine static 
%	captured positions, corrects the scale and bias of each of three accelerometers



load('calibration.mat')


%1-x 2-y 3-z
acc(:,1) = cell2mat(calibration(2:size(calibration,1),1));
acc(:,2) = cell2mat(calibration(2:size(calibration,1),2));
acc(:,3) = cell2mat(calibration(2:size(calibration,1),3));
time = 0.5 * 10^-3 * cell2mat(calibration(2:size(calibration,1),32)); %milliseconds



gyro(:,1) = cell2mat(calibration(2:size(calibration,1),10));
gyro(:,2) = cell2mat(calibration(2:size(calibration,1),11));
gyro(:,3) = cell2mat(calibration(2:size(calibration,1),12));



%subplot(2,1,1)
plot(sqrt(acc(:,1).^2+acc(:,2).^2+acc(:,3).^2))
hold on


%{
The user has to define the ranges by him/her self
This is done by examine the plot
%}

range{1}=5:37;
range{2}=52:84;
range{3}=104:137;
range{4}=153:183;
range{5}=204:238;
range{6}=250:298;

gamma=9.8123;

%cal_phi(i,k,l)
for i = 1:size(range,2)
    cut = acc(range{i},:);
    l(i,:) = mean(cut, 1);
end

handle = @(x) f_g(x,l,gamma);
x0=zeros(6,1);

options = optimoptions(@fsolve,'Display','iter',...
    'Algorithm','trust-region',...
    'SpecifyObjectiveGradient',false,'PrecondBandWidth',0);
[x,fval,exitflag,output] = fsolve(handle,x0,options);

bgx=x(1);
bgy=x(2);
bgz=x(3);
sgx=x(4);
sgy=x(5);
sgz=x(6);

for i=1:3
    acc(:,i)=acc(:,i)-x(i)*ones(size(acc,1),1);
end

for i=1:3
    acc(:,i)=acc(:,i)/(1+x(i+3));
end

%subplot(2,1,2)
plot(sqrt(acc(:,1).^2+acc(:,2).^2+acc(:,3).^2))

