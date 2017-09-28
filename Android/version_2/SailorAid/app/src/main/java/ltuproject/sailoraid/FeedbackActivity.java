package ltuproject.sailoraid;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
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
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
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
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import ltuproject.sailoraid.bluetooth.AcceptConnection;
import ltuproject.sailoraid.bluetooth.BTHandler;
import ltuproject.sailoraid.bluetooth.BTLEConnection;
import ltuproject.sailoraid.bluetooth.SampleGattAttributes;

import static android.bluetooth.BluetoothDevice.BOND_BONDED;
import static java.lang.Math.abs;

/**
 * Created by Henrik on 2017-09-05.
 */

public class FeedbackActivity extends AppCompatActivity {

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
    private BluetoothGattService mGattService;

    /*
    Gyro from phone for demo
     */
    BoatView mBoatView;
    NeedleView mNeedleView;
    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private float x, y, z;
    private Button btnMap;
    //Location variables
    private static final String TAG_LAT = "lat";
    private static final String TAG_LONG = "lon";
    private String latitude = "65.534439";
    private String longitude = "22.386603";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.feedback_activity);

        btnMap = (Button) findViewById(R.id.mapviewbtn);

        initFilter();
        initBTconn();

        /**
         * Change to map activity and sending location coordinates to intent
         */
        btnMap.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                Intent intent = new Intent(FeedbackActivity.this, MapsActivity.class);
                intent.putExtra(TAG_LAT, latitude);
                intent.putExtra(TAG_LONG, longitude);
                startActivity(intent);
            }
        });
    }

    public void initBTconn(){
        myBTHandler = new BTHandler(this);
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

    /*
    Init filter for bluetooth update handling
     */
    private void initFilter(){

        filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        this.registerReceiver(mReceiver, filter);

        filter = new IntentFilter(BTLEConnection.ACTION_GATT_CONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_DISCONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_SERVICES_DISCOVERED);
        filter.addAction(BTLEConnection.ACTION_DATA_AVAILABLE);
        filter.addAction(BTLEConnection.ACTION_GATT_SERVICE_NOTIFIED);
        this.registerReceiver(mGattUpdateReceiver, filter);
    }
    @Override
    protected void onResume() {
        super.onResume();
        displayDynamicPics();
    }

    private void displayDynamicPics(){
        LinearLayout linearLayout = (LinearLayout) findViewById(R.id.boatalignmentholder);
        mBoatView = new BoatView(getApplicationContext(),
                BitmapFactory.decodeResource(getResources(), R.drawable.boat_alignement));
        mBoatView.setZOrderOnTop(true);    // necessary
        linearLayout.addView(mBoatView);

        LinearLayout linearPressureLayout = (LinearLayout) findViewById(R.id.pressureMeter);
        mNeedleView = new NeedleView(getApplicationContext(),
                BitmapFactory.decodeResource(getResources(), R.drawable.needle));
        mNeedleView.setZOrderOnTop(true);
        linearPressureLayout.addView(mNeedleView);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mReceiver);
        this.unregisterReceiver(mGattUpdateReceiver);
        cleanPop();
        myBTHandler.scanLeDevice(mLeScanCallback, false);
        myBTHandler.closeGatt();
    }

    @Override
    protected void onPause() {
        super.onPause();
        myBTHandler.scanLeDevice(mLeScanCallback, false);
        LinearLayout linearLayout = (LinearLayout) findViewById(R.id.boatalignmentholder);
        linearLayout.removeView(mBoatView);
        LinearLayout linearPressureLayout = (LinearLayout) findViewById(R.id.pressureMeter);
        linearPressureLayout.removeView(mNeedleView);
    }

    private void setDegreeText(int degree){
        TextView tv = (TextView) findViewById(R.id.degreeText);
        String ph = String.valueOf(degree) + "\u00B0";
        tv.setText(ph);
    }
    private void setPressureText(int pressure){
        TextView tv = (TextView) findViewById(R.id.pressureText);
        String ph = String.valueOf(pressure) + " Psi";
        tv.setText(ph);
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

     */
    public void getKnownGattServices(List<BluetoothGattService> gattServices){
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
                    LIST_NAME, SampleGattAttributes.
                            lookup(uuid, unknownServiceString));
            currentServiceData.put(LIST_UUID, uuid);
            gattServiceData.add(currentServiceData);

            if (uuid.equals(SampleGattAttributes.HEART_RATE_SERVICE.toString())) {
                myBTHandler.getBtLEConnection().setmGattService(gattService);
            }
            if (uuid.equals(SampleGattAttributes.ACCELEROMETER_SERVICE.toString())) {
                myBTHandler.getBtLEConnection().setmGattService(gattService);
            }
            if (uuid.equals(SampleGattAttributes.ENV_SERVICE.toString())) {
                myBTHandler.getBtLEConnection().setmGattService(gattService);
            }
            if (uuid.equals(SampleGattAttributes.NUCLEO_GPS_SERVICE.toString())) {
                myBTHandler.getBtLEConnection().setmGattService(gattService);
            }
        }
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
            else{
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
            TextView tv = (TextView) findViewById(R.id.callbackText);
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
                getKnownGattServices(myBTHandler.getBtLEConnection().getSupportedGattServices());
                //displayGattServices(myBTHandler.getBtLEConnection().getSupportedGattServices());
                //myBTHandler.registerGattNotifications();
                startRegNotifications();

            } else if (myBTHandler.getBtLEConnection().ACTION_DATA_AVAILABLE.equals(action)) {
                displayData(intent.getStringExtra(myBTHandler.getBtLEConnection().EXTRA_DATA),intent.getStringExtra(myBTHandler.getBtLEConnection().EXTRA_TYPE));
            } else if (myBTHandler.getBtLEConnection().ACTION_GATT_SERVICE_NOTIFIED.equals(action)){
                //myBTHandler.registerGattNotifications();
                startRegNotifications();
            }
        }
    };

    public void startRegNotifications() {
        Handler handler = new Handler();
        handler.post(new Runnable() {
            @Override
            public void run() {
                myBTHandler.getBtLEConnection().registerGattNotifications();
            }
        });
    }

    private void displayData(String data, String dataType) {
        if (data != null) {
            if(dataType.equals("Incline")){
                /*String[] accelerometer = data.split(":");
                String x = accelerometer[0]; // this will contain "Fruit"
                String y = accelerometer[1]; // this will contain " they taste good"
                String z = accelerometer[2]; // this will contain " they taste good"
                this.x = Float.parseFloat(x);
                this.y = Float.parseFloat(y);
                this.z = Float.parseFloat(z);*/

                this.x = Byte.parseByte(data);
                mBoatView.setXYZ(this.x, this.y, this.z);
                //mNeedleView.setPressure(abs(this.y)/10);
                //setDegreeText((int) (this.x));
                //setPressureText((int) abs(this.y)/10);
            }
            else if(dataType.equals("Temp")){
                this.x = Byte.parseByte(data);
                mBoatView.setXYZ(this.x, this.y, this.z);
                TextView tv = (TextView) findViewById(R.id.tempText);
                tv.setText(String.valueOf(this.x));
            }
            else if(dataType.equals("Pressure")){
                int pressure = Byte.parseByte(data);
                setPressureText(pressure);
                mNeedleView.setPressure(pressure/10);
            }
            else if(dataType.equals("Free Fall")){

            }
            else if(dataType.equals("Humidity")){
                int hum = Byte.parseByte(data);
                TextView tv = (TextView) findViewById(R.id.humText);
                tv.setText(String.valueOf(hum));
            }
            else if(dataType.equals("Heart")){
                this.y = Byte.parseByte(data);
                setDegreeText((int) this.y);
            }
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
