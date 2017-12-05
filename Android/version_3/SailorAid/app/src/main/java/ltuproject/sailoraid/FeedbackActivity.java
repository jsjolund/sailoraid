package ltuproject.sailoraid;

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
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

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
import ltuproject.sailoraid.graphics.RotatableGLView;
import ltuproject.sailoraid.location.Locator;

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

/**
 * Created by Henrik on 2017-09-05.
 */

public class FeedbackActivity extends AppCompatActivity implements OnMapReadyCallback{
    private final static String TAG = FeedbackActivity.class.getSimpleName();

    /*
      Positioning and scale of rotatable images
     */

    private static final float BOAT_SCALE_X = 2.8f;
    private static final float BOAT_SCALE_Y = 2.8f;
    private static final float NEEDLE_SCALE_X = 2.8f;
    private static final float NEEDLE_SCALE_Y = 1.0f;
    private static final float COMPASS_BOAT_SCALE_X = 1.6f;
    private static final float COMPASS_BOAT_SCALE_Y = 1.8f;
    private static final float DRIFT_ARROW_SCALE_X = 1.0f;
    private static final float DRIFT_ARROW_SCALE_Y = 3.0f;
    private static final float NEEDLE_BOTTOM_POS = -1.5f;
    private static final float NEEDLE_X_POS = -0.3f;
    private static final float DRIFT_ARROW_CENTER = 4.2f;
    private static final float MAP_DRIFT_ARROW_CENTER = 5.2f;
    private static final float WAVE_SCALE_X = 1.9f;
    private static final float WAVE_SCALE_Y = 1.9f;
    private static final float WAVE_BOAT_SCALE_X = 1.4f;
    private static final float WAVE_BOAT_SCALE_Y = 1.4f;
    private static final float WAVE_X_POS = 2.0f;
    private static final float WAVE_Y_POS = -0.5f;
    private static final float WAVE_BOAT_X_POS = -1.8f;
    private static final float WAVE_BOAT_Y_POS = 0.2f;
    private static final float GRAPHIC_DAGGER_Y_POS = -3.3f;
    private static final float GRAPHIC_DAGGER_X_POS = -0.3f;
    private static final float GRAPHIC_DAGGER_Y_SCALE = 6.2f;
    private static final float GRAPHIC_DAGGER_X_SCALE = 6.2f;
    private static final float GRAPHIC_PRESSURE_X_POS = 0.0f;
    private static final float GRAPHIC_PRESSURE_Y_POS = -1.5f;
    private static final float GRAPHIC_PRESSURE_X_SCALE = 2.8f;
    private static final float GRAPHIC_PRESSURE_Y_SCALE = 1.0f;
    private static final float GRAPHIC_SPEED_X_POS = -7.0f;
    private static final float GRAPHIC_SPEED_Y_POS = 0.4f;
    private static final float GRAPHIC_SPEED_X_SCALE = 6.0f;
    private static final float GRAPHIC_SPEED_Y_SCALE = 0.2f;
    private static final float GRAPHIC_DRIFT_ARROW_CENTER = 5.8f;
    private ViewStates mCurrentViewState;

    public static final float KM_TO_KNOTS = 1/1.852f;

    /*
      Bluetooth states
     */
    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;
    private static final int INTERVAL = 250;
    private static final int UNINITIALIZED = 9999;
    private int mInterval = 1000;

    /*
      State constants describing states and limits from sensor data
     */
    private enum ViewStates {SIMPLE, INCLINE, MAP, GRAPHIC}
    private enum FeedbackStates {CLEAR, HEEL, DRIFT, HAULING, REEFING, HARDWIND, WRSPEED, LRSPEED, HIKE, KEELHAUL, RUNNINGHIGH, RUNNINGLOW, LANDCRAB}
    private FeedbackStates lastFeedbackState;

    private FeedbackStates mFeedbackState;
    private static int HEELINCLINEUPPERLIMIT = 30;
    private static int HEELINCLINEMIDLIMIT = 20;
    private static int HEELINCLINELOWERLIMIT = 10;
    private static float SOGWRLIMIT = 65.45f;
    private static float SOGLASERLIMIT = 16.8f;
    private static int SOGUPPERLIMIT = 12;
    private static int SOGLOWERLIMIT = 6;
    private static int RANGEMAXLIMIT = 80;
    private static int RANGEUPPERLIMIT = 70;
    private static int RANGELOWELIMIT = 10;
    private static int RANGEMIDLIMIT = 40;
    private static int PRESSUREUPPERLIMIT = 80;
    private static int PRESSUREMIDLIMIT = 50;
    private static int PRESSURELOWLIMIT = 20;
    private static int DRIFTUPPERLIMIT = 1;
    private static float DRIFTLOWERLIMIT = 0.5f;


    private Handler mFeedbackHandler;
    private Handler mVibratorHandler;
    private Handler mWaveHandler;
    private Handler mMapHandler;



