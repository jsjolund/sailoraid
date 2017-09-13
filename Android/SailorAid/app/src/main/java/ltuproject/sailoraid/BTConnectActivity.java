package ltuproject.sailoraid;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.UUID;
import java.util.zip.Inflater;

import ltuproject.sailoraid.bluetooth.AcceptConnection;
import ltuproject.sailoraid.bluetooth.BTConnection;
import ltuproject.sailoraid.bluetooth.BTLEConnection;

import static android.bluetooth.BluetoothDevice.BOND_BONDED;

public class BTConnectActivity extends AppCompatActivity {

    private static final String PAIRED_LIST = "Paired devices";
    private static final String DISCOVERED_LIST = "Discovered devices";
    private static final String LE_LIST = "LE devices";


    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_COARSE_LOCATION = 999;
    private static final long SCAN_PERIOD = 10000;

    private BluetoothAdapter btAdapter;
    private Button searchbtn, pairedbtn, listenbtn, searchLEbtn, connectLEbtn;
    private Set<BluetoothDevice> pairedDevices;
    private ArrayList<BluetoothDevice> newDeviceList;
    private ArrayList<BluetoothDevice> leDeviceList;
    private BluetoothDevice newDevice;
    private ArrayAdapter<String> BTArrayAdapter;
    private IntentFilter filter;
    private AlertDialog.Builder popDialog;
    private AlertDialog alertpop;
    private BluetoothDevice chosenDevice;
    private String deviceName, deviceaddress;

    private BTConnection btConnection;
    private BTLEConnection btLEConnection;
    private AcceptConnection btServer;
    private BluetoothGatt mBluetoothGatt;
    private byte[] readBuffer;
    private boolean hasPermission;

