package ltuproject.sailoraid.bluetooth;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.support.annotation.Nullable;
import android.support.annotation.StringDef;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import ltuproject.sailoraid.R;

import static java.lang.Thread.sleep;

/**
 * Created by Henrik on 2017-09-13.
 */

public class BTLEConnection extends Service {
    private final static String TAG = BTLEConnection.class.getSimpleName();

    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private String mBluetoothDeviceAddress;
    private BluetoothGatt mBluetoothGatt;
    private List<BluetoothGattService> supportedGattServices;
    private int mConnectionState = STATE_DISCONNECTED;

    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

    public final static String ACTION_GATT_CONNECTED =
            "ltuproject.sailoraid.bluetooth.BTLEConnection.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED =
            "ltuproject.sailoraid.bluetooth.BTLEConnection.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED =
            "ltuproject.sailoraid.bluetooth.BTLEConnection.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE =
            "ltuproject.sailoraid.bluetooth.BTLEConnection.ACTION_DATA_AVAILABLE";
    public final static String EXTRA_DATA =
            "ltuproject.sailoraid.bluetooth.BTLEConnection.EXTRA_DATA";
    public final static String EXTRA_TYPE =
            "ltuproject.sailoraid.bluetooth.BTLEConnection.EXTRA_TYPE";

    public final static UUID UUID_HEART_RATE_MEASUREMENT =
            UUID.fromString(SampleGattAttributes.HEART_RATE_MEASUREMENT);
    public final static UUID UUID_ACCELEROMETER_MEASUREMENT =
            UUID.fromString(SampleGattAttributes.ACCELEROMETER_MEASUREMENT);
    public final static UUID UUID_GPS_MEASUREMENT =
            UUID.fromString(SampleGattAttributes.HEART_RATE_MEASUREMENT);
    public final static UUID UUID_PRESSURE_MEASUREMENT =
            UUID.fromString(SampleGattAttributes.HEART_RATE_MEASUREMENT);
    public final static UUID UUID_IMU_ACCEL_MEASUREMENT =
            UUID.fromString(SampleGattAttributes.IMU_ACCEL_MEASUREMENT);
    public final static UUID UUID_IMU_GYRO_MEASUREMENT =
            UUID.fromString(SampleGattAttributes.IMU_GYRO_MEASUREMENT);

    private Context contx;

    public BTLEConnection(Context contx, BluetoothAdapter btAdapter){
        this.contx = contx;
        this.mBluetoothAdapter = btAdapter;
    }
    // Various callback methods defined by the BLE API.
    public final BluetoothGattCallback mGattCallback =
            new BluetoothGattCallback() {
                @Override
                public void onConnectionStateChange(BluetoothGatt gatt, int status,
                                                    int newState) {
                    String intentAction;
                    if (newState == BluetoothProfile.STATE_CONNECTED) {
                        intentAction = ACTION_GATT_CONNECTED;
                        mConnectionState = STATE_CONNECTED;
                        mBluetoothGatt = gatt;
                        broadcastUpdate(intentAction);
                        Log.i(TAG, "Connected to GATT server.");
                        Log.i(TAG, "Attempting to start service discovery:" +
                                mBluetoothGatt.discoverServices());

                    } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                        intentAction = ACTION_GATT_DISCONNECTED;
                        mConnectionState = STATE_DISCONNECTED;
                        Log.i(TAG, "Disconnected from GATT server.");
                        broadcastUpdate(intentAction);
                    }
                }