    private IntentFilter filter;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics;
    private RotatableGLView mInclineBoatView, mCompassView,mPressureNeedleView,mLeftDriftView,mRightDriftView,mWaveView,mDaggerView,mSpeedView;
    private TextView feedbackText;
    private SailLog mLogService;
    private Polyline travelRoutePolyline;
    private Marker hereMarker;
    private Polyline wayPointPolyline;
    private LatLng boat;
    private final Map<Integer, Marker> mMarkers = new ConcurrentHashMap<Integer, Marker>();
    private boolean logging = false;
    private BTLEConnection mBluetoothLeService;
    private Menu mMenu;
    private Bundle mSavedInstaceStare;
    private String prevTime;
    private int lastView = 0;
    static private List<Float> waveData = new ArrayList<Float>();
    public static synchronized void getWaveData(List<Float> output) {
        output.addAll(waveData);
    }
    private List<Float> filteredData = new ArrayList<Float>();
    private List<Float> sogData = new ArrayList<>();
    private float wavePeriod;
    private boolean isWaving = false;
    private float wavePos = 0;
    private boolean isSpeaking = false;
    /*
    Sensor variables
     */
    private float inclineX, inclineY, bearingZ;
    private float direction, speed;
    private double drift;
    private float pressure, leftPressure, rightPressure, maxPressure;
    private float range;
    private float batteryPower;
    private LatLng nextEstimate = new LatLng(0,0);
    private LatLng gpsPos = new LatLng(0,0);
    private Button btnMap;
    private TextToSpeech txtToSpeech;
    private int startFlag = 0;
    private GoogleMap mMap;
    HashMap<String, String> map = new HashMap<String, String>();
    private boolean enableVoice = false;
    private boolean enableVibration = false;
    //Location variables
    static private List<LatLng> estimatedRoute = new ArrayList<LatLng>();
    public static synchronized void getEstimatedRoute(List<LatLng> output) {
        output.addAll(estimatedRoute);
    }
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
        Toolbar myToolbar = findViewById(R.id.feedback_toolbar);
        setSupportActionBar(myToolbar);


