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
LIBS:IMU_Circuit
LIBS:IMU_Circuit-cache
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
L LSM6DSL U?
U 1 1 59AE6811
P 6950 4600
F 0 "U?" H 7400 5100 60  0000 C CNN
F 1 "LSM6DSL" H 7400 5000 60  0000 C CNN
F 2 "" H 7400 5050 60  0001 C CNN
F 3 "" H 7400 5050 60  0001 C CNN
	1    6950 4600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 59AE6948
P 6950 3100
F 0 "#PWR01" H 6950 2850 50  0001 C CNN
F 1 "GND" H 6950 2950 50  0000 C CNN
F 2 "" H 6950 3100 50  0001 C CNN
F 3 "" H 6950 3100 50  0001 C CNN
	1    6950 3100
	1    0    0    -1  
$EndComp
$Comp
L C_Small C?
U 1 1 59AE6A62
P 7850 2700
F 0 "C?" H 7860 2770 50  0000 L CNN
F 1 "220nF" H 7860 2620 50  0000 L CNN
F 2 "" H 7850 2700 50  0001 C CNN
F 3 "" H 7850 2700 50  0001 C CNN
	1    7850 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 3050 7050 3000
Wire Wire Line
	6850 4050 6850 4000
Wire Wire Line
	6850 4000 7050 4000
Wire Wire Line
	7050 4000 7050 4050
Wire Wire Line
	6850 5150 6850 5200
Wire Wire Line
	6850 5200 7050 5200
Wire Wire Line
	7050 5200 7050 5150
Wire Wire Line
	6950 5200 6950 5250
Connection ~ 6950 5200
Wire Wire Line
	6950 4000 6950 3950
Connection ~ 6950 4000
Wire Wire Line
	7850 4300 7850 4650
Wire Wire Line
	7850 4650 7800 4650
Wire Wire Line
	7800 4550 7850 4550
Connection ~ 7850 4550
Wire Wire Line
	7800 4450 7850 4450
Connection ~ 7850 4450
Connection ~ 7850 4350
$Comp
L C_Small C?
U 1 1 59AE745B
P 8900 2450
F 0 "C?" H 8910 2520 50  0000 L CNN
F 1 "100nF" H 8910 2370 50  0000 L CNN
F 2 "" H 8900 2450 50  0001 C CNN
F 3 "" H 8900 2450 50  0001 C CNN
	1    8900 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 2300 8900 2350
$Comp
L CP1_Small C?
U 1 1 59AE76A3
P 8600 2450
F 0 "C?" H 8610 2520 50  0000 L CNN
F 1 "10uF" H 8610 2370 50  0000 L CNN
F 2 "" H 8600 2450 50  0001 C CNN
F 3 "" H 8600 2450 50  0001 C CNN
	1    8600 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 2600 8900 2550
$Comp
L GND #PWR02
U 1 1 59AE77C4
P 8750 2650
F 0 "#PWR02" H 8750 2400 50  0001 C CNN
F 1 "GND" H 8750 2500 50  0000 C CNN
F 2 "" H 8750 2650 50  0001 C CNN
F 3 "" H 8750 2650 50  0001 C CNN
	1    8750 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 2600 8900 2600
Wire Wire Line
	8600 2600 8600 2550
Wire Wire Line
	8600 2350 8600 2300
Wire Wire Line
	8750 2650 8750 2600
Connection ~ 8750 2600
Wire Wire Line
	8600 2300 8900 2300
Wire Wire Line
	8750 2300 8750 2250
Connection ~ 8750 2300
Text Notes 8450 2050 0    60   ~ 0
LSM303AGR VDD\nDecoup. Cap.
$Comp
L C_Small C?
U 1 1 59AE8809
P 9650 2450
F 0 "C?" H 9660 2520 50  0000 L CNN
F 1 "100nF" H 9660 2370 50  0000 L CNN
F 2 "" H 9650 2450 50  0001 C CNN
F 3 "" H 9650 2450 50  0001 C CNN
	1    9650 2450
	1    0    0    -1  
$EndComp
Text Notes 9400 2050 0    60   ~ 0
LSM303AGR VDD_IO\nDecoup. Cap.
$Comp
L GND #PWR03
U 1 1 59AE8CF4
P 9650 2600
F 0 "#PWR03" H 9650 2350 50  0001 C CNN
F 1 "GND" H 9650 2450 50  0000 C CNN
F 2 "" H 9650 2600 50  0001 C CNN
F 3 "" H 9650 2600 50  0001 C CNN
	1    9650 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 2600 9650 2550
Wire Wire Line
	9650 2350 9650 2300
Wire Wire Line
	7800 4350 7850 4350
Wire Wire Line
	6850 1900 6850 1850
Wire Wire Line
	6850 1850 7050 1850
Wire Wire Line
	7050 1850 7050 1900
Wire Wire Line
	6950 1800 6950 1850
