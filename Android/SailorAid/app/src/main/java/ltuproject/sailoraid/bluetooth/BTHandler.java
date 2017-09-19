package ltuproject.sailoraid.bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.content.Context;
import android.content.IntentFilter;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.widget.ArrayAdapter;

import java.util.ArrayList;
import java.util.Set;


import static android.bluetooth.BluetoothDevice.BOND_BONDED;

/**
 * Created by Henrik on 2017-09-13.
 */

public class BTHandler {

    private static final String PAIRED_LIST = "Paired devices";
    private static final String DISCOVERED_LIST = "Discovered devices";
    private static final String LE_LIST = "LE devices";


    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_COARSE_LOCATION = 999;
    private static final long SCAN_PERIOD = 10000;

    private BluetoothAdapter btAdapter;
    private Set<BluetoothDevice> pairedDevices;
    private ArrayList<BluetoothDevice> newDeviceList;
    private ArrayList<BluetoothDevice> leDeviceList;
    private BluetoothDevice newDevice;
    private BluetoothGatt mBluetoothGatt;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics;
    private BluetoothGattCharacteristic characteristic;
    private BluetoothGattCharacteristic mNotifyCharacteristic;
    private BTLEConnection btLEConnection;
    private BTConnection btConnection;

    private boolean mScanning;
    private Handler mHandler;

    private Context contx;

    public BTHandler(Context contx){
        this.btAdapter = BluetoothAdapter.getDefaultAdapter();
        newDeviceList = new ArrayList<BluetoothDevice>();
        leDeviceList = new ArrayList<BluetoothDevice>();
        mHandler = new Handler();
        this.contx = contx;
    }

    public BluetoothDevice getDevice(String name, String address){
        BluetoothDevice tmp = null;
        if (newDeviceList != null){
            for (int i=0;i<newDeviceList.size();i++){
                tmp = newDeviceList.get(i);
                if (tmp.getAddress().equals(address)){
                    break;
                }
            }
        }
        if (pairedDevices != null){
            for(BluetoothDevice device : pairedDevices){
                tmp = device;
                if (tmp.getAddress().equals(address)){
                    break;
                }
            }
        }
        if (leDeviceList != null){
            for (int i=0;i<leDeviceList.size();i++){
                tmp = leDeviceList.get(i);
                if (tmp.getAddress().equals(address)){
                    break;
                }
            }
        }
        return tmp;
    }

    public void setPairedList(){
        pairedDevices = btAdapter.getBondedDevices();
    }
    public void createBond(BluetoothDevice bd){
        if (bd !=null) {
            boolean isPaired = (bd.getBondState() == BOND_BONDED);
            if (!isPaired) {
                bd.createBond();
            }
        }
    }
    public void startConnection(BluetoothDevice bd){
        boolean isPaired = (bd.getBondState() == BOND_BONDED);

        if (bd !=null) {
            if (!isPaired) {
                bd.createBond();
            }
            newDevice = bd;
        }
        btConnection = new BTConnection(newDevice, btAdapter);
    }

    public void startNewBTLEConnection(){
        btLEConnection = new BTLEConnection(contx, btAdapter);
    }
    public void connectToLEDevice(final BluetoothDevice device) {
        Handler handler = new Handler(contx.getMainLooper());
        handler.post(new Runnable() {
            @Override
            public void run() {

                if (device != null) {
                    mBluetoothGatt = device.connectGatt(contx, false, btLEConnection.getGattCallback());
                }
            }
        });
    }

    public void setmGattCharacteristics(ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics){
        this.mGattCharacteristics = mGattCharacteristics;
    }
    public void registerGattNotifications(){
        BluetoothGattCharacteristic characteristic = null;
        for (int i=0; i<mGattCharacteristics.size(); i++){
            ArrayList<BluetoothGattCharacteristic> tmp = mGattCharacteristics.get(i);
            for (int j=0; j<tmp.size(); j++) {
                characteristic = tmp.get(j);
                btLEConnection.setCharacteristicNotification(characteristic, true);
                final int charaProp = characteristic.getProperties();
                if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                    // If there is an active notification on a characteristic, clear
                    // it first so it doesn't update the data field on the user interface.
                    if (mNotifyCharacteristic != null) {
                        btLEConnection.setCharacteristicNotification(
                                mNotifyCharacteristic, false);
                        mNotifyCharacteristic = null;
                    }
                    btLEConnection.readCharacteristic(characteristic);
                }
                if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                    mNotifyCharacteristic = characteristic;
                    btLEConnection.setCharacteristicNotification(
                            characteristic, true);
                }
            }
        }
    }

    public boolean deviceExists(ArrayList<BluetoothDevice> list, BluetoothDevice device){
        boolean exists = false;
        for (int i=0;i<list.size();i++){
            if (list.get(i).equals(device)){
                exists = true;
                break;
            }
        }
        return exists;
    }

    public void closeGatt() {
        if (mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;
    }

    /*
    Scan for Bluetooth LE devices and sends result to a Scancallback
     */
    public void scanLeDevice(final ScanCallback mLeScanCallback, final boolean enable) {

        final BluetoothLeScanner bluetoothLeScanner = btAdapter.getBluetoothLeScanner();

        if (enable) {
            // Stops scanning after a pre-defined scan period.
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mScanning = false;

                    bluetoothLeScanner.stopScan(mLeScanCallback);
                }
            }, SCAN_PERIOD);

            mScanning = true;
            bluetoothLeScanner.startScan(mLeScanCallback);
        } else {
            mScanning = false;
            bluetoothLeScanner.stopScan(mLeScanCallback);
        }
    }

    public void addToDevices(BluetoothDevice device){
        newDeviceList.add(device);
    }

    public void addLeDeviceList(BluetoothDevice device){
        leDeviceList.add(device);
    }

    public BluetoothAdapter getBtAdapter(){
        return btAdapter;
    }

    public ArrayList<BluetoothDevice> getNewDeviceList(){
        return newDeviceList;
    }

    public ArrayList<BluetoothDevice> getLeDeviceList(){
        return  leDeviceList;
    }

    public BTLEConnection getBtLEConnection(){
        return btLEConnection;
    }
    public void clearLeList(){
        leDeviceList.clear();
    }

    public void clearNewDevieList(){
        newDeviceList.clear();
    }
    public Set<BluetoothDevice> getPairedDevices(){
        return pairedDevices;
    }
}