        this.feedbackText = new TextView(this); //findViewById(R.id.feedbackText);
        feedbackText.setText("Yo");
        feedbackText.setLayoutParams(new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.WRAP_CONTENT));
        btnMap = findViewById(R.id.mapviewbtn);
        mLogService = null;
        initFilter();
        displayInclineFocus();
        mFeedbackHandler = new Handler();
        mVibratorHandler = new Handler();
        mWaveHandler = new Handler();

        txtToSpeech=new TextToSpeech(getApplicationContext(), new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int status) {
                txtToSpeech.setLanguage(Locale.UK);
                if (status == TextToSpeech.SUCCESS) {
                    txtToSpeech.setOnUtteranceProgressListener(new UtteranceProgressListener() {
                        @Override
                        public void onDone(String utteranceId) {
                            isSpeaking = false;
                        }

                        @Override
                        public void onError(String utteranceId) {
                        }

                        @Override
                        public void onStart(String utteranceId) {
                            isSpeaking = true;
                        }
                    });
                }
            }
        });

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
                startActivity(intent);
            }
        });
    }

    private void changeView(int viewId){
        if (lastView == 0){
            hideInclineFocus();
        } else if (lastView == 1){
            hideMapFocus();
        } else if (lastView == 2){
            hideSimpleFocus();
        } else if (lastView == 3){
            hideGraphicFocus();
        }
        lastView = viewId;
        if (viewId == 0){
            displayInclineFocus();
        }else if (viewId == 1){
            displayMapFocus();
        }else if (viewId == 2){
            displaySimpleFocus();
        }else if (viewId == 3){
            displayGraphicFocus();
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
        Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
        if (mLogService == null){
            Intent logServiceIntent = new Intent(getApplicationContext(), SailLog.class);
            bindService(logServiceIntent, mLogServiceConnection, BIND_AUTO_CREATE);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mGattUpdateReceiver);
        stopRepeatingTask();
        clearBitmaps();
        Runtime.getRuntime().gc();
        if (mLogServiceConnection!= null){
            unbindService(mLogServiceConnection);
        }
        if (mServiceConnection != null){
            unbindService(mServiceConnection);
        }
    }

    public void clearBitmaps(){
        if(mInclineBoatView != null){
            mInclineBoatView.clearBitmap();
        }
        if(mCompassView != null){
            mCompassView.clearBitmap();
        }
        if(mSpeedView != null){
            mSpeedView.clearBitmap();
        }
        if(mDaggerView != null){
            mDaggerView.clearBitmap();
        }
        if(mRightDriftView != null){
            mRightDriftView.clearBitmap();
        }
        if(mLeftDriftView != null){
            mLeftDriftView.clearBitmap();
        }
        if(mPressureNeedleView != null){
            mPressureNeedleView.clearBitmap();
        }
    }
    @Override
    public void onLowMemory() {
        super.onLowMemory();
    }

    @Override
    protected void onPause() {
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
                enableVoice = item.isChecked();
                return true;
            case R.id.vibration_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                } else{
                    item.setChecked(false);
                }
                enableVibration = item.isChecked();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    /*
    Sets dynamic pictures on views defined by Id
     */
    private void displayOnView(int id, RotatableGLView view){
        LinearLayout linearLayout = findViewById(id);
        linearLayout.addView(view.getGlView());
    }
    private void hideFromView(int id, RotatableGLView view){
        LinearLayout linearPressureLayout = findViewById(id);
        linearPressureLayout.removeView(view.getGlView());
        view.clearBitmap();
        view.getGlView().onPause();
        view.clearRenderer();
    }

    private void displayMapFocus(){
        mCurrentViewState = ViewStates.MAP;
        initInclineBoat(R.drawable.boat_alignement, BOAT_SCALE_X, BOAT_SCALE_Y);
        initCompass(R.drawable.rowboat, COMPASS_BOAT_SCALE_X, COMPASS_BOAT_SCALE_Y);
        initDrift(R.drawable.left_drift_arrow, R.drawable.right_drift_arrow, DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        displayOnView(R.id.inclineMapFocusViewHolder, mInclineBoatView);
        displayOnView(R.id.compassMapFocusViewHolder, mCompassView);
        displayOnView(R.id.leftDriftMapFocus, mLeftDriftView);
        displayOnView(R.id.rightDriftMapFocus, mRightDriftView);
        LinearLayout ll = findViewById(R.id.feedbackMapView);
        feedbackText.setTextSize(34);
        feedbackText.setGravity(Gravity.CENTER);
        ll.addView(feedbackText);
        mLeftDriftView.moveGL(MAP_DRIFT_ARROW_CENTER,0);
        mRightDriftView.moveGL(-MAP_DRIFT_ARROW_CENTER,0);
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.mapFragment);
        mMapHandler = new Handler();
        mapFragment.getMapAsync(this);
    }
    private void hideMapFocus(){
        hideFromView(R.id.inclineMapFocusViewHolder, mInclineBoatView);
        hideFromView(R.id.compassMapFocusViewHolder, mCompassView);
        hideFromView(R.id.leftDriftMapFocus, mLeftDriftView);
        hideFromView(R.id.rightDriftMapFocus, mRightDriftView);
        LinearLayout ll = findViewById(R.id.simplefeedbackTextLayout);
        ll.removeView(feedbackText);
        stopRepeatingMapTask();
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.mapFragment);
        mapFragment.onPause();
    }
    private void hideSimpleFocus(){
        LinearLayout ll = findViewById(R.id.simplefeedbackTextLayout);
        ll.removeView(feedbackText);
        mWaveHandler.removeCallbacks(mWaveRunner);
        hideFromView(R.id.simpleboatalignmentholder, mInclineBoatView);
        hideFromView(R.id.simplewaveholder, mWaveView);
    }
    private void hideGraphicFocus(){
        hideFromView(R.id.graphicDaggerHolder, mDaggerView);
        hideFromView(R.id.graphicPressureHolder, mPressureNeedleView);
        hideFromView(R.id.graphicSpeedHolder, mSpeedView);
        hideFromView(R.id.graphicDriftLeftHolder, mLeftDriftView);
        hideFromView(R.id.graphicDriftRightHolder, mRightDriftView);
        hideFromView(R.id.graphicInclineHolder, mInclineBoatView);
        hideFromView(R.id.graphicCompassHolder, mCompassView);
    }
    private void displaySimpleFocus(){
        mCurrentViewState = ViewStates.SIMPLE;
        initInclineBoat(R.drawable.boat_alignement, BOAT_SCALE_X, BOAT_SCALE_Y);
        initWave(R.drawable.wave, R.drawable.pico_dyn, WAVE_SCALE_X, WAVE_SCALE_Y, WAVE_BOAT_SCALE_X, WAVE_BOAT_SCALE_Y);
        LinearLayout ll = findViewById(R.id.simplefeedbackTextLayout);
        feedbackText.setTextSize(34);
        feedbackText.setGravity(Gravity.CENTER);
        ll.addView(feedbackText);
        displayOnView(R.id.simpleboatalignmentholder, mInclineBoatView);
        displayOnView(R.id.simplewaveholder, mWaveView);
        mWaveView.moveGL2(WAVE_BOAT_X_POS, WAVE_BOAT_Y_POS);
        mWaveView.moveGL(WAVE_X_POS, WAVE_Y_POS);
    }
    private void displayInclineFocus(){
        mCurrentViewState = ViewStates.INCLINE;
        initInclineBoat(R.drawable.boat_alignement, BOAT_SCALE_X, BOAT_SCALE_Y);
        initDrift(R.drawable.left_drift_arrow, R.drawable.right_drift_arrow, DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        initCompass(R.drawable.rowboat, COMPASS_BOAT_SCALE_X, COMPASS_BOAT_SCALE_Y);
        initPressure(R.drawable.needle, NEEDLE_SCALE_X, NEEDLE_SCALE_Y);
        displayOnView(R.id.boatalignmentholder, mInclineBoatView);
        displayOnView(R.id.driftImg, mCompassView);
        displayOnView(R.id.leftDrift, mLeftDriftView);
        displayOnView(R.id.rightDrift, mRightDriftView);
        displayOnView(R.id.pressureMeter, mPressureNeedleView);
        mPressureNeedleView.moveGL(NEEDLE_X_POS, NEEDLE_BOTTOM_POS);
        LinearLayout ll = findViewById(R.id.feedbackTextLayout);
        feedbackText.setTextSize(16);
        feedbackText.setGravity(Gravity.RIGHT);
        ll.addView(feedbackText);
        mLeftDriftView.moveGL(DRIFT_ARROW_CENTER,0);
        mRightDriftView.moveGL(-DRIFT_ARROW_CENTER,0);
    }
    private void displayGraphicFocus(){
        mCurrentViewState = ViewStates.GRAPHIC;
       // initInclineBoat();
       // initDrift();
       // initCompass();
        initPressure(R.drawable.needle, GRAPHIC_PRESSURE_X_SCALE, GRAPHIC_PRESSURE_Y_SCALE);
        initDagger(R.drawable.dagger, GRAPHIC_DAGGER_X_SCALE, GRAPHIC_DAGGER_Y_SCALE);
        initSpeed(R.drawable.speedbar, GRAPHIC_SPEED_X_SCALE, GRAPHIC_SPEED_Y_SCALE);
        initDrift(R.drawable.left_drift_arrow, R.drawable.right_drift_arrow, DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        initInclineBoat(R.drawable.boat_alignement, BOAT_SCALE_X, BOAT_SCALE_Y);
        initCompass(R.drawable.rowboat, COMPASS_BOAT_SCALE_X, COMPASS_BOAT_SCALE_Y);
        displayOnView(R.id.graphicPressureHolder, mPressureNeedleView);
        displayOnView(R.id.graphicDaggerHolder, mDaggerView);
        displayOnView(R.id.graphicSpeedHolder, mSpeedView);
        displayOnView(R.id.graphicDriftRightHolder, mRightDriftView);
        displayOnView(R.id.graphicDriftLeftHolder, mLeftDriftView);
        displayOnView(R.id.graphicInclineHolder, mInclineBoatView);
        displayOnView(R.id.graphicCompassHolder, mCompassView);
        mPressureNeedleView.moveGL(GRAPHIC_PRESSURE_X_POS, GRAPHIC_PRESSURE_Y_POS);
        mDaggerView.moveGL(GRAPHIC_DAGGER_X_POS,GRAPHIC_DAGGER_Y_POS);
        mSpeedView.moveGL(GRAPHIC_SPEED_X_POS,GRAPHIC_SPEED_Y_POS);
        mLeftDriftView.moveGL(GRAPHIC_DRIFT_ARROW_CENTER,0);
        mRightDriftView.moveGL(-GRAPHIC_DRIFT_ARROW_CENTER,0);
    }
    private void hideInclineFocus(){
        hideFromView(R.id.boatalignmentholder, mInclineBoatView);
        hideFromView(R.id.driftImg, mCompassView);
        hideFromView(R.id.pressureMeter, mPressureNeedleView);
        hideFromView(R.id.leftDrift, mLeftDriftView);
        hideFromView(R.id.rightDrift, mRightDriftView);
        LinearLayout ll = findViewById(R.id.feedbackTextLayout);
        ll.removeView(feedbackText);
    }
    private void initInclineBoat(int drawable, float scaleX, float scaleY){
        mInclineBoatView = new RotatableGLView(this,decodeSampledBitmapFromResource(getResources(),
                drawable, 180, 180),
                scaleX, scaleY);
    }
    private void initPressure(int drawable, float scaleX, float scaleY){
        mPressureNeedleView = new RotatableGLView(this,
                decodeSampledBitmapFromResource(getResources(), drawable, 20, 20),
                scaleX, scaleY);
    }
    private void initCompass(int drawable, float scaleX, float scaleY){
        mCompassView = new RotatableGLView(this,
                decodeSampledBitmapFromResource(getResources(), drawable, 60,60),
                scaleX, scaleY);
    }
    private void initDrift(int drawable, int drawable2, float scaleX, float scaleY){
        mLeftDriftView = new RotatableGLView(this,
                decodeSampledBitmapFromResource(getResources(), drawable, 10, 10),
                scaleX, scaleY);
        mRightDriftView = new RotatableGLView(this,
                decodeSampledBitmapFromResource(getResources(), drawable2, 10, 10),
                scaleX, scaleY);
    }
    private void initWave(int drawable, int drawable2, float scaleX, float scaleY, float scaleX2, float scaleY2){
        mWaveView = new RotatableGLView(this,
                decodeSampledBitmapFromResource(getResources(), drawable, 140, 140), decodeSampledBitmapFromResource(getResources(), drawable2, 100,100),
                scaleX, scaleY, scaleX2, scaleY2);
    }
    private void initDagger(int drawable, float scaleX, float scaleY){
        mDaggerView = new RotatableGLView(this,
                decodeSampledBitmapFromResource(getResources(), drawable, 120, 180),
                scaleX, scaleY);
    }
    private void initSpeed(int drawable, float scaleX, float scaleY){
        mSpeedView = new RotatableGLView(this,
                decodeSampledBitmapFromResource(getResources(), drawable, 120, 20),
                scaleX, scaleY);
    }
    public static Bitmap decodeSampledBitmapFromResource(Resources res, int resId,
                                                         int reqWidth, int reqHeight) {

        // First decode with inJustDecodeBounds=true to check dimensions
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeResource(res, resId, options);

        // Calculate inSampleSize
        options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight);

        // Decode bitmap with inSampleSize set
        options.inJustDecodeBounds = false;
        return BitmapFactory.decodeResource(res, resId, options);
    }
    public static int calculateInSampleSize(
            BitmapFactory.Options options, int reqWidth, int reqHeight) {
        // Raw height and width of image
        final int height = options.outHeight;
        final int width = options.outWidth;
        int inSampleSize = 1;

        if (height > reqHeight || width > reqWidth) {

            final int halfHeight = height / 2;
            final int halfWidth = width / 2;

            // Calculate the largest inSampleSize value that is a power of 2 and keeps both
            // height and width larger than the requested height and width.
            while ((halfHeight / inSampleSize) >= reqHeight
                    && (halfWidth / inSampleSize) >= reqWidth) {
                inSampleSize *= 2;
            }
        }

        return inSampleSize;
    }

    /*
    Given a found service from a connected device checks all services that are known by SampleGattAttributes List, adds known to the bluetooth service for registering
     */
    public void getKnownGattServices(List<BluetoothGattService> gattServices){
        if (gattServices == null) return;
        String uuid = null;
        // Loops through available GATT Services.
        for (BluetoothGattService gattService : gattServices) {
            uuid = gattService.getUuid().toString();
            if (uuid.equals(SampleGattAttributes.ACCELEROMETER_SERVICE.toString())
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
            if (mBluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                invalidateOptionsMenu();
                mBluetoothLeService.discoverServices();
            } else if (mBluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                invalidateOptionsMenu();
                stopRepeatingTask();
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

    /**
    Display data received from bluetooth device.
     */
    public void displayData(String data, String dataType) {
        if (data != null) {
            String time = new SimpleDateFormat("HHmmssSSS").format(new Date());
            if(dataType.equals(DATA_TYPE_INCLINE)){
                // Get Roll, pitch and yaw
                String[] accelerometer = data.split(":");
                this.inclineY = Float.parseFloat(accelerometer[0]);
                this.inclineX = Float.parseFloat(accelerometer[1]);
                this.bearingZ = Float.parseFloat(accelerometer[2]);

                if (mCurrentViewState == ViewStates.INCLINE){
                    setTiltText(inclineX);
                    TextView tv = findViewById(R.id.tiltYText);
                    tv.setText(String.format("%.1f\u00B0", inclineY));
                    tv = findViewById(R.id.zText);
                    tv.setText(String.format("%.2f", wavePeriod)+"Hz");
                }
                if (mCurrentViewState == ViewStates.INCLINE
                        || mCurrentViewState == ViewStates.MAP
                        || mCurrentViewState == ViewStates.SIMPLE
                        || mCurrentViewState == ViewStates.GRAPHIC){
                    mInclineBoatView.rotateGl((int) this.inclineX);
                    mInclineBoatView.getGlView().requestRender();
                }
                if (mCurrentViewState == ViewStates.SIMPLE){
                    mWaveView.rotateGl2((int) inclineY);
                    mWaveView.getGlView().requestRender();
                    if (!isWaving){
                        mWaveRunner.run();
                    }
                }
                waveData.add(inclineY);
                if(waveData.size() > 500){
                    wavePeriod = calculateWaveFrequency(waveData);
                    for (int i = 0; i<25; i++){
                        waveData.remove(0);
                    }
                    if (mLogService != null){
                        if(mLogService.isLogging()){
                            mLogService.writeToLog(DATA_TYPE_WAVES +":" +time +":" +this.wavePeriod);
                        }
                    }
                }
                if (mCurrentViewState == ViewStates.INCLINE
                        || mCurrentViewState == ViewStates.MAP
                        || mCurrentViewState == ViewStates.GRAPHIC){
                    mCompassView.rotateGl(abs(bearingZ));
                    mCompassView.getGlView().requestRender();
                }
                //float positionBoat = this.x / 100;
                //mInclineBoatView.moveGL(positionBoat, positionBoat);
                // Rotates compass with pitch
                if(mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_INCLINE +":" +time  +":" +this.inclineX);
                        //mLogService.writeToLog(DATA_TYPE_COMPASS +":" +time +":" +this.bearingZ);
                    }
                }
            } else if(dataType.equals(DATA_TYPE_TEMPERATURE)){
                data = data.replace(',', '.');
                setTempText(Float.parseFloat(data));
                if (mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_TEMPERATURE +":" +time +":" +data);
                    }
                }
            } else if(dataType.equals(DATA_TYPE_PRESSURE)){
                data = data.replace(',', '.');
                String[] loadCell = data.split(":");
                // Todo uncomment
               // this.leftPressure = Float.parseFloat(loadCell[0]);
               // this.rightPressure = Float.parseFloat(loadCell[1]);
               // this.maxPressure = Math.max(leftPressure, rightPressure);
                this.maxPressure = Float.parseFloat(loadCell[0])-18;

                setPressureText(this.maxPressure);
                //mNeedleView.setPressure(pressure/1000 - 1.01325f);
                /* Todo Calibrate
                    Range of move needle Min moveGL -1.5f, max moveGl 1.2f
                    No calibration made
                */

                if(mCurrentViewState == ViewStates.INCLINE){
                    mPressureNeedleView.moveGL(NEEDLE_X_POS, this.maxPressure/40 -1.2f);
                }
                if (mCurrentViewState == ViewStates.GRAPHIC) {
                    // Todo pressure
                    mPressureNeedleView.moveGL(GRAPHIC_PRESSURE_X_POS, this.maxPressure/40 -1.2f);
                    mPressureNeedleView.getGlView().requestRender();
                }
                if (mLogService != null){
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_PRESSURE +":" +time  +":" +this.maxPressure);
                    }
                }
            } else if(dataType.equals(DATA_TYPE_HUMIDITY)){
                data = data.replace(',', '.');
                setHumText(Float.parseFloat(data));
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
                this.speed = Float.parseFloat(pos[3])*KM_TO_KNOTS;
                this.direction = Float.parseFloat(pos[4]);
                this.batteryPower = Float.parseFloat(pos[5]);
                showBatteryLeft(this.batteryPower);
                if (mCurrentViewState == ViewStates.INCLINE){
                    TextView tv = findViewById(R.id.batPer);
                    tv.setText(String.format("%.0f",batteryPower) +"%");
                }
                if (latitude != 0f && longitude != 0f) {
                    LatLng currPos = new LatLng(latitude, longitude);
                    //double speed_mps = 0;
                    if (this.gpsPos.latitude != 0f && this.gpsPos.longitude != 0f){
                        //Calculate distance and speed from last point, possibly could filter moving avg
                        double dist = Locator.distance_on_geoid(currPos.latitude, currPos.longitude, this.gpsPos.latitude, this.gpsPos.longitude);

                        sogData.add(this.speed);
                        if(sogData.size() >= 4){
                            this.speed = movingAverageFilter(sogData, 3);
                            sogData.remove(0);
                            sogData.remove(sogData.size()-1);
                            sogData.add(this.speed);
                        }
                        // Calculate perpendicular drift from the ship navigational bearing will update from previously calculated value to compare with the new values.
                        if (this.nextEstimate.latitude != 0f && this.nextEstimate.longitude != 0f){
                            if(mLogService != null) {
                                if (mLogService.isLogging()) {
                                    mLogService.writeToLog(DATA_TYPE_ESTPOSITION + ":" + time + ":" + this.nextEstimate.latitude + ":" + this.nextEstimate.longitude + ":" + direction);
                                }
                            }
                            this.drift = Locator.distance_on_geoid(this.nextEstimate.latitude, this.nextEstimate.longitude, latitude, longitude);
                        } else{
                            this.drift = UNINITIALIZED;
                        }
                        this.nextEstimate = Locator.calcNextEstimatePos(new LatLng(latitude,longitude), dist, direction);
                        if (this.nextEstimate.latitude != 0 && this.nextEstimate.longitude != 0){
                            this.estimatedRoute.add(this.nextEstimate);
                        }
                        if (mCurrentViewState == ViewStates.INCLINE){
                            TextView tv = findViewById(R.id.driftText);
                            if (this.drift == UNINITIALIZED){
                                tv.setText("Nan");
                            } else{
                                tv.setText(String.format("%.1f", this.drift) +"m");
                                tv = findViewById(R.id.speedText);
                                tv.setText(String.format("%.1f", this.speed) +"kn");
                            }
                        }
                        if (mCurrentViewState == ViewStates.INCLINE){
                            displayDrift((float) this.drift, DRIFT_ARROW_CENTER);
                        }
                        if (mCurrentViewState == ViewStates.MAP) {
                            displayDrift((float) this.drift, MAP_DRIFT_ARROW_CENTER);
                        }
                        if (mCurrentViewState == ViewStates.GRAPHIC) {
                            displayDrift((float) this.drift, GRAPHIC_DRIFT_ARROW_CENTER);
                            mSpeedView.moveGL(GRAPHIC_SPEED_X_POS+(this.speed/7), GRAPHIC_SPEED_Y_POS);
                            mSpeedView.getGlView().requestRender();
                        }
                    }
                    this.gpsPos = currPos;
                    this.estimatedRoute.add(this.gpsPos);

                    travelRoute.add(gpsPos);
                    if(mLogService != null) {
                        if (mLogService.isLogging()){
                            mLogService.writeToLog(DATA_TYPE_POSITION + ":" + time + ":" + latitude + ":" + longitude + ":" +direction);
                            mLogService.writeToLog(DATA_TYPE_SOG + ":" +time +":" +this.speed);
                            mLogService.writeToLog(DATA_TYPE_DRIFT + ":" +time +":" +this.drift);
                            mLogService.writeToLog(DATA_TYPE_COMPASS + ":" +time +":" +direction);
                        }
                    }
                }
            } else if (dataType.equals(DATA_TYPE_RANGE)){
                data = data.replace(',', '.');
                this.range = Float.parseFloat(data);
                if (mCurrentViewState == ViewStates.INCLINE){
                    TextView tv = findViewById(R.id.rangeText);
                    tv.setText(String.format("%.0f",this.range) +"cm");
                }
                if (mCurrentViewState == ViewStates.GRAPHIC){
                    if (this.range < 80){
                        mDaggerView.moveGL(GRAPHIC_DAGGER_X_POS, GRAPHIC_DAGGER_Y_POS +(this.range/35));
                    } else{
                        mDaggerView.moveGL(GRAPHIC_DAGGER_X_POS, GRAPHIC_DAGGER_Y_POS +(RANGEMAXLIMIT/35));
                    }
                    mDaggerView.getGlView().requestRender();
                }
                if(mLogService != null) {
                    if (mLogService.isLogging()){
                        mLogService.writeToLog(DATA_TYPE_RANGE + ":" + time + ":" + this.range);
                    }
                }
            }
        }
    }

    Runnable mWaveRunner = new Runnable() {

        @Override
        public void run() {
            try {
                isWaving = true;
                wavePos +=wavePeriod;
                mWaveView.moveGL(WAVE_X_POS-wavePos,WAVE_Y_POS);
                mWaveView.moveGL2(WAVE_BOAT_X_POS+0.60f*wavePos,WAVE_BOAT_Y_POS);
                mWaveView.getGlView().requestRender();


            } finally {
                if (wavePos > 5f && wavePeriod > 0){
                    mWaveView.moveGL(WAVE_X_POS,WAVE_Y_POS);
                    mWaveView.moveGL2(WAVE_BOAT_X_POS, WAVE_BOAT_Y_POS);
                    mWaveView.getGlView().requestRender();
                    wavePos = 0;
                    isWaving = false;
                } else{
                    mWaveHandler.postDelayed(mWaveRunner, 200);
                }
            }
        }
    };
    public static float calculateWaveFrequency(List<Float> data) {
        float period;
        float incPos = 0;
        boolean posFlag = false;
        float incNeg = 0;
        boolean negFlag = false;
        for (Float measurement: data){
            if (measurement > 0.00f && !posFlag){
                negFlag = false;
                incPos += 1 ;
                posFlag = true;
            } else if (measurement < 0.00f && !negFlag){
                posFlag = false;
                incNeg += 1 ;
                negFlag = true;
            }
        }
        period = (incPos + incNeg) / 2.00f;
        return period/10.00f;
    }

    public static float movingAverageFilter(List<Float> data, int filterLength){
      float sum = 0;
        for (Float value : data){
            sum+=value;
        }
        return sum/data.size();
    }
    /*
        Gives User feedback tips based on sensor data
     */
    Runnable mStateChecker = new Runnable() {
        @Override
        public void run() {
            try {
                /*
                Todo add states depending on sensor data to give sailor usable feedback
                 */
                String txt ="";
                if(!isSpeaking){
                    if(abs(inclineX) > HEELINCLINEUPPERLIMIT){
                        mFeedbackState = FeedbackStates.HEEL;
                    } else if (speed > SOGWRLIMIT){
                        mFeedbackState = FeedbackStates.WRSPEED;
                    } else if (speed > SOGLASERLIMIT){
                        mFeedbackState = FeedbackStates.LRSPEED;
                    } else if (abs(inclineX) > HEELINCLINELOWERLIMIT && maxPressure > PRESSUREUPPERLIMIT){
                        mFeedbackState = FeedbackStates.REEFING;
                    } else if (abs(inclineX) > HEELINCLINELOWERLIMIT && maxPressure > PRESSUREMIDLIMIT && maxPressure < PRESSUREUPPERLIMIT){
                        mFeedbackState = FeedbackStates.HIKE;
                    } else if (abs(inclineX) > HEELINCLINEMIDLIMIT && range > RANGELOWELIMIT){
                        mFeedbackState = FeedbackStates.KEELHAUL;
                    } else if (abs(inclineX) > HEELINCLINELOWERLIMIT && range < RANGEMIDLIMIT && pressure < PRESSURELOWLIMIT && drift < DRIFTLOWERLIMIT){
                        mFeedbackState = FeedbackStates.RUNNINGLOW;
                    } else if (abs(inclineX) < HEELINCLINELOWERLIMIT && range > RANGEMIDLIMIT && pressure < PRESSURELOWLIMIT && drift < DRIFTLOWERLIMIT){
                        mFeedbackState = FeedbackStates.RUNNINGHIGH;
                    } else if (drift > DRIFTUPPERLIMIT && range > RANGELOWELIMIT ){
                        mFeedbackState = FeedbackStates.DRIFT;
                    } else if (abs(inclineX) < HEELINCLINELOWERLIMIT && pressure > PRESSUREMIDLIMIT && drift < DRIFTLOWERLIMIT && speed > SOGUPPERLIMIT){
                        mFeedbackState = FeedbackStates.CLEAR;
                    } else if (speed < SOGLOWERLIMIT){
                        mFeedbackState = FeedbackStates.LANDCRAB;
                    }
                    if (!mFeedbackState.equals(lastFeedbackState)){
                        if (startFlag == 0){
                            startFlag = 1;
                        } else {
                            talkFeedback();
                        }
                    }
                    lastFeedbackState = mFeedbackState;
                }
            } finally {
                // 100% guarantee that this always happens, even if
                // your update method throws an exception
                mFeedbackHandler.postDelayed(mStateChecker, INTERVAL);
            }
        }
    };

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
    private void talkFeedback(){
        IntervalVibrator mVibrator =  new IntervalVibrator(this,0, 0, 0);
        String txt = "";
        switch (mFeedbackState) {
            case HEEL:
                txt = "All hands! \n Abandon ship!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.red));
                feedbackText.setBackground(getDrawable(R.color.black));
                mVibrator = new IntervalVibrator(this, 1, 1000, 500);
                break;
            case WRSPEED:
                txt = "Yargh matey! \n New world record speed!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.green));
                feedbackText.setBackground(getDrawable(R.color.darkblue));
                mVibrator = new IntervalVibrator(this, 10, 1000, 300);
                break;
            case LRSPEED:
                txt = "Ahoy land crab! \n New laser record speed!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.darkgreen));
                feedbackText.setBackground(getDrawable(R.color.laserblue));
                mVibrator = new IntervalVibrator(this, 5, 200, 400);
                break;
            case REEFING:
                txt = "Furl the jib \n Lower the mainsail";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.orange));
                feedbackText.setBackground(getDrawable(R.color.blue));
                mVibrator = new IntervalVibrator(this, 3, 200, 700);
                break;
            case HIKE:
                txt = "Hike more! \n You can do it!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.darkblue));
                feedbackText.setBackground(getDrawable(R.color.green));
                mVibrator = new IntervalVibrator(this, 3, 200, 700);
                break;
            case KEELHAUL:
                txt = "Lower the centerboard";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.darkblue));
                feedbackText.setBackground(getDrawable(R.color.darkgreen));
                mVibrator = new IntervalVibrator(this, 3, 200, 700);
                break;
            case HAULING:
                txt = "Ahoy! \n Lift Centerboard";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.blue));
                feedbackText.setBackground(getDrawable(R.color.yellow));
                mVibrator = new IntervalVibrator(this, 2, 1200, 800);
                break;
            case RUNNINGLOW:
                txt = "Lower the centerboard! \n Death roll imminent!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.red));
                feedbackText.setBackground(getDrawable(R.color.blue));
                mVibrator = new IntervalVibrator(this, 2, 200, 600);
                break;
            case RUNNINGHIGH:
                txt = "Dead ahead! \n Full speed!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.red));
                feedbackText.setBackground(getDrawable(R.color.green));
                mVibrator = new IntervalVibrator(this, 2, 200, 600);
                break;
            case DRIFT:
                txt = "Ship Adrift! \n Lower Centerboard more!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.orange));
                feedbackText.setBackground(getDrawable(R.color.black));
                mVibrator = new IntervalVibrator(this, 2, 100, 500);
                break;
            case CLEAR:
                txt = "Your an able seaman \n Congrats!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.darkgreen));
                feedbackText.setBackground(getDrawable(R.color.orange));
                mVibrator = new IntervalVibrator(this, 3, 200, 500);
                break;
            case LANDCRAB:
                txt = "Speed up land crab!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(getColor(R.color.darkgreen));
                feedbackText.setBackground(getDrawable(R.color.orange));
                mVibrator = new IntervalVibrator(this, 3, 200, 500);
                break;
            default:

        }
        if (enableVoice){
            isSpeaking = true;
            map.put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, "UniqueID");
            txtToSpeech.speak(txt, TextToSpeech.QUEUE_FLUSH, map);
        }
        txtToSpeech.setLanguage(Locale.ENGLISH);
        if (enableVibration){
            mVibrator.run();
        }
    }


    private void startRepeatingTask() {
        mStateChecker.run();
    }

    private void stopRepeatingTask() {
        mFeedbackHandler.removeCallbacks(mStateChecker);
    }

    private void displayDrift(float drift, float arrowCenter){
        // Use pitch for testing drifting feedback
            // Max resize ARROW_SCALE*x = 12 also move from center with -x/6
        mLeftDriftView.resizeGL(DRIFT_ARROW_SCALE_X*drift/15, DRIFT_ARROW_SCALE_Y);
        mLeftDriftView.moveGL(arrowCenter-drift/45, 0);
        mRightDriftView.resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        mRightDriftView.moveGL(-arrowCenter, 0);
        mLeftDriftView.resizeGL(DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        mLeftDriftView.moveGL(arrowCenter, 0);
        mRightDriftView.resizeGL(DRIFT_ARROW_SCALE_X*(-drift/15), DRIFT_ARROW_SCALE_Y);
        mRightDriftView.moveGL(-arrowCenter-drift/45, 0);
        mLeftDriftView.getGlView().requestRender();
        mRightDriftView.getGlView().requestRender();
    }




    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        startRepeatingMapTask();
    }
    Runnable mMapStatusChecker = new Runnable() {
        @Override
        public void run() {
            try {
                // Update travel route on timer update
                PolylineOptions newTravelRoute = new PolylineOptions();
                newTravelRoute.addAll(travelRoute);
                if (travelRoutePolyline != null)
                    travelRoutePolyline.remove();
                travelRoutePolyline = mMap.addPolyline(newTravelRoute);

                PolylineOptions newWaypointRoute = new PolylineOptions();
                if (travelRoute.size() > 0) {
                    if (hereMarker != null) {
                        hereMarker.remove();
                    }
                    boat = travelRoute.get(travelRoute.size() - 1);
                    if(!mMarkers.isEmpty()){
                        if (wayPointPolyline != null){
                            wayPointPolyline.remove();
                        }
                        Set<Integer> keySet = mMarkers.keySet();
                        Object[] keys = keySet.toArray();
                        int index = 0;
                        for (int i = 0; i< keySet.size(); i++){
                            if (mMarkers.get(keys[i]).isVisible()){
                                index = i;
                            }
                        }
                        newWaypointRoute.add(boat, mMarkers.get(keys[index]).getPosition());
                        newWaypointRoute.color(getColor(R.color.green));
                        wayPointPolyline = mMap.addPolyline(newWaypointRoute);
                        double dist = Locator.distance_on_geoid(boat.latitude,boat.longitude, mMarkers.get(keys[index]).getPosition().latitude, mMarkers.get(keys[index]).getPosition().longitude);
                        MenuItem distItem = mMenu.findItem(R.id.dist_to_waypoint);
                        //Check i close to wp and remove to redraw
                        if (dist < 10){
                            Marker closeMarker = mMarkers.get(keys[index]);
                            closeMarker.setVisible(false);
                        }
                        distItem.setTitle(String.valueOf((int)dist) +"m to WP");
                    }
                    hereMarker = mMap.addMarker(new MarkerOptions().position(boat).title("You are here"));
                    CameraPosition cameraPosition = new CameraPosition.Builder()
                            .target(boat)
                            .zoom(15)
                            .bearing(0)
                            .tilt(5)
                            .build();
                    mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
                }
            } finally {
                // 100% guarantee that this always happens, even if
                // your update method throws an exception
              mMapHandler.postDelayed(mMapStatusChecker, mInterval);
            }
        }
    };

    void startRepeatingMapTask() {
        mMapStatusChecker.run();
    }

    void stopRepeatingMapTask() {
        mMapHandler.removeCallbacks(mMapStatusChecker);
    }

    private void setTiltText(float degree){
        TextView tv = findViewById(R.id.tiltText);
        String ph = String.format("%.1f\u00B0", degree);
        tv.setText(ph);
    }
    private void setPressureText(float pressure){
        TextView tv = findViewById(R.id.pressureText);
        String ph = String.format("%.1f Psi", pressure);
        tv.setText(ph);
    }
    private void setTempText(float degree){
        TextView tv = findViewById(R.id.tempText);
        String ph = String.format("%.1f\u00B0C", degree);
        tv.setText(ph);
    }
    private void setHumText(float degree){
        TextView tv = findViewById(R.id.humText);
        String ph = String.format("%.1f%%", degree);
        tv.setText(ph);
    }

}
