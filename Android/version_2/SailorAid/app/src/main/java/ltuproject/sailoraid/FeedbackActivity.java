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
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.model.LatLng;

import org.w3c.dom.Text;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Random;

import ltuproject.sailoraid.bluetooth.BTHandler;
import ltuproject.sailoraid.bluetooth.BTLEConnection;
import ltuproject.sailoraid.bluetooth.SampleGattAttributes;
import ltuproject.sailoraid.datalog.SailLog;
import ltuproject.sailoraid.graphics.BoatView;
import ltuproject.sailoraid.graphics.NeedleView;
import ltuproject.sailoraid.graphics.RotatableGLView;

import static android.bluetooth.BluetoothDevice.BOND_BONDED;
import static java.lang.Math.abs;
import static java.lang.Math.random;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_COMPASS;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_FREE_FALL;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_HUMIDITY;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_INCLINE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_POSITION;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_PRESSURE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_SOG;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_TEMPERATURE;

/**
 * Created by Henrik on 2017-09-05.
 */

public class FeedbackActivity extends AppCompatActivity {

    private static final String PAIRED_LIST = "Paired devices";
    private static final String DISCOVERED_LIST = "Discovered devices";
    private static final String LE_LIST = "LE devices";


    private static final float BOAT_SCALE_X = 2.8f;
    private static final float BOAT_SCALE_Y = 2.8f;
    private static final float NEEDLE_SCALE_X = 2.8f;
    private static final float NEEDLE_SCALE_Y = 1.0f;
    private static final float COMPASS_SCALE_X = 3.2f;
    private static final float COMPASS_SCALE_Y = 3.2f;
    private static final float COMPASS_BOAT_SCALE_X = 0.5f;
    private static final float COMPASS_BOAT_SCALE_Y = 0.6f;

    private static final float DRIFT_ARROW_SCALE_X = 1.0f;
    private static final float DRIFT_ARROW_SCALE_Y = 3.0f;

    private static final float NEEDLE_BOTTOM_POS = -1.5f;
    private static final float DRIFT_ARROW_CENTER = 3.6f;
    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";
    private SampleGattAttributes sampleGattAttributes;
    private float currentIncline = 0;
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
    private boolean hasPermission;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics;
    private BTHandler myBTHandler;
    private RotatableGLView mInclineBoatView;
    private RotatableGLView mCompassView;
    private RotatableGLView mBoatDriftView;
    private RotatableGLView mPressureNeedleView;
    private RotatableGLView mCompassBoatView;
    private RotatableGLView mLeftDriftView;
    private RotatableGLView mRightDriftView;
    private SailLog log;
    private boolean logging = false;
    /*
    Gyro from phone for demo
     */
    private float x, y, z;
    private Button btnMap;
    //Location variables

    static private List<LatLng> travelRoute = new ArrayList<LatLng>();
    public static synchronized void getRoute(List<LatLng> output) {
        output.addAll(travelRoute);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.feedback_activity);

        btnMap = (Button) findViewById(R.id.mapviewbtn);

        initFilter();
        initBTConn();
        displayDynamicPics();

