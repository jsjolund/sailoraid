package ltuproject.sailoraid;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.io.ObjectStreamClass;
import java.text.BreakIterator;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import ltuproject.sailoraid.bluetooth.AcceptConnection;
import ltuproject.sailoraid.bluetooth.BTHandler;
import ltuproject.sailoraid.bluetooth.BTLEConnection;
import ltuproject.sailoraid.bluetooth.SampleGattAttributes;

import static android.bluetooth.BluetoothDevice.BOND_BONDED;

public class BTConnectActivity extends AppCompatActivity {

    private static final String PAIRED_LIST = "Paired devices";
    private static final String DISCOVERED_LIST = "Discovered devices";
    private static final String LE_LIST = "LE devices";
    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";
    private SampleGattAttributes sampleGattAttributes;

    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_COARSE_LOCATION = 999;
    private static final long SCAN_PERIOD = 10000;
    private boolean mConnected = false;
    private BluetoothAdapter btAdapter;
    private Button searchbtn, pairedbtn, listenbtn, searchLEbtn, connectLEbtn;
    private ArrayAdapter<String> BTArrayAdapter;
    private IntentFilter filter;
    private AlertDialog.Builder popDialog;
    private AlertDialog alertpop;
    private BluetoothDevice chosenDevice;
    private AcceptConnection btServer;
    private boolean hasPermission;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics;
    private BTHandler myBTHandler;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.connect_activity);

        // Register for broadcasts when a device is discovered.
        filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        this.registerReceiver(mReceiver, filter);

        filter = new IntentFilter(BTLEConnection.ACTION_GATT_CONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_DISCONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_SERVICES_DISCOVERED);
        filter.addAction(BTLEConnection.ACTION_DATA_AVAILABLE);
        this.registerReceiver(mGattUpdateReceiver, filter);

        //btAdapter = BluetoothAdapter.getDefaultAdapter();

        pairedbtn = (Button)findViewById(R.id.paireddevicebtn);
        searchbtn = (Button)findViewById(R.id.searchdevicebtn);
        listenbtn = (Button)findViewById(R.id.listenForDevicebtn);
        searchLEbtn = (Button)findViewById(R.id.searchLe);
        connectLEbtn = (Button)findViewById(R.id.connectLe);

        //Create a handler for bluetooth search and managing
        myBTHandler = new BTHandler(this);

        assert pairedbtn != null;
        pairedbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(!myBTHandler.getBtAdapter().isEnabled()) {
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
                if(!myBTHandler.getBtAdapter().isEnabled()) {
                    turnOn();
                }
                checkLocationPermission();
                if (hasPermission){
                    if ( myBTHandler.getBtAdapter().isDiscovering()){
                        myBTHandler.getBtAdapter().cancelDiscovery();
                    }
                    myBTHandler.getBtAdapter().startDiscovery();
                }
            }
        });

        assert listenbtn != null;
        listenbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(!myBTHandler.getBtAdapter().isEnabled()) {
                    turnOn();
                }
                btServer = new AcceptConnection(myBTHandler.getBtAdapter());
                btServer.run();
            }
        });

        assert searchLEbtn != null;
        searchLEbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                if(!myBTHandler.getBtAdapter().isEnabled()) {
                    turnOn();
                }
                checkLocationPermission();
                if(hasPermission){
                    cleanPop();
                    setPopDialog(LE_LIST);
                    BTArrayAdapter.clear();
                    alertpop = popDialog.show();
                    myBTHandler.clearLeList();
                    myBTHandler.scanLeDevice(mLeScanCallback, true);
                }
            }
        });

        assert connectLEbtn != null;
        connectLEbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                myBTHandler.registerGattNotifications();
            }
        });
    }

    /*
    Popup when searching for bluetooth devices
     */
    public void setPopDialog(String type) {

        BTArrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
        LayoutInflater inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        View Viewlayout = inflater.inflate(R.layout.bt_list, (ViewGroup) findViewById(R.id.bt_list));
        ListView myListView = (ListView) Viewlayout.findViewById(R.id.BTList);
        myListView.setAdapter(BTArrayAdapter);
        myListView.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            public void onItemClick(AdapterView<?> parent, View view,int position, long id) {
                //Highlight chosen item on the popup list
                view.setBackgroundColor(Color.LTGRAY);
                String device = parent.getAdapter().getItem(position).toString();
                String lines[] = device.split("[\\r\\n]+");
                String deviceName = lines[0];
                String deviceAddress = lines[1];
                chosenDevice = myBTHandler.getDevice(deviceName, deviceAddress);
            }
        });
        popDialog = new AlertDialog.Builder(this);
        popDialog.setView(Viewlayout);
        popDialog.setTitle(type);
        if(!type.equals(PAIRED_LIST)){
            popDialog.setNeutralButton("Pair",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                            if (chosenDevice != null){
                                myBTHandler.createBond(chosenDevice);
                            }
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
                                myBTHandler.closeGatt();
                                myBTHandler.startNewBTLEConnection();
                                myBTHandler.connectToLEDevice(chosenDevice);
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
                                myBTHandler.startConnection(chosenDevice);
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

    /*
    Updates the adapter that is on the Listview displayed in the popup window with lists of found devices
     */
    public void updateDevicesShown(String text) {
        BTArrayAdapter.clear();

        if (text.equals(PAIRED_LIST)){
            myBTHandler.setPairedList();
            // put it's one to the adapter
            for(BluetoothDevice device : myBTHandler.getPairedDevices())
                BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());

        }
        else if(text.equals(DISCOVERED_LIST)){
            for (BluetoothDevice device : myBTHandler.getNewDeviceList()){
                BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());
            }
        }
        else if(text.equals(LE_LIST)){
            for (BluetoothDevice device : myBTHandler.getLeDeviceList()){
                BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());
            }
        }
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
                        if (!myBTHandler.deviceExists(myBTHandler.getNewDeviceList(), device)){
                        myBTHandler.addToDevices(device);
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
                //Start searching clean popup before show or it will crash
                else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)){
                    cleanPop();
                    setPopDialog(DISCOVERED_LIST);
                    alertpop = popDialog.show();
                }
                else if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)){
                    final int bluetoothState = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                            BluetoothAdapter.ERROR);
                    switch (bluetoothState) {
                        case BluetoothAdapter.STATE_ON:
                            break;
                    }
                }
            }
    };

    /*
    Displays all services provided by connected ble device not really needed in the end
     */
    private void displayGattServices(List<BluetoothGattService> gattServices) {
        if (gattServices == null) return;
        String uuid = null;
        String unknownServiceString = getResources().
                getString(R.string.unknown_service);
        String unknownCharaString = getResources().
                getString(R.string.unknown_characteristic);
        ArrayList<HashMap<String, String>> gattServiceData =
                new ArrayList<HashMap<String, String>>();
        ArrayList<ArrayList<HashMap<String, String>>> gattCharacteristicData
                = new ArrayList<ArrayList<HashMap<String, String>>>();
        mGattCharacteristics =
                new ArrayList<ArrayList<BluetoothGattCharacteristic>>();

        // Loops through available GATT Services.
        for (BluetoothGattService gattService : gattServices) {
            HashMap<String, String> currentServiceData =
                    new HashMap<String, String>();
            uuid = gattService.getUuid().toString();
            currentServiceData.put(
                    LIST_NAME, sampleGattAttributes.
                            lookup(uuid, unknownServiceString));
            currentServiceData.put(LIST_UUID, uuid);
            gattServiceData.add(currentServiceData);

            ArrayList<HashMap<String, String>> gattCharacteristicGroupData =
                    new ArrayList<HashMap<String, String>>();
            List<BluetoothGattCharacteristic> gattCharacteristics =
                    gattService.getCharacteristics();
            ArrayList<BluetoothGattCharacteristic> charas =
                    new ArrayList<BluetoothGattCharacteristic>();
            // Loops through available Characteristics.
            for (BluetoothGattCharacteristic gattCharacteristic :
                    gattCharacteristics) {
                charas.add(gattCharacteristic);
                HashMap<String, String> currentCharaData =
                        new HashMap<String, String>();
                uuid = gattCharacteristic.getUuid().toString();
                currentCharaData.put(
                        LIST_NAME, sampleGattAttributes.lookup(uuid,
                                unknownCharaString));
                currentCharaData.put(LIST_UUID, uuid);
                gattCharacteristicGroupData.add(currentCharaData);
            }
            mGattCharacteristics.add(charas);
            myBTHandler.setmGattCharacteristics(mGattCharacteristics);
            gattCharacteristicData.add(gattCharacteristicGroupData);

        }
        TextView tv1 = (TextView) findViewById(R.id.uuidText);
        for (int i =0;i<gattServiceData.size();i++){
            tv1.append(gattServiceData.get(i).toString() +"\n");
        }
        TextView tv2 = (TextView) findViewById(R.id.chatData);
        tv2.setText(gattCharacteristicData.get(0).get(0).toString());
    }

    /*
    Get results from Bluetooth LE device scan and adds to list on BTHandler and updates adapter so the item will be displayed on the popup window
     */
    private ScanCallback mLeScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            BluetoothDevice bd = result.getDevice();
            if(!myBTHandler.deviceExists(myBTHandler.getLeDeviceList(), bd)){
                myBTHandler.addLeDeviceList(bd);
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

    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device. This can be a
    // result of read or notification operations.
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            TextView tv = (TextView) findViewById(R.id.connectedtext);
            if (myBTHandler.getBtLEConnection().ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                tv.setText(R.string.connected);
                invalidateOptionsMenu();
            } else if (myBTHandler.getBtLEConnection().ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                tv.setText(R.string.disconnected);
                invalidateOptionsMenu();
            } else if (myBTHandler.getBtLEConnection().
                    ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the
                // user interface.

                displayGattServices(myBTHandler.getBtLEConnection().getSupportedGattServices());


            } else if (myBTHandler.getBtLEConnection().ACTION_DATA_AVAILABLE.equals(action)) {
                displayData(intent.getStringExtra(myBTHandler.getBtLEConnection().EXTRA_DATA),intent.getStringExtra(myBTHandler.getBtLEConnection().EXTRA_TYPE));
            }
        }
    };

    private void displayData(String data, String dataType) {
        if (data != null) {
            TextView tv1 = (TextView) findViewById(R.id.charName);
            TextView tv = (TextView) findViewById(R.id.chatData);
            tv.setText(data);
            tv1.setText(dataType);
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
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        this.registerReceiver(mReceiver, filter);
        filter = new IntentFilter(BTLEConnection.ACTION_GATT_CONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_DISCONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_SERVICES_DISCOVERED);
        filter.addAction(BTLEConnection.ACTION_DATA_AVAILABLE);
        this.registerReceiver(mGattUpdateReceiver, filter);
    }

    @Override
    protected void onDestroy() {
        if (mReceiver !=null){
            this.unregisterReceiver(mReceiver);
        }
        if (mGattUpdateReceiver != null){
            this.unregisterReceiver(mGattUpdateReceiver);
        }
        cleanPop();
        myBTHandler.closeGatt();
        super.onDestroy();

    }

    @Override
    protected  void onPause() {
        this.unregisterReceiver(mReceiver);
        this.unregisterReceiver(mGattUpdateReceiver);
        cleanPop();
        super.onPause();

    }

    public void turnOn(){
        Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        this.startActivityForResult(turnOn, REQUEST_ENABLE_BT);
        while(!myBTHandler.getBtAdapter().isEnabled()){

        }
    }

    public void turnOff(){
        myBTHandler.getBtAdapter().disable();
    }

}