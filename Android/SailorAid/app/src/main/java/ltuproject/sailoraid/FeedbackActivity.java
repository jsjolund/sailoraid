package ltuproject.sailoraid;

import android.content.Context;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import static java.lang.Math.abs;

/**
 * Created by Henrik on 2017-09-05.
 */

public class FeedbackActivity extends AppCompatActivity implements SensorEventListener {

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

        // Instantiate SensorManager
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        // Get Accelerometer sensor
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);


        /*
        Draw the boat alignment
         */
        LinearLayout linearLayout = (LinearLayout) findViewById(R.id.boatalignmentholder);
        mBoatView = new BoatView(getApplicationContext(),
                BitmapFactory.decodeResource(getResources(), R.drawable.boat_alignement));
        mBoatView.setZOrderOnTop(true);    // necessary
        linearLayout.addView(mBoatView);

        /*
        Draw needle for pressure measurement
         */
        LinearLayout linearPressureLayout = (LinearLayout) findViewById(R.id.pressureMeter);
        mNeedleView = new NeedleView(getApplicationContext(),
                BitmapFactory.decodeResource(getResources(), R.drawable.needle));
        linearPressureLayout.addView(mNeedleView);
        mNeedleView.setZOrderOnTop(true);    // necessary




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

    @Override
    protected void onResume() {
        super.onResume();
        // register Listener for SensorManager and Accelerometer sensor
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_UI);
    }

    @Override
    protected void onPause() {

        super.onPause();
        // unregister Listener for SensorManager
        mSensorManager.unregisterListener(this);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // can be safely ignored for tutorial
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        final float alpha = 0.8f;

        float gravity[] = new float[3];
        gravity[0] = alpha * gravity[0] + (1-alpha) * event.values[0];
        gravity[1] = alpha * gravity[1] + (1-alpha) * event.values[1];
        gravity[2] = alpha * gravity[2] + (1-alpha) * event.values[2];

        this.x = event.values[0]- gravity[0];
        this.y = event.values[1]- gravity[1];
        this.z = event.values[2]- gravity[2];
        mBoatView.setXYZ(this.x, this.y, this.z);
        mNeedleView.setPressure(abs(this.x));
        setDegreeText((int) (this.x*10));
        setPressureText((int) abs(this.x));
    }
    private void setDegreeText(int degree){
        TextView tv = (TextView) findViewById(R.id.degreeText);
        tv.setText(degree + "\u2103");
    }
    private void setPressureText(int pressure){
        TextView tv = (TextView) findViewById(R.id.pressureText);
        tv.setText(pressure + " Psi");
    }
}
