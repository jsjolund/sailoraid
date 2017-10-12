package ltuproject.sailoraid;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.ClipData;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
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
    private final static String TAG = MainActivity.class.getSimpleName();

    private static final float BOAT_SCALE_X = 2.8f;
    private static final float BOAT_SCALE_Y = 2.8f;
    private static final float NEEDLE_SCALE_X = 2.8f;
    private static final float NEEDLE_SCALE_Y = 1.0f;
    private static final float COMPASS_SCALE_X = 3.2f;
    private static final float COMPASS_SCALE_Y = 3.2f;
    private static final float COMPASS_BOAT_SCALE_X = 0.5f;
    private static final float COMPASS_BOAT_SCALE_Y = 0.6f;


    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

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
    private BTLEConnection mBluetoothLeService;
    private Menu mMenu;
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
        log = null;
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

    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BTLEConnection.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            if (mBluetoothLeService.getConnectionStatus() == STATE_CONNECTED){
                mBluetoothLeService.discoverServices();
            } else{
                Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
                startService(gattServiceIntent);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    @Override
    protected void onResume() {
        Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
        initFilter();
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mGattUpdateReceiver);
        removeDynamicPics();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    /*
    Init filter for bluetooth update handling
     */
    private void initFilter(){
        filter = new IntentFilter(BTLEConnection.ACTION_GATT_CONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_DISCONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_SERVICES_DISCOVERED);
        filter.addAction(BTLEConnection.ACTION_DATA_AVAILABLE);
        filter.addAction(BTLEConnection.ACTION_GATT_SERVICE_NOTIFIED);
        this.registerReceiver(mGattUpdateReceiver, filter);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.feedback_menu, menu);
        mMenu = menu;
        if (mBluetoothLeService != null){
            setConnectionIcons(mBluetoothLeService.getConnectionStatus());
        }
        return super.onCreateOptionsMenu(mMenu);
    }

    private void setConnectionIcons(int state){
        MenuItem item = mMenu.findItem(R.id.boat_connection);
        MenuItem itemLog = mMenu.findItem(R.id.start_log);
        if (state == STATE_CONNECTED){
            item.setIcon(getDrawable(R.drawable.pico_connected));
            itemLog.setVisible(true);
        } else{
            item.setIcon(getDrawable(R.drawable.pico_disconnected));
            itemLog.setVisible(false);
        }
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.start_log:
                if (!logging){
                    Toast.makeText(getApplicationContext(), "Logging started!", Toast.LENGTH_SHORT).show();
                    log = new SailLog(this);
                    log.initLogData();
                    //writeCrapToLog();
                    //log.writeToLog("Yoyo");
                    logging = true;

                    item.setIcon(getDrawable(R.drawable.loggo));
                    MenuItem newItem = mMenu.findItem(R.id.stop_log);
                    newItem.setVisible(true);
                }
                // User chose the "Settings" item, show the app settings UI...
                return true;

            case R.id.stop_log:
                if (logging){
                    //log.stopLogData();
                    //log.readLog();
                    MenuItem newItem = mMenu.findItem(R.id.start_log);
                    newItem.setIcon(getDrawable(R.drawable.log));
                    Toast.makeText(getApplicationContext(), "Logging finished!", Toast.LENGTH_SHORT).show();
                    item.setVisible(false);
                    log.finalizeLog();
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
                mBluetoothLeService.setmGattService(gattService);
            }
            if (uuid.equals(SampleGattAttributes.ACCELEROMETER_SERVICE.toString())) {
                mBluetoothLeService.setmGattService(gattService);
            }
            if (uuid.equals(SampleGattAttributes.ENV_SERVICE.toString())) {
                mBluetoothLeService.setmGattService(gattService);
            }
            if (uuid.equals(SampleGattAttributes.NUCLEO_GPS_SERVICE.toString())) {
                mBluetoothLeService.setmGattService(gattService);
            }
        }
    }

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
            if (mBluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                tv.setText(R.string.connected);
                invalidateOptionsMenu();
                //MenuItem item = (MenuItem) findViewById(R.id.boat_connection);
                //item.setIcon(getDrawable(R.drawable.pico_connected));
            } else if (mBluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                tv.setText(R.string.disconnected);
                invalidateOptionsMenu();
                //MenuItem item = (MenuItem) findViewById(R.id.boat_connection);
                //item.setIcon(getDrawable(R.drawable.pico_disconnected));
            } else if (mBluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the
                // user interface.
                getKnownGattServices(mBluetoothLeService.getSupportedGattServices());
                //displayGattServices(myBTHandler.getBtLEConnection().getSupportedGattServices());
                //myBTHandler.registerGattNotifications();startRegNotifications();
                startRegNotifications();
            } else if (mBluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                displayData(intent.getStringExtra(mBluetoothLeService.EXTRA_DATA),intent.getStringExtra(mBluetoothLeService.EXTRA_TYPE));
            } else if (mBluetoothLeService.ACTION_GATT_SERVICE_NOTIFIED.equals(action)){
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
                mBluetoothLeService.registerGattNotifications();
            }
        });
    }

    private void displayData(String data, String dataType) {
        if (data != null) {
            String time = new SimpleDateFormat("HHmmssSSS").format(new Date());
            if(dataType.equals(DATA_TYPE_INCLINE)){
                // Get Roll, pitch and yaw
                String[] accelerometer = data.split(":");

                this.x = Float.parseFloat(accelerometer[0]);
                this.y = Float.parseFloat(accelerometer[1]);
                this.z = Float.parseFloat(accelerometer[2]);


                mInclineBoatView.rotateGl((int) this.x);
                // Rotates compass with pitch
                mCompassView.rotateGl((int) this.z);
                if(log != null){
                    if (log.isLogging()){
                        log.writeToLog(DATA_TYPE_INCLINE +":" +time  +":" +x);
                        log.writeToLog(DATA_TYPE_COMPASS +":" +time +":" +z);
                    }

                }

                // Rotates Boat bearing with Yaw
                //mCompassView.rotateGl2(this.z);
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
                setTiltText(this.x);
            }
            else if(dataType.equals(DATA_TYPE_TEMPERATURE)){
                data = data.replace(',', '.');
                setTempText(Float.parseFloat(data));
                if (log != null){
                    if (log.isLogging()){
                        log.writeToLog(DATA_TYPE_TEMPERATURE +":" +time +":" +data);
                    }
                }
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
                if (log != null){
                    if (log.isLogging()){
                        log.writeToLog(DATA_TYPE_PRESSURE +":" +time  +":" + pressure);
                    }
                }

              //  mPressureNeedleView.requestRender();
            }
            else if(dataType.equals(DATA_TYPE_FREE_FALL)){

            }
            else if(dataType.equals(DATA_TYPE_HUMIDITY)){
                data = data.replace(',', '.');
                setHumText(Float.parseFloat(data));
                if(log != null) {
                    if (log.isLogging()){
                        log.writeToLog(DATA_TYPE_HUMIDITY + ":" + time + ":" + data);
                    }
                }
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
                    if(log != null) {
                        if (log.isLogging()){
                            log.writeToLog(DATA_TYPE_POSITION + ":" + time + ":" + longitude + ":" + latitude);
                        }
                    }
                }

            }
            else if (dataType.equals(DATA_TYPE_COMPASS)){
                //String[] accelerometer = data.split(":");
                data = data.replace(',', '.');
                this.z = Float.parseFloat(data);

                // Rotates Boat bearing with Yaw
                mCompassView.rotateGl2(this.z);
                if(log != null) {
                    if (log.isLogging()){
                        log.writeToLog(DATA_TYPE_COMPASS + ":" + time + ":" + z);
                    }
                }
               // mCompassView.requestRender();
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
        String latitude = "";
        String compass = "";
        Random xRand = new Random();
        Random pressureRand = new Random();
        Random longRand = new Random();
        Random lattRand = new Random();
        Random speedRand = new Random();
        Random compRand = new Random();

        float minLong = -68.4f;
        float maxLong = 7.3f;
        float minLatt = -176.29f;
        float maxLatt = -83.5f;
        float minXSpeed = 0.0f;
        float maxXSpeed = 30.0f;
        int t = 0;
        for (int i=0;i<3600;i++){
            x = String.valueOf((xRand.nextInt(180)) - 90);
            speed = String.valueOf(speedRand.nextFloat()* (maxXSpeed - minXSpeed) + minXSpeed);
            pressure = String.valueOf((pressureRand.nextInt(500)) +750);
            longitude = String.valueOf(longRand.nextFloat() * (maxLong - minLong) +minLong);
            latitude = String.valueOf(lattRand.nextFloat() * (maxLatt - minLatt) +minLatt);
            compass = String.valueOf(compRand.nextInt(360));
            time = String.valueOf(t);
            t++;
            log.writeToLog(DATA_TYPE_COMPASS +":" +time +":" +compass);
            log.writeToLog(DATA_TYPE_INCLINE +":" +time  +":" +x);
            log.writeToLog(DATA_TYPE_SOG +":" +time  +":" +speed);
            log.writeToLog(DATA_TYPE_PRESSURE +":" +time  +":" + pressure);
            if (t %100 == 0){
                log.writeToLog(DATA_TYPE_POSITION +":" +time  +":" + longitude +":" +latitude);
            }

        }
    }
}