Connection ~ 6950 1850
Text Label 6100 2250 2    60   ~ 0
I2C1_CL
Text Label 6100 4450 2    60   ~ 0
I2C1_CL
Text Label 6100 2350 2    60   ~ 0
I2C1_DA
Text Label 6100 4550 2    60   ~ 0
I2C1_DA
$Comp
L GND #PWR04
U 1 1 59AEAB2A
P 6950 5250
F 0 "#PWR04" H 6950 5000 50  0001 C CNN
F 1 "GND" H 6950 5100 50  0000 C CNN
F 2 "" H 6950 5250 50  0001 C CNN
F 3 "" H 6950 5250 50  0001 C CNN
	1    6950 5250
	1    0    0    -1  
$EndComp
$Comp
L C_Small C?
U 1 1 59AEADF5
P 8750 4600
F 0 "C?" H 8760 4670 50  0000 L CNN
F 1 "100nF" H 8760 4520 50  0000 L CNN
F 2 "" H 8750 4600 50  0001 C CNN
F 3 "" H 8750 4600 50  0001 C CNN
	1    8750 4600
	1    0    0    -1  
$EndComp
$Comp
L C_Small C?
U 1 1 59AEAE47
P 9600 4600
F 0 "C?" H 9610 4670 50  0000 L CNN
F 1 "100nF" H 9610 4520 50  0000 L CNN
F 2 "" H 9600 4600 50  0001 C CNN
F 3 "" H 9600 4600 50  0001 C CNN
	1    9600 4600
	1    0    0    -1  
$EndComp
Text Notes 8450 4200 0    60   ~ 0
LSM6DSL VDD\nDecoup. Cap.
Text Notes 9400 4200 0    60   ~ 0
LSM6DSL VDD_IO\nDecoup. Cap.
Wire Wire Line
	8750 4700 8750 4750
Wire Wire Line
	9600 4700 9600 4750
Wire Wire Line
	8750 4500 8750 4450
Wire Wire Line
	9600 4500 9600 4450
$Comp
L +3V3 #PWR05
U 1 1 59AED7D9
P 9650 2300
F 0 "#PWR05" H 9650 2150 50  0001 C CNN
F 1 "+3V3" H 9650 2440 50  0000 C CNN
F 2 "" H 9650 2300 50  0001 C CNN
F 3 "" H 9650 2300 50  0001 C CNN
	1    9650 2300
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR06
U 1 1 59AED815
P 8750 2250
F 0 "#PWR06" H 8750 2100 50  0001 C CNN
F 1 "+3V3" H 8750 2390 50  0000 C CNN
F 2 "" H 8750 2250 50  0001 C CNN
F 3 "" H 8750 2250 50  0001 C CNN
	1    8750 2250
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR07
U 1 1 59AED883
P 6950 1800
F 0 "#PWR07" H 6950 1650 50  0001 C CNN
F 1 "+3V3" H 6950 1940 50  0000 C CNN
F 2 "" H 6950 1800 50  0001 C CNN
F 3 "" H 6950 1800 50  0001 C CNN
	1    6950 1800
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR08
U 1 1 59AED96E
P 7850 2250
F 0 "#PWR08" H 7850 2100 50  0001 C CNN
F 1 "+3V3" H 7850 2390 50  0000 C CNN
F 2 "" H 7850 2250 50  0001 C CNN
F 3 "" H 7850 2250 50  0001 C CNN
	1    7850 2250
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR09
U 1 1 59AEDBD9
P 6950 3950
F 0 "#PWR09" H 6950 3800 50  0001 C CNN
F 1 "+3V3" H 6950 4090 50  0000 C CNN
F 2 "" H 6950 3950 50  0001 C CNN
F 3 "" H 6950 3950 50  0001 C CNN
	1    6950 3950
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR010
U 1 1 59AEDC1C
P 7850 4300
F 0 "#PWR010" H 7850 4150 50  0001 C CNN
F 1 "+3V3" H 7850 4440 50  0000 C CNN
F 2 "" H 7850 4300 50  0001 C CNN
F 3 "" H 7850 4300 50  0001 C CNN
	1    7850 4300
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR011
U 1 1 59AEDE3F
P 8750 4450
F 0 "#PWR011" H 8750 4300 50  0001 C CNN
F 1 "+3V3" H 8750 4590 50  0000 C CNN
F 2 "" H 8750 4450 50  0001 C CNN
F 3 "" H 8750 4450 50  0001 C CNN
	1    8750 4450
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR012
U 1 1 59AEDEFA
P 9600 4450
F 0 "#PWR012" H 9600 4300 50  0001 C CNN
F 1 "+3V3" H 9600 4590 50  0000 C CNN
F 2 "" H 9600 4450 50  0001 C CNN
F 3 "" H 9600 4450 50  0001 C CNN
	1    9600 4450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 59AEE41D
P 8750 4750
F 0 "#PWR013" H 8750 4500 50  0001 C CNN
F 1 "GND" H 8750 4600 50  0000 C CNN
F 2 "" H 8750 4750 50  0001 C CNN
F 3 "" H 8750 4750 50  0001 C CNN
	1    8750 4750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 59AEE447
