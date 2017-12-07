package ltuproject.sailoraid;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.MaskFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Vibrator;
import android.speech.tts.TextToSpeech;
import android.speech.tts.UtteranceProgressListener;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.io.Serializable;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import ltuproject.sailoraid.bluetooth.BTLEConnection;
import ltuproject.sailoraid.bluetooth.SampleGattAttributes;
import ltuproject.sailoraid.datalog.SailLog;
import ltuproject.sailoraid.feedback.IntervalVibrator;
import ltuproject.sailoraid.feedback.StateChecker;
import ltuproject.sailoraid.graphics.RotatableGLView;
import ltuproject.sailoraid.graphics.ViewDisplayer;
import ltuproject.sailoraid.location.Locator;
import ltuproject.sailoraid.location.MapRunner;

import static java.lang.Math.abs;
import static java.lang.Math.acos;
import static java.lang.Math.cos;
import static java.lang.Math.sin;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_BATTERY;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_COMPASS;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_DRIFT;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_ESTPOSITION;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_FREE_FALL;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_HUMIDITY;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_INCLINE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_POSITION;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_PRESSURE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_RANGE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_SOG;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_TEMPERATURE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_WAVES;
import static ltuproject.sailoraid.graphics.BitmapScaler.decodeSampledBitmapFromResource;

/**
 * Created by Henrik on 2017-09-05.
 */

public class FeedbackActivity extends AppCompatActivity implements OnMapReadyCallback{
    private final static String TAG = FeedbackActivity.class.getSimpleName();
    static final int WP_REQUEST = 1;
    /*
      Positioning and scale of rotatable images
     */
    private static final float DRIFT_ARROW_SCALE_X = 1.0f;
    private static final float DRIFT_ARROW_SCALE_Y = 3.0f;
    private static final float NEEDLE_X_POS = -0.3f;
    private static final float DRIFT_ARROW_CENTER = 4.2f;
    private static final float MAP_DRIFT_ARROW_CENTER = 5.2f;
    private static final float WAVE_X_POS = 2.0f;
    private static final float WAVE_Y_POS = -0.5f;
    private static final float WAVE_BOAT_X_POS = -1.8f;
    private static final float WAVE_BOAT_Y_POS = 0.2f;
    private static final float GRAPHIC_DAGGER_Y_POS = -3.3f;
    private static final float GRAPHIC_DAGGER_X_POS = -0.3f;
    private static final float GRAPHIC_PRESSURE_X_POS = 0.0f;
    private static final float GRAPHIC_PRESSURE_Y_POS = -1.5f;
    private static final float GRAPHIC_SPEED_X_POS = -7.0f;
    private static final float GRAPHIC_SPEED_Y_POS = 0.4f;
    private static final float GRAPHIC_DRIFT_ARROW_CENTER = 5.8f;

    public static final float KM_TO_KNOTS = 1/1.852f;

    /*
      Bluetooth states
     */
    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;
    private static final int UNINITIALIZED = 9999;

    private static int RANGEMAXLIMIT = 80;

    private Handler mWaveHandler;

    private IntentFilter filter;
    private TextView feedbackText;
    private SailLog mLogService;
    private boolean logging = false;
    private BTLEConnection mBluetoothLeService;
    private Menu mMenu;
    private Bundle mSavedInstaceStare;
    private int lastView = 0;

    private boolean isWaving = false;
    private float wavePos = 0;
    /*
    Sensor variables
     */
    private float direction;
    private float batteryPower;
    private LatLng nextEstimate = new LatLng(0,0);
    private LatLng gpsPos = new LatLng(0,0);
    private Button btnMap;

    private GoogleMap mMap;

    //Location variables

