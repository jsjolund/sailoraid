package ltuproject.sailoraid;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.PowerManager;
import android.support.constraint.ConstraintLayout;
import android.support.v4.app.Fragment;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapView;
import com.google.android.gms.maps.MapsInitializer;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

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
import ltuproject.sailoraid.graphics.RotatableGLView;

import static java.lang.Math.abs;
import static java.lang.Math.acos;
import static java.lang.Math.asin;
import static java.lang.Math.atan2;
import static java.lang.Math.cos;
import static java.lang.Math.sin;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_COMPASS;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_DRIFT;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_FREE_FALL;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_HUMIDITY;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_INCLINE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_POSITION;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_PRESSURE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_RANGE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_SOG;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_TEMPERATURE;

/**
 * Created by Henrik on 2017-09-05.
 */

public class FeedbackActivity extends AppCompatActivity{
    private final static String TAG = FeedbackActivity.class.getSimpleName();

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
    private SailLog mLogService;
    private boolean logging = false;
    private BTLEConnection mBluetoothLeService;
    private Menu mMenu;
    private MapView mMapView;
    private GoogleMap mGoogleMap;
    private Bundle mSavedInstaceStare;
    private String prevTime;
    private int lastView = 0;
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
        mSavedInstaceStare = savedInstanceState;
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.feedback_activity);

        btnMap = (Button) findViewById(R.id.mapviewbtn);
        mLogService = null;
        initDynamicPics();
        displayInclineFocus();
        showMap();
        Toolbar myToolbar = (Toolbar) findViewById(R.id.feedback_toolbar);
        setSupportActionBar(myToolbar);
        /**
         * Change to map activity and sending location coordinates to intent
         */
        final ViewFlipper viewFlipper = (ViewFlipper) findViewById(R.id.myFeedbackViewFlipper);
        viewFlipper.setOnTouchListener(new OnSwipeTouchListener(FeedbackActivity.this) {

            public void onSwipeRight() {
                viewFlipper.showPrevious();
                int id = viewFlipper.getDisplayedChild();
                if (lastView == 0){
                    hideInclineFocus();
                } else if (lastView == 1){
                    hideMapFocus();
                }
                lastView = id;
                if (id == 0){
                    displayInclineFocus();
                }else if (id == 1){
                    displayMapFocus();
                }
            }
            public void onSwipeLeft() {
                viewFlipper.showNext();
            }

        });

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
                getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            } else{
                Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
                startService(gattServiceIntent);
                if (mBluetoothLeService.getConnectionStatus() != STATE_CONNECTED) {
                    Toast.makeText(getApplicationContext(), "Go back and connect!", Toast.LENGTH_SHORT).show();
                }
            }
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    // Code to manage Service lifecycle.
    private final ServiceConnection mLogServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mLogService = ((SailLog.LocalBinder) service).getService();
        }
        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mLogService = null;
        }
    };

    @Override
    protected void onResume() {
        Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
        if (mLogService == null){
            Intent logServiceIntent = new Intent(getApplicationContext(), SailLog.class);
            bindService(logServiceIntent, mLogServiceConnection, BIND_AUTO_CREATE);
        }
        initFilter();
        if (mMapView != null) {
            mMapView.onResume();
        }
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mGattUpdateReceiver);
        if (mLogServiceConnection!= null){
            unbindService(mLogServiceConnection);
        }
        if (mMapView != null) {
            try {
                mMapView.onDestroy();
            } catch (NullPointerException e) {
                Log.e(TAG, "Error while attempting MapView.onDestroy(), ignoring exception", e);
            }
        }
        removeDynamicPics();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        if (mMapView != null) {
            mMapView.onLowMemory();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mMapView != null) {
            mMapView.onPause();
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (mMapView != null) {
            mMapView.onSaveInstanceState(outState);
        }
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

    /*
    Changes icons in the actionbar depending on connection status
     */
    private void setConnectionIcons(int state){
        MenuItem item = mMenu.findItem(R.id.boat_connection);
        MenuItem itemLog = mMenu.findItem(R.id.start_log);
        MenuItem itemStopLog = mMenu.findItem(R.id.stop_log);
        if (state == STATE_CONNECTED){
            item.setIcon(getDrawable(R.drawable.pico_connected));
            itemLog.setVisible(true);
            if (mLogService != null){
                if(mLogService.isLogging()){
                    itemStopLog.setVisible(true);
                    itemLog.setIcon(getDrawable(R.drawable.loggo));
                }
            }else{
                itemStopLog.setVisible(false);
            }
        } else{
            item.setIcon(getDrawable(R.drawable.pico_disconnected));
            itemLog.setVisible(false);
        }
    }

    /*
    Handles choices made on the actionbar menu
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.start_log:
                if (mLogService != null){
                    logging = mLogService.isLogging();
                }
                if (!logging && (mBluetoothLeService.getConnectionStatus() == STATE_CONNECTED)){
                    logging = true;
                    Toast.makeText(getApplicationContext(), "Logging started!", Toast.LENGTH_SHORT).show();
                    Intent logServiceIntent = new Intent(getApplicationContext(), SailLog.class);
                    if(mLogService != null){
                        startService(logServiceIntent);
                    }
                    item.setIcon(getDrawable(R.drawable.loggo));
                    MenuItem newItem = mMenu.findItem(R.id.stop_log);
                    newItem.setVisible(true);
                } else {
                    Toast.makeText(getApplicationContext(), "Connect to the boat first!", Toast.LENGTH_SHORT).show();
                }
                return true;

            case R.id.stop_log:
                if (mLogService != null){
                    logging = mLogService.isLogging();
                }
                if (logging){
                    MenuItem newItem = mMenu.findItem(R.id.start_log);
                    newItem.setIcon(getDrawable(R.drawable.log));
                    if (mLogService != null){
                        mLogService.stopLogData();
                        mLogService.finalizeLog();
                    }
                    Toast.makeText(getApplicationContext(), "Logging finished!", Toast.LENGTH_SHORT).show();
                    item.setVisible(false);
                    logging = false;
                }
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void displayBoatOnView(int id){
        LinearLayout linearLayout = (LinearLayout) findViewById(id);
        linearLayout.addView(mInclineBoatView.getGlView());
    }
    private void hideBoatFromView(int id){
        LinearLayout linearLayout = (LinearLayout) findViewById(id);
        linearLayout.removeView(mInclineBoatView.getGlView());
    }
    private void displayCompassOnView(int id){
        LinearLayout linearCompassLayout = (LinearLayout) findViewById(id);
        linearCompassLayout.addView(mCompassView.getGlView());
        mCompassView.moveGL2(0.02f,0);
    }
    private void hideCompassFromView(int id){
        LinearLayout linearLayout = (LinearLayout) findViewById(id);
        linearLayout.removeView(mCompassView.getGlView());
    }
    private void displayNeedleOnView(int id){
        LinearLayout linearPressureLayout = (LinearLayout) findViewById(id);
        linearPressureLayout.addView(mPressureNeedleView.getGlView());
    }
    private void hideNeedleFromView(int id){
        LinearLayout linearPressureLayout = (LinearLayout) findViewById(id);
        linearPressureLayout.removeView(mPressureNeedleView.getGlView());
    }
    private void displayDriftArrowOnView(int leftId, int rightId, float centerLeft, float centerRight){
        LinearLayout leftDriftLayout = (LinearLayout) findViewById(leftId);
        leftDriftLayout.addView(mLeftDriftView.getGlView());
        LinearLayout rightDriftLayout = (LinearLayout) findViewById(rightId);
        rightDriftLayout.addView(mRightDriftView.getGlView());
        mLeftDriftView.moveGL(centerLeft,0);
        mRightDriftView.moveGL(-centerRight,0);
    }
    private void hideDriftArrowFromView(int leftId, int rightId){
        LinearLayout leftDriftLayout = (LinearLayout) findViewById(leftId);
        leftDriftLayout.removeView(mLeftDriftView.getGlView());
        LinearLayout rightDriftLayout = (LinearLayout) findViewById(rightId);
        rightDriftLayout.removeView(mRightDriftView.getGlView());
    }
    private void displayMapFocus(){
        displayBoatOnView(R.id.inclineMapFocusViewHolder);
        displayCompassOnView(R.id.compassMapFocusViewHolder);
        displayDriftArrowOnView(R.id.leftDriftMapFocus, R.id.rightDriftMapFocus, DRIFT_ARROW_CENTER, DRIFT_ARROW_CENTER);
    }
    private void hideMapFocus(){
        hideBoatFromView(R.id.inclineMapFocusViewHolder);
        hideCompassFromView(R.id.compassMapFocusViewHolder);
        hideDriftArrowFromView(R.id.leftDriftMapFocus, R.id.rightDriftMapFocus);
    }
    private void displayInclineFocus(){
        displayBoatOnView(R.id.boatalignmentholder);
        displayCompassOnView(R.id.driftImg);
        displayDriftArrowOnView(R.id.leftDrift, R.id.rightDrift, DRIFT_ARROW_CENTER, DRIFT_ARROW_CENTER);
        displayNeedleOnView(R.id.pressureMeter);
        mPressureNeedleView.moveGL(0, NEEDLE_BOTTOM_POS);
    }
    private void hideInclineFocus(){
        hideBoatFromView(R.id.boatalignmentholder);
        hideCompassFromView(R.id.driftImg);
        hideNeedleFromView(R.id.pressureMeter);
        hideDriftArrowFromView(R.id.leftDrift, R.id.rightDrift);
    }
    private void initDynamicPics(){
        mInclineBoatView = new RotatableGLView(this, BitmapFactory.decodeResource(getResources(),
                R.drawable.boat_alignement),
                BOAT_SCALE_X, BOAT_SCALE_Y);
        mPressureNeedleView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.needle),
                NEEDLE_SCALE_X, NEEDLE_SCALE_Y);
        mCompassView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.compass), BitmapFactory.decodeResource(getResources(), R.drawable.rowboat),
                COMPASS_SCALE_Y, COMPASS_SCALE_X, COMPASS_BOAT_SCALE_X, COMPASS_BOAT_SCALE_Y);
        mLeftDriftView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.left_drift_arrow),
                DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        mRightDriftView = new RotatableGLView(this,
                BitmapFactory.decodeResource(getResources(), R.drawable.right_drift_arrow),
                DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
    }
    /*
    Places the moving pictures onto wanted views.
     */
    private void displayDynamicPics(){

        // Range of move needle Min moveGL -1.5f, max moveGl 1.2f
        //mPressureNeedleView.moveGL(0,NEEDLE_BOTTOM_POS);

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

    /*
    Given a found service from a connected device checks all services that are known by SampleGattAttributes List, adds known to the bluetooth service for registering
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
            if (uuid.equals(SampleGattAttributes.HEART_RATE_SERVICE.toString())
                || uuid.equals(SampleGattAttributes.ACCELEROMETER_SERVICE.toString())
                || uuid.equals(SampleGattAttributes.ENV_SERVICE.toString())
                || uuid.equals(SampleGattAttributes.NUCLEO_GPS_SERVICE.toString())
                || uuid.equals(SampleGattAttributes.NUCLEO_RANGE_SERVICE.toString())){
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
                } else if (mBluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                tv.setText(R.string.disconnected);
                invalidateOptionsMenu();
               } else if (mBluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                  getKnownGattServices(mBluetoothLeService.getSupportedGattServices());
                   startRegNotifications();
            } else if (mBluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                displayData(intent.getStringExtra(mBluetoothLeService.EXTRA_DATA),intent.getStringExtra(mBluetoothLeService.EXTRA_TYPE));
            } else if (mBluetoothLeService.ACTION_GATT_SERVICE_NOTIFIED.equals(action)){
                 startRegNotifications();
            }
        }
    };

    /*
    Creates a handler for a thread that registers found Gatt characteristics from connected device.
     */
    public void startRegNotifications() {
        Handler handler = new Handler();
        handler.post(new Runnable() {
            @Override
            public void run() {
                mBluetoothLeService.registerGattNotifications();
            }
        });
    }

    /*
    Display data received from bluetooth device.
     */
    public void displayData(String data, String dataType) {
        if (data != null) {
            String time = new SimpleDateFormat("HHmmssSSS").format(new Date());
            if(dataType.equals(DATA_TYPE_INCLINE)){
                // Get Roll, pitch and yaw
                String[] accelerometer = data.split(":");

                this.x = Float.parseFloat(accelerometer[0]);
                this.y = Float.parseFloat(accelerometer[1]);
                this.z = Float.parseFloat(accelerometer[2]);

                mInclineBoatView.rotateGl((int) this.x);
                //float positionBoat = this.x / 100;
                //mInclineBoatView.moveGL(positionBoat, positionBoat);
                // Rotates compass with pitch
                mCompassView.rotateGl((int) this.z);
                if(mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_INCLINE +":" +time  +":" +x);
                        mLogService.writeToLog(DATA_TYPE_COMPASS +":" +time +":" +z);
                    }
                }
                setTiltText(this.x);
            }
            else if(dataType.equals(DATA_TYPE_TEMPERATURE)){
                data = data.replace(',', '.');
                setTempText(Float.parseFloat(data));
                if (mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_TEMPERATURE +":" +time +":" +data);
                    }
                }
            }
            else if(dataType.equals(DATA_TYPE_PRESSURE)){
                data = data.replace(',', '.');
                float pressure = Float.parseFloat(data);
                setPressureText(pressure);
                //mNeedleView.setPressure(pressure/1000 - 1.01325f);
                /* Todo Calibrate
                    Range of move needle Min moveGL -1.5f, max moveGl 1.2f
                    No calibration made
                */
                mPressureNeedleView.moveGL(0, pressure/1000 - 1.01325f);
                if (mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_PRESSURE +":" +time  +":" + pressure);
                    }
                }

              //  mPressureNeedleView.requestRender();
            }
            else if(dataType.equals(DATA_TYPE_FREE_FALL)){

            }
            else if(dataType.equals(DATA_TYPE_HUMIDITY)){
                data = data.replace(',', '.');
                setHumText(Float.parseFloat(data));
                if(mLogService != null) {
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_HUMIDITY + ":" + time + ":" + data);
                    }
                }
            }
            else if (dataType.equals(DATA_TYPE_POSITION)) {
                data = data.replace(',', '.');
                String[] pos = data.split(":");
                String newTime = new SimpleDateFormat("ssSSS").format(new Date());
                float latitude = Float.parseFloat(pos[0]);
                float longitude = Float.parseFloat(pos[1]);
                float elevation = Float.parseFloat(pos[2]);
                float speed = Float.parseFloat(pos[3]);
                float direction = Float.parseFloat(pos[4]);
                if (latitude != 0f && longitude != 0f) {
                    LatLng gpsPos = new LatLng(latitude, longitude);
                    //double speed_mps = 0;
                    double drift = 0;
                    if (travelRoute.size()>1){
                        //Calculate distance and speed from last point, possibly could filter moving avg
                        double dist = distance_on_geoid(latitude, longitude, gpsPos.latitude, gpsPos.longitude);
                        //double time_s = (Double.valueOf(prevTime) - Double.valueOf(newTime)) / 1000.0;
                        //speed_mps = dist/time_s;
                        // Estimates next position using brearing, now just z axis from IMU
                        // TODO this.z should be compass bearing, gps direction bearing atm
                        LatLng nextEstimate = calcNextEstimatePos(gpsPos, dist, direction);
                        // Calculate perpendicular drift from the ship navigational bearing
                        drift = (nextEstimate.longitude-longitude) * Math.cos((latitude+nextEstimate.latitude)/2);
                        TextView tv = (TextView) findViewById(R.id.driftText);
                        tv.setText(String.valueOf(drift +"m"));
                        // Use pitch for testing drifting feedback
                        displayDrift((float) drift);
                    }
                    travelRoute.add(gpsPos);
                    TextView tv = (TextView) findViewById(R.id.speedText);
                    tv.setText(String.valueOf(speed));
                    if(mLogService != null) {
                        if (mLogService.isLogging()){
                            mLogService.writeToLog(DATA_TYPE_POSITION + ":" + time + ":" + longitude + ":" + latitude);
                            mLogService.writeToLog(DATA_TYPE_SOG + ":" +time +":" +speed);
                            mLogService.writeToLog(DATA_TYPE_DRIFT + ":" +time +":" +drift);
                        }
                    }
                }
                prevTime = new SimpleDateFormat("ssSSS").format(new Date());
            }
            else if (dataType.equals(DATA_TYPE_COMPASS)){
                //String[] accelerometer = data.split(":");
                data = data.replace(',', '.');
                this.z = Float.parseFloat(data);
                // Rotates Boat bearing with Yaw
                mCompassView.rotateGl2(this.z);
                if(mLogService != null) {
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_COMPASS + ":" + time + ":" + z);
                    }
                }
            } else if (dataType.equals(DATA_TYPE_RANGE)){
                data = data.replace(',', '.');
                Float range = Float.parseFloat(data);

                TextView tv = (TextView) findViewById(R.id.rangeText);
                tv.setText(String.valueOf(range));
                if(mLogService != null) {
                    if (mLogService.isLogging()){
                        //mLogService.writeToLog(DATA_TYPE_COMPASS + ":" + time + ":" + z);
                    }
                }
            }

        }
    }

    /*
    Shows a map inside a view
     */
    private void showMap(){
        MapView mMapView = (MapView) findViewById(R.id.mapFragment);
        MapsInitializer.initialize(getApplicationContext());

        mMapView.onCreate(mSavedInstaceStare);
        mMapView.onResume();// needed to get the map to display immediately
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(final GoogleMap googleMap) {
                LatLng pos = new LatLng(0,0);
                if (!travelRoute.isEmpty()){
                    pos = travelRoute.get(travelRoute.size()); ////your lat lng
                }
                googleMap.addMarker(new MarkerOptions().position(pos).title("Yout title"));
                googleMap.moveCamera(CameraUpdateFactory.newLatLng(pos));
                googleMap.getUiSettings().setZoomControlsEnabled(true);
                googleMap.animateCamera(CameraUpdateFactory.zoomTo(10), 2000, null);
            }
        });
    }

    private void displayDrift(float drift){
        // Use pitch for testing drifting feedback
        if(drift >=0){
            // Max resize ARROW_SCALE*x = 12 also move from center with -x/6
            mLeftDriftView.resizeGL(DRIFT_ARROW_SCALE_X*drift/15, DRIFT_ARROW_SCALE_Y);
            mLeftDriftView.moveGL(DRIFT_ARROW_CENTER-drift/90, 0);
            mRightDriftView.resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
            mRightDriftView.moveGL(-DRIFT_ARROW_CENTER, 0);
        } else{
            mLeftDriftView.resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
            mLeftDriftView.moveGL(DRIFT_ARROW_CENTER, 0);
            mRightDriftView.resizeGL(DRIFT_ARROW_SCALE_X*(-drift/15), DRIFT_ARROW_SCALE_Y);
            mRightDriftView.moveGL(-DRIFT_ARROW_CENTER-drift/90, 0);
        }
    }

    /*
    Calculate distance in meters based on two points in longitude latitude
     */
    private double distance_on_geoid(double lat1, double lon1, double lat2, double lon2) {

        // Convert degrees to radians
        lat1 = lat1 * Math.PI / 180.0;
        lon1 = lon1 * Math.PI / 180.0;

        lat2 = lat2 * Math.PI / 180.0;
        lon2 = lon2 * Math.PI / 180.0;

        // radius of earth in metres
        double r = 6378100;

        // P
        double rho1 = r * cos(lat1);
        double z1 = r * sin(lat1);
        double x1 = rho1 * cos(lon1);
        double y1 = rho1 * sin(lon1);

        // Q
        double rho2 = r * cos(lat2);
        double z2 = r * sin(lat2);
        double x2 = rho2 * cos(lon2);
        double y2 = rho2 * sin(lon2);

        // Dot product
        double dot = (x1 * x2 + y1 * y2 + z1 * z2);
        double cos_theta = dot / (r * r);

        double theta = acos(cos_theta);

        // Distance in Metres
        return r * theta;
    }

    /*
    Estimate where next position should be using bearing and distance traveled between last two points.
     */
    private LatLng calcNextEstimatePos(LatLng pos, double distance, float radialBearing){
        int R = 6371; // Earth Radius in Km

        double lat2 = Math.asin(Math.sin(Math.PI / 180 * pos.latitude) * Math.cos(distance / R) + Math.cos(Math.PI / 180 * pos.latitude) * Math.sin(distance / R) * Math.cos(Math.PI / 180 * radialBearing));
        double lon2 = Math.PI / 180 * pos.longitude + Math.atan2(Math.sin( Math.PI / 180 * radialBearing) * Math.sin(distance / R) * Math.cos( Math.PI / 180 * pos.longitude ), Math.cos(distance / R) - Math.sin( Math.PI / 180 * pos.longitude) * Math.sin(lat2));

        return new LatLng(180 / Math.PI * lat2 , 180 / Math.PI * lon2);
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
}
