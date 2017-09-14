EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:INA1245 amplifier-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L INA128 U1
U 1 1 59BA39EB
P 5450 3350
F 0 "U1" H 5600 3475 50  0000 L CNN
F 1 "INA126" H 5600 3225 50  0000 L CNN
F 2 "" H 5550 3350 50  0001 C CNN
F 3 "" H 5550 3350 50  0001 C CNN
	1    5450 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 2750 5450 3050
$Comp
L GND #PWR01
U 1 1 59BA3A9D
P 5500 3950
F 0 "#PWR01" H 5500 3700 50  0001 C CNN
F 1 "GND" H 5500 3800 50  0000 C CNN
F 2 "" H 5500 3950 50  0001 C CNN
F 3 "" H 5500 3950 50  0001 C CNN
	1    5500 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 3950 5550 3650
Wire Wire Line
	5450 3650 5450 3950
Connection ~ 5500 3950
Wire Wire Line
	5450 3950 5550 3950
$Comp
L R RG1
U 1 1 59BA3B02
P 4850 3350
F 0 "RG1" V 4950 3350 50  0000 C CNN
F 1 "5360" V 4850 3350 50  0000 C CNN
F 2 "" V 4780 3350 50  0001 C CNN
F 3 "" H 4850 3350 50  0001 C CNN
	1    4850 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 3200 5000 3200
Wire Wire Line
	5000 3200 5000 3250
Wire Wire Line
	5000 3250 5150 3250
Wire Wire Line
	5150 3450 5000 3450
Wire Wire Line
	5000 3450 5000 3500
Wire Wire Line
	5000 3500 4850 3500
Wire Wire Line
	5150 3550 5100 3550
Wire Wire Line
	5100 3550 5100 3600
Wire Wire Line
	5100 3600 4650 3600
Wire Wire Line
	5150 3150 5100 3150
Wire Wire Line
	5100 3150 5100 3100
Wire Wire Line
	5100 3100 4650 3100
Wire Wire Line
	5850 3350 6200 3350
Text Label 6200 3350 0    60   ~ 0
force_an
Text Label 4650 3100 0    60   ~ 0
O+_yellow
$Comp
L Conn_01x04 J1
U 1 1 59BA3E2D
P 4300 3400
F 0 "J1" H 4300 3600 50  0000 C CNN
F 1 "Conn_FS" H 4300 3100 50  0000 C CNN
F 2 "" H 4300 3400 50  0001 C CNN
F 3 "" H 4300 3400 50  0001 C CNN
	1    4300 3400
	-1   0    0    1   
$EndComp
Wire Wire Line
	4650 3100 4650 3300
Wire Wire Line
	4650 3600 4650 3400
Wire Wire Line
	4650 3300 4500 3300
Wire Wire Line
	4650 3400 4500 3400
Wire Wire Line
	4500 3500 4500 3950
Wire Wire Line
	4500 3950 5500 3950
Wire Wire Line
	4500 3200 4500 2950
Wire Wire Line
	4500 2950 5450 2950
Connection ~ 5450 2950
$Comp
L +5V #PWR02
U 1 1 59BA6D0C
P 5450 2750
F 0 "#PWR02" H 5450 2600 50  0001 C CNN
F 1 "+5V" H 5450 2890 50  0000 C CNN
F 2 "" H 5450 2750 50  0001 C CNN
F 3 "" H 5450 2750 50  0001 C CNN
	1    5450 2750
	1    0    0    -1  
$EndComp
Text Notes 4650 2850 0    60   ~ 0
O+_yellow
$EndSCHEMATC
