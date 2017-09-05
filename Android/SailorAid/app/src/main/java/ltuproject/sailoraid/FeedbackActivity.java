package ltuproject.sailoraid;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

/**
 * Created by Henrik on 2017-09-05.
 */

public class FeedbackActivity extends AppCompatActivity implements SensorEventListener {

    /*
    Gyro from phone for demo
     */
    BoatView mBoatView;
    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private float x, y, z;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.feedback_activity);

        // Instantiate SensorManager
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        // Get Accelerometer sensor
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        ConstraintLayout constraintLayout = (ConstraintLayout) findViewById(R.id.constraintFeedbackView);
        mBoatView = new BoatView(getApplicationContext(),
                BitmapFactory.decodeResource(getResources(), R.drawable.boat_alignement));
        constraintLayout.addView(mBoatView);
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
    }
}