                @Override
                // New services discovered
                public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);
                    } else {
                        Log.w(TAG, "onServicesDiscovered received: " + status);
                    }
                }

                @Override
                // Result of a characteristic read operation
                public void onCharacteristicRead(BluetoothGatt gatt,
                                                 BluetoothGattCharacteristic characteristic,
                                                 int status) {
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
                    }
                }
                @Override
                public void onCharacteristicChanged(BluetoothGatt gatt,
                                                    BluetoothGattCharacteristic characteristic) {
                    broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
                }
            };

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public BluetoothGattCallback getGattCallback(){
        return mGattCallback;
    }
    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        contx.sendBroadcast(intent);
    }

    public List<BluetoothGattService> getSupportedGattServices() {
        if (mBluetoothGatt == null) return null;

        return mBluetoothGatt.getServices();
    }

    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.readCharacteristic(characteristic);
    }

    /**
     * Enables or disables notification on a give characteristic.
     *
     * @param characteristic Characteristic to act on.
     * @param enabled If true, enable notification.  False otherwise.
     */
    public void setCharacteristicNotification(BluetoothGattCharacteristic characteristic,
                                              boolean enabled) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.setCharacteristicNotification(characteristic, enabled);

        try {
            Thread.sleep(100);
        } catch (Exception e) {
            // ignore
        }

        // This is specific to Heart Rate Measurement.
        if (UUID_HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
                    UUID.fromString(SampleGattAttributes.CLIENT_CHARACTERISTIC_CONFIG));

            characteristic.addDescriptor(descriptor);
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            mBluetoothGatt.writeDescriptor(descriptor);
        } // 340a1b80-cf4b-11e1-ac36-0002a5d5c51b
        if (UUID_IMU_ACCEL_MEASUREMENT.equals(characteristic.getUuid())) {
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
                    UUID.fromString(SampleGattAttributes.CLIENT_CHARACTERISTIC_CONFIG));
            characteristic.addDescriptor(descriptor);
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            mBluetoothGatt.writeDescriptor(descriptor);
        }
        if (UUID_ACCELEROMETER_MEASUREMENT.equals(characteristic.getUuid())) {
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
                    UUID.fromString(SampleGattAttributes.CLIENT_CHARACTERISTIC_CONFIG));
            characteristic.addDescriptor(descriptor);
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            mBluetoothGatt.writeDescriptor(descriptor);
        }
        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void broadcastUpdate(final String action,
                                 final BluetoothGattCharacteristic characteristic) {
        final Intent intent = new Intent(action);

        // This is special handling for the Heart Rate Measurement profile. Data
        // parsing is carried out as per profile specifications.
        if (UUID_HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
            int flag = characteristic.getProperties();
            int format = -1;
            if ((flag & 0x01) != 0) {
                format = BluetoothGattCharacteristic.FORMAT_UINT16;
                Log.d(TAG, "Heart rate format UINT16.");
            } else {
                format = BluetoothGattCharacteristic.FORMAT_UINT8;
                Log.d(TAG, "Heart rate format UINT8.");
            }
            final int heartRate = characteristic.getIntValue(format, 1);
            final String charName = characteristic.getStringValue(format);
            Log.d(TAG, String.format("Received heart rate: %d", heartRate));
            intent.putExtra(EXTRA_TYPE, "Current rate: ");
            intent.putExtra(EXTRA_DATA, String.valueOf(heartRate));
        } else if (UUID_ACCELEROMETER_MEASUREMENT.equals(characteristic.getUuid())) {
                int flag = characteristic.getProperties();
                int format = -1;
                if ((flag & 0x01) != 0) {
                    format = BluetoothGattCharacteristic.FORMAT_FLOAT;
                    Log.d(TAG, "Accelerometer UINT16.");
                } else {
                    format = BluetoothGattCharacteristic.FORMAT_SFLOAT;
                    Log.d(TAG, "Accelerometer UINT8.");
                }
                byte[] recByte = characteristic.getValue();

                final float x = ((recByte[0]) | ((recByte[1])<<8));
                final float y = ((recByte[2]) | ((recByte[3])<<8));
                final float z = ((recByte[4]) | ((recByte[5])<<8));
            /*    final float x = characteristic.getFloatValue(format, 0);
                final float y = characteristic.getFloatValue(format, 2);
                final float z = characteristic.getFloatValue(format, 4);
*/
                final String charName = characteristic.getStringValue(format);
                //Log.d(TAG, String.format("Received Accelerometer: %f", x));
                intent.putExtra(EXTRA_TYPE, "Incline");
                intent.putExtra(EXTRA_DATA, String.valueOf(x) +":" +String.valueOf(y) +":" +String.valueOf(z));
                //intent.putExtra(EXTRA_DATA, "x:  " +String.valueOf(x) +"\n y: " + String.valueOf(y));

        }
        contx.sendBroadcast(intent);
    }
}