    private ArrayList<LatLng> wpRoute = new ArrayList<LatLng>();
    static private List<LatLng> estimatedRoute = new ArrayList<LatLng>();
    public static synchronized void getEstimatedRoute(List<LatLng> output) {
        output.addAll(estimatedRoute);
    }
    private MapRunner mRunner;
    static private List<LatLng> travelRoute = new ArrayList<LatLng>();
    public static synchronized void getRoute(List<LatLng> output) {
        output.addAll(travelRoute);
    }

    private StateChecker mFeedbackStateChecker;
    private ViewDisplayer mViewDisplayer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mSavedInstaceStare = savedInstanceState;
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.feedback_activity);
        Toolbar myToolbar = findViewById(R.id.feedback_toolbar);
        setSupportActionBar(myToolbar);

        Bundle extras = getIntent().getBundleExtra("bundle");
        if (extras != null){
            wpRoute = (ArrayList<LatLng>) extras.getSerializable("ARRAYLIST");
        }
        this.feedbackText = new TextView(this); //findViewById(R.id.feedbackText);
        feedbackText.setText("Yo");
        feedbackText.setLayoutParams(new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.WRAP_CONTENT));
        mFeedbackStateChecker = new StateChecker(this, feedbackText);
        mViewDisplayer = new ViewDisplayer(this, feedbackText);
        btnMap = findViewById(R.id.mapviewbtn);
        mLogService = null;
        initFilter();
        mViewDisplayer.displayInclineFocus();
        mWaveHandler = new Handler();


        final ViewFlipper viewFlipper = findViewById(R.id.myFeedbackViewFlipper);
        viewFlipper.setOnTouchListener(new OnSwipeTouchListener(FeedbackActivity.this) {

            public void onSwipeRight() {
                viewFlipper.showPrevious();
                changeView(viewFlipper.getDisplayedChild());
            }
            public void onSwipeLeft() {
                viewFlipper.showNext();
                changeView(viewFlipper.getDisplayedChild());
            }

        });

        btnMap.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                Intent intent = new Intent(FeedbackActivity.this, MapsActivity.class);
                Bundle args = new Bundle();
                args.putSerializable("ARRAYLIST", wpRoute);
                intent.putExtra("bundle", args);
                startActivityForResult(intent, WP_REQUEST);
            }
        });
    }


    private void changeView(int viewId){
        if (lastView == 0){
            mViewDisplayer.hideInclineFocus();
        } else if (lastView == 1){
            mViewDisplayer.hideMapFocus();
            mRunner.stopMap();
            SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                    .findFragmentById(R.id.mapFragment);
            mapFragment.onPause();
        } else if (lastView == 2){
            mViewDisplayer.hideSimpleFocus();
            mWaveHandler.removeCallbacks(mWaveRunner);
        } else if (lastView == 3){
            mViewDisplayer.hideGraphicFocus();
        }
        lastView = viewId;
        if (viewId == 0){
            mViewDisplayer.displayInclineFocus();
        }else if (viewId == 1){
            mViewDisplayer.displayMapFocus();
            SupportMapFragment mapFragment = (SupportMapFragment)  getSupportFragmentManager()
                    .findFragmentById(R.id.mapFragment);
            mapFragment.getMapAsync(this);
        }else if (viewId == 2){
            mViewDisplayer.displaySimpleFocus();
            mWaveRunner.run();
        }else if (viewId == 3){
            mViewDisplayer.displayGraphicFocus();
        }
    }
    /*
    Binds connection service to this activity
     */
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BTLEConnection.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            if (mBluetoothLeService.getConnectionStatus() == STATE_CONNECTED
                    && !mBluetoothLeService.isDiscovered()){
                   mBluetoothLeService.discoverServices();
            } else{
                if (mBluetoothLeService.getConnectionStatus() != STATE_CONNECTED) {
                    Toast.makeText(getApplicationContext(), "Go back and connect!", Toast.LENGTH_SHORT).show();
                }
            }
            startRepeatingTask();
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
            stopRepeatingTask();
        }
    };

    /*
    Binds log service to this activity
     */
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
        super.onResume();
        if (mRunner != null){
            mRunner.updateWpRoute(wpRoute);
        }
        Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
        if (mLogService == null){
            Intent logServiceIntent = new Intent(getApplicationContext(), SailLog.class);
            bindService(logServiceIntent, mLogServiceConnection, BIND_AUTO_CREATE);
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch(requestCode) {
            case (WP_REQUEST) : {
                if (resultCode == Activity.RESULT_OK) {
                    Bundle args = data.getBundleExtra("bundle");
                    if (args != null){
                        wpRoute = (ArrayList<LatLng>) args.getSerializable("ARRAYLIST");
                    }
                }
                break;
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mGattUpdateReceiver);
        stopRepeatingTask();
        mViewDisplayer.clearBitmaps();
        Runtime.getRuntime().gc();
        if (mLogServiceConnection!= null){
            unbindService(mLogServiceConnection);
        }
        if (mServiceConnection != null){
            unbindService(mServiceConnection);
        }
    }


    @Override
    public void onLowMemory() {
        super.onLowMemory();
    }

    @Override
    protected void onPause() {
        stopRepeatingTask();
        super.onPause();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
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
                    if(mLogService != null){
                        Intent logServiceIntent = new Intent(getApplicationContext(), SailLog.class);
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

            case R.id.voice_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                } else{
                    item.setChecked(false);
                }
                mFeedbackStateChecker.setEnableVoice(item.isChecked());
                return true;
            case R.id.vibration_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                } else{
                    item.setChecked(false);
                }
                mFeedbackStateChecker.setEnableVibration(item.isChecked());
                return true;
            default:
                return super.onOptionsItemSelected(item);
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
            if (mBluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                invalidateOptionsMenu();
                mBluetoothLeService.discoverServices();
            } else if (mBluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                invalidateOptionsMenu();
                stopRepeatingTask();
            } else if (mBluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                mBluetoothLeService.getKnownGattServices(mBluetoothLeService.getSupportedGattServices());
                mBluetoothLeService.startRegNotifications();
            } else if (mBluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                displayData(intent.getStringExtra(mBluetoothLeService.EXTRA_DATA),intent.getStringExtra(mBluetoothLeService.EXTRA_TYPE));
            } else if (mBluetoothLeService.ACTION_GATT_SERVICE_NOTIFIED.equals(action)){
                mBluetoothLeService.startRegNotifications();
            }
        }
    };

    /**
    Display data received from bluetooth device.
     */
    public void displayData(String data, String dataType) {
        if (data != null) {
            String time = new SimpleDateFormat("HHmmssSSS").format(new Date());
            if(dataType.equals(DATA_TYPE_INCLINE)){
                // Get Roll, pitch and yaw
                String[] accelerometer = data.split(":");
                mFeedbackStateChecker.setInclineX(Float.parseFloat(accelerometer[1]));
                mFeedbackStateChecker.setInclineY(Float.parseFloat(accelerometer[0]));
                mFeedbackStateChecker.setBearingZ(Float.parseFloat(accelerometer[2]));
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE){
                    TextView tv = findViewById(R.id.tiltText);
                    tv.setText(String.format("%.1f\u00B0", mFeedbackStateChecker.getInclineX()));
                    tv = findViewById(R.id.tiltYText);
                    tv.setText(String.format("%.1f\u00B0", mFeedbackStateChecker.getInclineY()));
                    tv = findViewById(R.id.zText);
                    tv.setText(String.format("%.2f", mFeedbackStateChecker.getWavePeriod())+"Hz");
                }
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE
                        || mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.MAP
                        || mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.SIMPLE
                        || mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.GRAPHIC){
                    mViewDisplayer.getmInclineBoatView().rotateGl((int) mFeedbackStateChecker.getInclineX());
                    mViewDisplayer.getmInclineBoatView().getGlView().requestRender();
                }
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.SIMPLE){
                    mViewDisplayer.getmWaveView().rotateGl2((int) mFeedbackStateChecker.getInclineY());
                    mViewDisplayer.getmWaveView().getGlView().requestRender();
                    if (!isWaving){
                        mWaveRunner.run();
                    }
                }
                mFeedbackStateChecker.setWavePeriod(mFeedbackStateChecker.getInclineY());
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE
                        || mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.MAP
                        || mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.GRAPHIC){
                    mViewDisplayer.getmCompassView().rotateGl(abs(mFeedbackStateChecker.getBearingZ()));
                    mViewDisplayer.getmCompassView().getGlView().requestRender();
                }
                if(mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_INCLINE +":" +time  +":" +mFeedbackStateChecker.getInclineX());
                    }
                }
            } else if(dataType.equals(DATA_TYPE_TEMPERATURE)){
                data = data.replace(',', '.');
                TextView tv = findViewById(R.id.tempText);
                tv.setText(String.format("%.1f\u00B0C", data));
                if (mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_TEMPERATURE +":" +time +":" +data);
                    }
                }
            } else if(dataType.equals(DATA_TYPE_PRESSURE)){
                data = data.replace(',', '.');
                String[] loadCell = data.split(":");
                // Todo uncomment
                // this.maxPressure = Math.max(Float.parseFloat(loadCell[0]), Float.parseFloat(loadCell[1]));
                //float maxPressure = Float.parseFloat(loadCell[0])-18;
                // mFeedbackStateChecker.setMaxPressure(maxPressure-18);
                mFeedbackStateChecker.setMaxPressure(Float.parseFloat(loadCell[0])-18);
                TextView tv = findViewById(R.id.pressureText);
                tv.setText(String.format("%.1f Psi", mFeedbackStateChecker.getMaxPressure()));
                if(mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE){
                    mViewDisplayer.getmPressureNeedleView().moveGL(NEEDLE_X_POS, mFeedbackStateChecker.getMaxPressure()/40 -1.2f);
                    mViewDisplayer.getmPressureNeedleView().getGlView().requestRender();
                }
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.GRAPHIC) {
                    mViewDisplayer.getmPressureNeedleView().moveGL(GRAPHIC_PRESSURE_X_POS, mFeedbackStateChecker.getMaxPressure()/40 -1.2f);
                    mViewDisplayer.getmPressureNeedleView().getGlView().requestRender();
                }
                if (mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_PRESSURE +":" +time  +":" +mFeedbackStateChecker.getMaxPressure());
                    }
                }
            } else if(dataType.equals(DATA_TYPE_HUMIDITY)){
                data = data.replace(',', '.');
                TextView tv = findViewById(R.id.humText);
                tv.setText(String.format("%.1f%%", data));
                if(mLogService != null) {
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_HUMIDITY + ":" + time + ":" + data);
                    }
                }
            } else if (dataType.equals(DATA_TYPE_POSITION)) {
                data = data.replace(',', '.');
                String[] pos = data.split(":");
                float latitude = Float.parseFloat(pos[0]);
                float longitude = Float.parseFloat(pos[1]);
                float elevation = Float.parseFloat(pos[2]);
                mFeedbackStateChecker.setSpeed(Float.parseFloat(pos[3])*KM_TO_KNOTS);
                this.direction = Float.parseFloat(pos[4]);
               // this.batteryPower = Float.parseFloat(pos[5]);
               // showBatteryLeft(this.batteryPower);
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE){
                    TextView tv = findViewById(R.id.batPer);
                    tv.setText(String.format("%.0f",batteryPower) +"%");
                }
                if (latitude != 0f && longitude != 0f) {
                    LatLng currPos = new LatLng(latitude, longitude);
                    if (this.gpsPos.latitude != 0f && this.gpsPos.longitude != 0f){
                        //Calculate distance and speed from last point, possibly could filter moving avg
                        double dist = Locator.distance_on_geoid(currPos.latitude, currPos.longitude, this.gpsPos.latitude, this.gpsPos.longitude);
                        // Calculate perpendicular drift from the ship navigational bearing will update from previously calculated value to compare with the new values.
                        if (this.nextEstimate.latitude != 0f && this.nextEstimate.longitude != 0f){
                            if(mLogService != null) {
                                if (mLogService.isLogging()) {
                                    mLogService.writeToLog(DATA_TYPE_ESTPOSITION + ":" + time + ":" + this.nextEstimate.latitude + ":" + this.nextEstimate.longitude + ":" + direction);
                                }
                            }
                           // this.drift = Locator.distance_on_geoid(this.nextEstimate.latitude, this.nextEstimate.longitude, latitude, longitude);
                            mFeedbackStateChecker.setDrift(Locator.distance_on_geoid(this.nextEstimate.latitude, this.nextEstimate.longitude, latitude, longitude));
                        }
                        this.nextEstimate = Locator.calcNextEstimatePos(new LatLng(latitude,longitude), dist, direction);
                        if (this.nextEstimate.latitude != 0 && this.nextEstimate.longitude != 0){
                            this.estimatedRoute.add(this.nextEstimate);
                        }
                        if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE){
                            TextView tv = findViewById(R.id.driftText);
                            tv.setText(String.format("%.1f", mFeedbackStateChecker.getDrift()) +"m");
                            tv = findViewById(R.id.speedText);
                            tv.setText(String.format("%.1f", mFeedbackStateChecker.getSpeed()) +"kn");

                        }
                        if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE){
                            displayDrift((float) mFeedbackStateChecker.getDrift(), DRIFT_ARROW_CENTER);
                        }
                        if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.MAP) {
                            displayDrift((float) mFeedbackStateChecker.getDrift(), MAP_DRIFT_ARROW_CENTER);
                        }
                        if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.GRAPHIC) {
                            displayDrift((float) mFeedbackStateChecker.getDrift(), GRAPHIC_DRIFT_ARROW_CENTER);
                            mViewDisplayer.getmSpeedView().moveGL(GRAPHIC_SPEED_X_POS+(mFeedbackStateChecker.getSpeed()/7), GRAPHIC_SPEED_Y_POS);
                            mViewDisplayer.getmSpeedView().getGlView().requestRender();
                        }
                    }
                    this.gpsPos = currPos;
                    this.estimatedRoute.add(this.gpsPos);

                    travelRoute.add(gpsPos);
                    if(mLogService != null) {
                        if (mLogService.isLogging()){
                            mLogService.writeToLog(DATA_TYPE_POSITION + ":" + time + ":" + latitude + ":" + longitude + ":" +direction);
                            mLogService.writeToLog(DATA_TYPE_COMPASS + ":" +time +":" +direction);
                            mLogService.writeToLog(DATA_TYPE_SOG + ":" +time +":" +mFeedbackStateChecker.getSpeed());
                            mLogService.writeToLog(DATA_TYPE_DRIFT + ":" +time +":" +mFeedbackStateChecker.getDrift());
                            mLogService.writeToLog(DATA_TYPE_WAVES +":" +time +":" +mFeedbackStateChecker.getWavePeriod());
                          }
                    }
                }
            } else if (dataType.equals(DATA_TYPE_RANGE)){
                data = data.replace(',', '.');
                mFeedbackStateChecker.setRange(Float.parseFloat(data));
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.INCLINE){
                    TextView tv = findViewById(R.id.rangeText);
                    tv.setText(String.format("%.0f",mFeedbackStateChecker.getRange()) +"cm");
                }
                if (mViewDisplayer.getmCurrentViewState() == ViewDisplayer.ViewStates.GRAPHIC){
                    if (mFeedbackStateChecker.getRange() < 80){
                        mViewDisplayer.getmDaggerView().moveGL(GRAPHIC_DAGGER_X_POS, GRAPHIC_DAGGER_Y_POS +(mFeedbackStateChecker.getRange()/35));
                    } else{
                        mViewDisplayer.getmDaggerView().moveGL(GRAPHIC_DAGGER_X_POS, GRAPHIC_DAGGER_Y_POS +(RANGEMAXLIMIT/35));
                    }
                    mViewDisplayer.getmDaggerView().getGlView().requestRender();
                }
                if(mLogService != null) {
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_RANGE + ":" + time + ":" + mFeedbackStateChecker.getRange());
                    }
                }
            }
        }
    }

    /*
     Moves the wave image with speed depending on calculated waveperiod
     */
    Runnable mWaveRunner = new Runnable() {
        @Override
        public void run() {
            try {
                isWaving = true;
             //   wavePos +=wavePeriod;
                wavePos += mFeedbackStateChecker.getWavePeriod();
                mViewDisplayer.getmWaveView().moveGL(WAVE_X_POS-wavePos,WAVE_Y_POS);
                mViewDisplayer.getmWaveView().moveGL2(WAVE_BOAT_X_POS+0.60f*wavePos,WAVE_BOAT_Y_POS);
                mViewDisplayer.getmWaveView().getGlView().requestRender();


            } finally {
                if (wavePos > 5f && mFeedbackStateChecker.getWavePeriod() > 0){
                    mViewDisplayer.getmWaveView().moveGL(WAVE_X_POS,WAVE_Y_POS);
                    mViewDisplayer.getmWaveView().moveGL2(WAVE_BOAT_X_POS, WAVE_BOAT_Y_POS);
                    mViewDisplayer.getmWaveView().getGlView().requestRender();
                    wavePos = 0;
                    isWaving = false;
                } else{
                    mWaveHandler.postDelayed(mWaveRunner, 200);
                }
            }
        }
    };

    /*
    Change icon in toolbar menu to display current battery level
     */
    private void showBatteryLeft(float bat){
        if (mMenu != null){
            if(bat > 70){
                mMenu.getItem(R.id.battery).setIcon(getDrawable(R.drawable.high_bat));
            } else if(bat<70 && bat > 30){
                mMenu.getItem(R.id.battery).setIcon(getDrawable(R.drawable.mid_bat));
            } else if(bat < 30){
                mMenu.getItem(R.id.battery).setIcon(getDrawable(R.drawable.low_bat));
            }
        }
    }

    private void startRepeatingTask() {
        mFeedbackStateChecker.run();
//        mStateChecker.run();
    }

    private void stopRepeatingTask() {
        mFeedbackStateChecker.stop();
     //   mFeedbackHandler.removeCallbacks(mStateChecker);
    }

    /*
     Move the drift arrow images depending on the calculated drift amount
     */
    private void displayDrift(float drift, float arrowCenter){
        // Use pitch for testing drifting feedback
            // Max resize ARROW_SCALE*x = 12 also move from center with -x/6
        mViewDisplayer.getmLeftDriftView().resizeGL(DRIFT_ARROW_SCALE_X*drift/15, DRIFT_ARROW_SCALE_Y);
        mViewDisplayer.getmLeftDriftView().moveGL(arrowCenter-drift/45, 0);
        mViewDisplayer.getmRightDriftView().resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        mViewDisplayer.getmRightDriftView().moveGL(-arrowCenter, 0);
        mViewDisplayer.getmLeftDriftView().resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        mViewDisplayer.getmLeftDriftView().moveGL(arrowCenter, 0);
        mViewDisplayer.getmRightDriftView().resizeGL(DRIFT_ARROW_SCALE_X*(-drift/15), DRIFT_ARROW_SCALE_Y);
        mViewDisplayer.getmRightDriftView().moveGL(-arrowCenter-drift/45, 0);
        mViewDisplayer.getmLeftDriftView().getGlView().requestRender();
        mViewDisplayer.getmRightDriftView().getGlView().requestRender();
    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        mRunner = new MapRunner(this, mMap, wpRoute);
        mRunner.run();
    }
}