P 9600 4750
F 0 "#PWR014" H 9600 4500 50  0001 C CNN
F 1 "GND" H 9600 4600 50  0000 C CNN
F 2 "" H 9600 4750 50  0001 C CNN
F 3 "" H 9600 4750 50  0001 C CNN
	1    9600 4750
	1    0    0    -1  
$EndComp
$Comp
L LSM303AGR U?
U 1 1 59AEB7F4
P 6950 2450
F 0 "U?" H 7350 2950 60  0000 C CNN
F 1 "LSM303AGR" H 7350 2850 60  0000 C CNN
F 2 "" H 6950 2700 60  0001 C CNN
F 3 "" H 6950 2700 60  0001 C CNN
	1    6950 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 3000 6850 3050
Wire Wire Line
	6850 3050 7050 3050
Wire Wire Line
	6950 3050 6950 3100
Connection ~ 6950 3050
Wire Wire Line
	7800 2550 7850 2550
Wire Wire Line
	7850 2550 7850 2600
Wire Wire Line
	7850 2800 7850 2850
Wire Wire Line
	7850 2450 7800 2450
Wire Wire Line
	7850 2250 7850 2450
Wire Wire Line
	7800 2350 7850 2350
Connection ~ 7850 2350
$Comp
L GND #PWR015
U 1 1 59AEED02
P 7850 2850
F 0 "#PWR015" H 7850 2600 50  0001 C CNN
F 1 "GND" H 7850 2700 50  0000 C CNN
F 2 "" H 7850 2850 50  0001 C CNN
F 3 "" H 7850 2850 50  0001 C CNN
	1    7850 2850
	1    0    0    -1  
$EndComp
Text HLabel 1800 2100 0    60   Input ~ 0
I2C1_CL
Text HLabel 1800 2200 0    60   BiDi ~ 0
I2C1_DA
Text Label 1950 2100 0    60   ~ 0
I2C1_CL
Wire Wire Line
	1800 2100 2650 2100
Wire Wire Line
	1800 2200 2650 2200
Text Label 1950 2200 0    60   ~ 0
I2C1_DA
Text HLabel 1800 1650 0    60   Input ~ 0
+3V3
Text HLabel 1800 1750 0    60   Input ~ 0
GND
Wire Wire Line
	1800 1650 1900 1650
Wire Wire Line
	1850 1650 1850 1600
Wire Wire Line
	1800 1750 1900 1750
Wire Wire Line
	1850 1750 1850 1800
$Comp
L GND #PWR016
U 1 1 59AF43F3
P 1850 1800
F 0 "#PWR016" H 1850 1550 50  0001 C CNN
F 1 "GND" H 1850 1650 50  0000 C CNN
F 2 "" H 1850 1800 50  0001 C CNN
F 3 "" H 1850 1800 50  0001 C CNN
	1    1850 1800
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR017
U 1 1 59AF4417
P 1850 1600
F 0 "#PWR017" H 1850 1450 50  0001 C CNN
F 1 "+3V3" H 1850 1740 50  0000 C CNN
F 2 "" H 1850 1600 50  0001 C CNN
F 3 "" H 1850 1600 50  0001 C CNN
	1    1850 1600
	1    0    0    -1  
$EndComp
Text Label 6100 2450 2    60   ~ 0
LSM303AGR_INT_MAG
Text Label 6100 2550 2    60   ~ 0
LSM303AGR_INT_1_XL
Text Label 6100 2650 2    60   ~ 0
LSM303AGR_INT_1_XL
Text Label 6100 4650 2    60   ~ 0
LSM6DSL_INT1
Text Label 6100 4750 2    60   ~ 0
LSM6DSL_INT2
Text Notes 1200 2800 0    60   ~ 0
I2C1 och I2C2 är ihopkopplade \nsom standard på IKS01A2. Vi får \nvälja vilken vi ska använda och \nså kan vi strunta i den andra.
Text Label 2650 2100 0    60   ~ 0
I2C2_CL
Text Label 2650 2200 0    60   ~ 0
I2C2_DA
$Comp
L PWR_FLAG #FLG?
U 1 1 59B2957C
P 1900 1650
F 0 "#FLG?" H 1900 1725 50  0001 C CNN
F 1 "PWR_FLAG" V 1900 1950 50  0000 C CNN
F 2 "" H 1900 1650 50  0001 C CNN
F 3 "" H 1900 1650 50  0001 C CNN
	1    1900 1650
	0    1    1    0   
$EndComp
$Comp
L PWR_FLAG #FLG?
U 1 1 59B295A6
P 1900 1750
F 0 "#FLG?" H 1900 1825 50  0001 C CNN
F 1 "PWR_FLAG" V 1900 2050 50  0000 C CNN
F 2 "" H 1900 1750 50  0001 C CNN
F 3 "" H 1900 1750 50  0001 C CNN
	1    1900 1750
	0    1    1    0   
$EndComp
Connection ~ 1850 1650
Connection ~ 1850 1750
$EndSCHEMATC
