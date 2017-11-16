package ltuproject.sailoraid.bluetooth;

/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import java.util.HashMap;

/**
 * This class includes a small subset of standard GATT attributes for demonstration purposes.
 */
public class SampleGattAttributes {

    private static HashMap<String, String> attributes = new HashMap();
    public static String HEART_RATE_SERVICE =       "0000180d-0000-1000-8000-00805f9b34fb";
    public static String IMU_SERVICE =              "917649a0-d98e-11e5-9eec-0002a5d5c51b";
    public static String ENV_SERVICE =              "42821a40-e477-11e2-82d0-0002a5d5c51b";
    public static String ACCELEROMETER_SERVICE =    "02366e80-cf3a-11e1-9ab4-0002a5d5c51b";

    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";

    public static String HEART_RATE_MEASUREMENT =   "00002a37-0000-1000-8000-00805f9b34fb";
    public static String TEMP_MEASUREMENT =         "a32e5520-e477-11e2-a9e3-0002a5d5c51b";
    public static String PRESSURE_MEASUREMENT =     "cd20c480-e48b-11e2-840b-0002a5d5c51b";
    public static String HUMIDITY_MEASUREMENT =     "01c50b60-e48c-11e2-a073-0002a5d5c51b";
    public static String BATTERY_MEASUREMENT =     "01c50b60-e48c-11e2-a073-0002a5d5c51b";

    public static String ACCELEROMETER_MEASUREMENT ="340a1b80-cf4b-11e1-ac36-0002a5d5c51b";
    public static String COMPASS_MEASUREMENT =      "340a1b81-cf4b-11e1-ac36-0002a5d5c51b";
    public static String IMU_ACCEL_MEASUREMENT =    "917649a1-d98e-11e5-9eec-0002a5d5c51b";
    public static String IMU_GYRO_MEASUREMENT =     "917649a2-d98e-11e5-9eec-0002a5d5c51b";
    public static String FREE_FALL_MEASUREMENT =    "e23e78a0-cf4a-11e1-8ffc-0002a5d5c51b";

    public static String NUCLEO_GPS_SERVICE = "abcd6e80-cf3a-11e1-9ab4-0002a5d5c51b";
    public static String NUCLEO_GPS_MEASUREMENT = "aabb1b80-cf4b-11e1-ac36-0002a5d5c51b";

    public static String NUCLEO_RANGE_SERVICE = "23cd6e80-cf3a-11e1-9ab4-0002a5d5c51b";
    public static String NUCLEO_RANGE_MEASUREMENT = "23bb1b80-cf4b-11e1-ac36-0002a5d5c51b";
    public static String GPS_MEASUREMENT =          "00002902-0000-1000-8000-00805f9b34fb";

    static {
        // Sample Services.
        attributes.put(HEART_RATE_SERVICE, "Heart Rate Service");
        attributes.put("0000180a-0000-1000-8000-00805f9b34fb", "Device Information Service");
        attributes.put(IMU_SERVICE, "IMU Service");
        attributes.put(ACCELEROMETER_SERVICE, "Accelerometer Service");
        attributes.put(ENV_SERVICE, "Enviromental Service");
        attributes.put(NUCLEO_GPS_SERVICE, "Nucleao gps service");
        // Sample Characteristics.
        attributes.put(HEART_RATE_MEASUREMENT, "Heart Rate Measurement");
        attributes.put(ACCELEROMETER_MEASUREMENT, "Accelerometer Measurement");
        attributes.put(IMU_ACCEL_MEASUREMENT, "IMU Accelerometer Measurement");
        attributes.put(IMU_GYRO_MEASUREMENT, "IMU Gyro Measurement");
        attributes.put(TEMP_MEASUREMENT, "Temperature Measurement");
        attributes.put(PRESSURE_MEASUREMENT, "Pressure Measurement");
        attributes.put(HUMIDITY_MEASUREMENT, "Humidity Measurement");
        attributes.put(BATTERY_MEASUREMENT, "Battery Measurement");
        attributes.put(FREE_FALL_MEASUREMENT, "Free Fall Measurement");
        attributes.put(NUCLEO_GPS_MEASUREMENT, "Nucleo gps Measurement");
        attributes.put(NUCLEO_RANGE_MEASUREMENT, "Nucleo gps Measurement");
        attributes.put(COMPASS_MEASUREMENT, "Compass Measurement");
        attributes.put("00002a29-0000-1000-8000-00805f9b34fb", "Manufacturer Name String");
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}