    private boolean mScanning;
    private Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.connect_activity);

        // Register for broadcasts when a device is discovered.
        filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        this.registerReceiver(mReceiver, filter);

        btAdapter = BluetoothAdapter.getDefaultAdapter();

        pairedbtn = (Button)findViewById(R.id.paireddevicebtn);
        searchbtn = (Button)findViewById(R.id.searchdevicebtn);
        listenbtn = (Button)findViewById(R.id.listenForDevicebtn);
        searchLEbtn = (Button)findViewById(R.id.searchLe);
        connectLEbtn = (Button)findViewById(R.id.connectLe);

        newDeviceList = new ArrayList<>();
        leDeviceList = new ArrayList<>();
        mHandler = new Handler();


        assert pairedbtn != null;
        pairedbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                if(!btAdapter.isEnabled()) {
                    turnOn();
                }
                cleanPop();
                setPopDialog(PAIRED_LIST);
                updateDevicesShown(PAIRED_LIST);
                alertpop = popDialog.show();
            }
        });

        assert searchbtn != null;
        searchbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                if(!btAdapter.isEnabled()) {
                    turnOn();
                }
                //newDeviceList.clear();
                checkLocationPermission();
                if (hasPermission){
                    if ( btAdapter.isDiscovering()){
                        btAdapter.cancelDiscovery();
                    }
                    newDeviceList.clear();
                    btAdapter.startDiscovery();
                }
            }
        });

        assert listenbtn != null;
        listenbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                if(!btAdapter.isEnabled()) {
                    turnOn();
                }
                btServer = new AcceptConnection(btAdapter);
                btServer.run();
            }
        });

        assert searchLEbtn != null;
        searchLEbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                if(!btAdapter.isEnabled()) {
                    turnOn();
                }
                scanLeDevice(true);
            }
        });
    }

    public void setPopDialog(String type) {
        //Popup when searching for bluetooth devices
        BTArrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
        LayoutInflater inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        View Viewlayout = inflater.inflate(R.layout.bt_list, (ViewGroup) findViewById(R.id.bt_list));
        ListView myListView = (ListView) Viewlayout.findViewById(R.id.BTList);
        myListView.setAdapter(BTArrayAdapter);
        myListView.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            public void onItemClick(AdapterView<?> parent, View view,int position, long id) {
                String device = parent.getAdapter().getItem(position).toString();
                String lines[] = device.split("[\\r\\n]+");
                String deviceName = lines[0];
                String deviceaddress = lines[1];
                chosenDevice = getDevice(deviceName, deviceaddress);
            }
        });
        popDialog = new AlertDialog.Builder(this);
        popDialog.setView(Viewlayout);

        if(!type.equals(PAIRED_LIST)){
            popDialog.setNeutralButton("Pair",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                            if (chosenDevice != null){
                                createBond(chosenDevice);
                            }
                            //btDevice.run();
                        }
                    });
        }
        popDialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.dismiss();
                    }
                }
        );
        if (type.equals(LE_LIST)){
            popDialog.setPositiveButton("Connect",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                            if (chosenDevice != null){
                                btLEConnection = new BTLEConnection();
                                mBluetoothGatt = chosenDevice.connectGatt(getApplicationContext(), false, btLEConnection.getGattCallback());
                            }
                        }
                    });
        }
        else {
            popDialog.setPositiveButton("Connect",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                            if (chosenDevice != null){
                                startConnection(chosenDevice);
                            }
                        }
                    });
        }
        popDialog.create();
    }

    public void cleanPop(){
        if (popDialog != null){

            popDialog = null;
        }
        if (alertpop != null) {
            alertpop.dismiss();
            alertpop = null;
        }
    }

    public void updateDevicesShown(String text) {
        BTArrayAdapter.clear();
        popDialog.setTitle(text);

        if (text.equals(PAIRED_LIST)){
            pairedDevices = btAdapter.getBondedDevices();
            // put it's one to the adapter
            for(BluetoothDevice device : pairedDevices)
                BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());

        }
        else if(text.equals(DISCOVERED_LIST)){
            for (BluetoothDevice device : newDeviceList){
                    String name = device.getName();
                    BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());
            }
        }
    }

    private BluetoothDevice getDevice(String name, String address){
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

    private void createBond(BluetoothDevice bd){
        if (bd !=null) {
            boolean isPaired = (bd.getBondState() == BOND_BONDED);
            if (!isPaired) {
                bd.createBond();
            }
        }
    }
    private void startConnection(BluetoothDevice bd){
        boolean isbtElement = false;
        boolean isPaired = (bd.getBondState() == BOND_BONDED);

        if (bd !=null) {
            if (!isPaired) {
                bd.createBond();
            }
            newDevice = bd;
        }
        else{
            TextView connectTv = (TextView)findViewById(R.id.connectedtext);
            connectTv.setText("Not Connected!");
        }
        btConnection = new BTConnection(newDevice, btAdapter);
    }

    // Create a BroadcastReceiver for ACTION_FOUND.
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();

        // When discovery finds a device
        if (BluetoothDevice.ACTION_FOUND.equals(action)) {
            // Get the BluetoothDevice object from the Intent
            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            // If it's already paired, skip it, because it's been listed already
            if (device.getBondState() != BOND_BONDED)
            {
                 if(!deviceExists(newDeviceList, device)){
                    String name = device.getName();
                    newDeviceList.add(device);
                }
            }
            updateDevicesShown(DISCOVERED_LIST);
            BTArrayAdapter.notifyDataSetChanged();

        }
        else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)){
            if (btAdapter.isDiscovering()){
                btAdapter.cancelDiscovery();
            }
        }
        else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)){
            cleanPop();
            setPopDialog(DISCOVERED_LIST);
            alertpop = popDialog.show();
        }
            }
    };

    private boolean deviceExists(ArrayList<BluetoothDevice> list, BluetoothDevice device){
        boolean exists = false;
        for (int i=0;i<list.size();i++){
            if (list.get(i).equals(device)){
                exists = true;
                break;
            }
        }
        return exists;
    }
    private ScanCallback mLeScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            BluetoothDevice bd = result.getDevice();
            if(!deviceExists(leDeviceList, bd)){
                leDeviceList.add(bd);
                result.getRssi();
                BTArrayAdapter.add(bd.getName()+ "\n" + bd.getAddress());
                BTArrayAdapter.notifyDataSetChanged();
            }
        }

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            super.onBatchScanResults(results);
        }

        @Override
        public void onScanFailed(int errorCode) {
            super.onScanFailed(errorCode);
        }
    };

    private void scanLeDevice(final boolean enable) {

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
            cleanPop();
            setPopDialog(LE_LIST);
            BTArrayAdapter.clear();
            leDeviceList.clear();
            popDialog.setTitle("LE devices");
            alertpop = popDialog.show();
            bluetoothLeScanner.startScan(mLeScanCallback);
        } else {
            mScanning = false;
            bluetoothLeScanner.stopScan(mLeScanCallback);
        }
    }
    /*
    Check permissions during runtime for bluetooth discovery
     */
    protected void checkLocationPermission() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                    REQUEST_COARSE_LOCATION);
        }
        else{
            hasPermission = true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case REQUEST_COARSE_LOCATION: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    hasPermission = true;
                } else {
                    hasPermission = false;
                }
                break;
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        this.registerReceiver(mReceiver, filter);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mReceiver);
        cleanPop();
    }

    @Override
    protected  void onPause() {
        super.onPause();
        this.unregisterReceiver(mReceiver);
        cleanPop();
    }

    public void turnOn(){
        Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        this.startActivityForResult(turnOn, REQUEST_ENABLE_BT);
    }


    public void turnOff(){
        btAdapter.disable();
    }

}