        Toolbar myToolbar = (Toolbar) findViewById(R.id.feedback_toolbar);
        setSupportActionBar(myToolbar);
        /**
         * Change to map activity and sending location coordinates to intent
         */
        btnMap.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                Intent intent = new Intent(FeedbackActivity.this, MapsActivity.class);
                startActivity(intent);
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mReceiver);
        this.unregisterReceiver(mGattUpdateReceiver);
        cleanPop();
        myBTHandler.scanLeDevice(mLeScanCallback, false);
        myBTHandler.closeGatt();
        myBTHandler = null;
        removeDynamicPics();
    }

    @Override
    protected void onPause() {
        super.onPause();
        myBTHandler.scanLeDevice(mLeScanCallback, false);

    }

    public void initBTConn(){
        myBTHandler = new BTHandler(this);
        if(!myBTHandler.getBtAdapter().isEnabled()) {
            turnOn();
        }
        checkLocationPermission();
        if(hasPermission){
            //cleanPop();
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

    private float dipToPixels(int dipValue){
        final float scale = getResources().getDisplayMetrics().scaledDensity;
        float scaledSize = (getResources().getDimensionPixelSize(dipValue) / scale);
        return scaledSize;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.feedback_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.start_log:
                if (!logging){
                    log = new SailLog(this);
                    log.initLogData();
                    writeCrapToLog();
                    //log.writeToLog("Yoyo");
                    logging = true;
                    Toast.makeText(getApplicationContext(), "Logging started!", Toast.LENGTH_SHORT).show();
                    item.setIcon(getDrawable(R.drawable.loggo));
                } else{
                    item.setIcon(getDrawable(R.drawable.loggo));
                    logging = false;
                }
                // User chose the "Settings" item, show the app settings UI...
                return true;

            case R.id.stop_log:
                if (logging){
                    //log.stopLogData();
                    //log.readLog();
                    TextView tv = (TextView) findViewById(R.id.feedbackText);
                    logging = false;
                }
                // User chose the "Favorite" action, mark the current item
                // as a favorite...
                return true;

            default:
                // If we got here, the user's action was not recognized.
                // Invoke the superclass to handle it.
                return super.onOptionsItemSelected(item);

        }
    }

    private void displayDynamicPics(){
        LinearLayout linearLayout = (LinearLayout) findViewById(R.id.boatalignmentholder);
         mInclineBoatView = new RotatableGLView(this, BitmapFactory.decodeResource(getResources(),
                 R.drawable.boat_alignement),
                 BOAT_SCALE_X, BOAT_SCALE_Y);
        linearLayout.addView(mInclineBoatView.getGlView());

        mInclineBoatView.moveGL(0, -0.3f);
        LinearLayout linearPressureLayout = (LinearLayout) findViewById(R.id.pressureMeter);
        mPressureNeedleView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.needle),
                NEEDLE_SCALE_X, NEEDLE_SCALE_Y);
        linearPressureLayout.addView(mPressureNeedleView.getGlView());

        // Range of move needle Min moveGL -1.5f, max moveGl 1.2f
        mPressureNeedleView.moveGL(0,NEEDLE_BOTTOM_POS);
        LinearLayout linearCompassLayout = (LinearLayout) findViewById(R.id.driftImg);
        mCompassView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.compass), BitmapFactory.decodeResource(getResources(), R.drawable.rowboat),
                COMPASS_SCALE_Y, COMPASS_SCALE_X, COMPASS_BOAT_SCALE_X, COMPASS_BOAT_SCALE_Y);
        linearCompassLayout.addView(mCompassView.getGlView());
        mCompassView.moveGL2(0.02f,0);
        LinearLayout leftDriftLayout = (LinearLayout) findViewById(R.id.leftDrift);
        mLeftDriftView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.left_drift_arrow),
                DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        leftDriftLayout.addView(mLeftDriftView.getGlView());

        LinearLayout rightDriftLayout = (LinearLayout) findViewById(R.id.rightDrift);
        mRightDriftView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.right_drift_arrow),
                DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        rightDriftLayout.addView(mRightDriftView.getGlView());
        mLeftDriftView.moveGL(DRIFT_ARROW_CENTER,0);
        mRightDriftView.moveGL(-DRIFT_ARROW_CENTER,0);
        // Max resize ARROW_SCALE*x = 12 also move from center with -x/6
        /*float x = 12f;
        mLeftDriftView.resizeGL(DRIFT_ARROW_SCALE_X*x, DRIFT_ARROW_SCALE_Y);
        mLeftDriftView.moveGL(DRIFT_ARROW_CENTER-x/6, 0);

        mRightDriftView.resizeGL(DRIFT_ARROW_SCALE_X*x, DRIFT_ARROW_SCALE_Y);
        mRightDriftView.moveGL(-DRIFT_ARROW_CENTER+x/6, 0);*/
    }

    private void removeDynamicPics(){
        mInclineBoatView.clearRenderer();
        mCompassView.clearRenderer();
        mPressureNeedleView.clearRenderer();
        mInclineBoatView = null;
        mCompassView = null;
        mPressureNeedleView = null;
    }

    private void setTiltText(float degree){
        TextView tv = (TextView) findViewById(R.id.tiltText);
        String ph = String.format("%.1f\u00B0", degree);
        tv.setText(ph);
    }
    private void setPressureText(float pressure){
        TextView tv = (TextView) findViewById(R.id.pressureText);
        String ph = String.format("%.1f Psi", pressure);
        tv.setText(ph);
    }
    private void setTempText(float degree){
        TextView tv = (TextView) findViewById(R.id.tempText);
        String ph = String.format("%.1f\u00B0C", degree);
        tv.setText(ph);
    }
    private void setHumText(float degree){
        TextView tv = (TextView) findViewById(R.id.humText);
        String ph = String.format("%.1f%%", degree);
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
            if(dataType.equals(DATA_TYPE_INCLINE)){
                // Get Roll, pitch and yaw
                String[] accelerometer = data.split(":");

                this.x = Float.parseFloat(accelerometer[0]);
                this.y = Float.parseFloat(accelerometer[1]);
                this.z = Float.parseFloat(accelerometer[2]);


                mInclineBoatView.rotateGl(this.x);
                // Rotates compass with pitch
                mCompassView.rotateGl(this.y);
                // Rotates Boat bearing with Yaw
                mCompassView.rotateGl2(this.z);
                // Use pitch for testing drifting feedback
                if(this.y >=0){
                    // Max resize ARROW_SCALE*x = 12 also move from center with -x/6
                    mLeftDriftView.resizeGL(DRIFT_ARROW_SCALE_X*this.y/15, DRIFT_ARROW_SCALE_Y);
                    mLeftDriftView.moveGL(DRIFT_ARROW_CENTER-this.y/90, 0);
                    mRightDriftView.resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
                    mRightDriftView.moveGL(-DRIFT_ARROW_CENTER, 0);
                } else{
                    mLeftDriftView.resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
                    mLeftDriftView.moveGL(DRIFT_ARROW_CENTER, 0);
                    mRightDriftView.resizeGL(DRIFT_ARROW_SCALE_X*(-this.y/15), DRIFT_ARROW_SCALE_Y);
                    mRightDriftView.moveGL(-DRIFT_ARROW_CENTER-this.y/90, 0);
                }




                mInclineBoatView.requestRender();
                mCompassView.requestRender();
                setTiltText(this.x);
            }
            else if(dataType.equals(DATA_TYPE_TEMPERATURE)){
                data = data.replace(',', '.');
                setTempText(Float.parseFloat(data));
            }
            else if(dataType.equals(DATA_TYPE_PRESSURE)){
                data = data.replace(',', '.');
                float pressure = Float.parseFloat(data);
                setPressureText(pressure);
                //mNeedleView.setPressure(pressure/1000 - 1.01325f);
                /* Todo
                    Range of move needle Min moveGL -1.5f, max moveGl 1.2f
                    No calibration made
                */
                mPressureNeedleView.moveGL(0, pressure/1000 - 1.01325f);

                mPressureNeedleView.requestRender();
            }
            else if(dataType.equals(DATA_TYPE_FREE_FALL)){

            }
            else if(dataType.equals(DATA_TYPE_HUMIDITY)){
                data = data.replace(',', '.');
                setHumText(Float.parseFloat(data));
            }
            else if(dataType.equals("Heart")){
                this.y = Byte.parseByte(data);
                setTiltText((int) this.y);
            }
            else if (dataType.equals(DATA_TYPE_POSITION)) {
                data = data.replace(',', '.');
                String[] pos = data.split(":");
                float latitude = Float.parseFloat(pos[0]);
                float longitude = Float.parseFloat(pos[1]);
                float elevation = Float.parseFloat(pos[2]);
                if (latitude != 0f && longitude != 0f) {
                    travelRoute.add(new LatLng(latitude, longitude));
                }

            }
            else if (dataType.equals(DATA_TYPE_COMPASS)){
                //String[] accelerometer = data.split(":");

                this.z = Float.parseFloat(data);

                // Rotates Boat bearing with Yaw
                mCompassView.rotateGl2(this.z);

                mCompassView.requestRender();
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


    public void writeCrapToLog(){
        String time = "";
        String x = "";
        String speed = "";
        String pressure = "";
        String longitude = "";
        String lattitude = "";
        String compass = "";
        Random xRand = new Random();
        Random pressureRand = new Random();
        Random longRand = new Random();
        Random lattRand = new Random();
        Random speedRand = new Random();
        Random compRand = new Random();

        float minLong = 65.4f;
        float maxLong = 65.5f;
        float minLatt = 22.5f;
        float maxLatt = 22.6f;
        float minXSpeed = 0.0f;
        float maxXSpeed = 30.0f;
        for (int i=0;i<100;i++){
            x = String.valueOf((xRand.nextInt(180)) - 90);
            speed = String.valueOf(speedRand.nextFloat()* (maxXSpeed - minXSpeed) + minXSpeed);
            pressure = String.valueOf((pressureRand.nextInt(500)) +750);
            longitude = String.valueOf(longRand.nextFloat() * (maxLong - minLong) +minLong);
            lattitude = String.valueOf(lattRand.nextFloat() * (maxLatt - minLatt) +minLatt);
            compass = String.valueOf(compRand.nextInt(360));
            time = new SimpleDateFormat("HHmmss").format(new Date());
            log.writeToLog(DATA_TYPE_COMPASS +":" +time +":" +compass);
            log.writeToLog(DATA_TYPE_INCLINE +":" +time  +":" +x);
            log.writeToLog(DATA_TYPE_SOG +":" +time  +":" +speed);
            log.writeToLog(DATA_TYPE_PRESSURE +":" +time  +":" + pressure);
            log.writeToLog(DATA_TYPE_POSITION +":" +time  +":" + longitude +":" +lattitude);
        }
    }
}
