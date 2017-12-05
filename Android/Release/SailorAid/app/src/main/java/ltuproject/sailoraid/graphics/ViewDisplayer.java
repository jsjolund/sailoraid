package ltuproject.sailoraid.graphics;

import android.app.Activity;
import android.content.Context;
import android.view.Gravity;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.google.android.gms.maps.SupportMapFragment;

import ltuproject.sailoraid.FeedbackActivity;
import ltuproject.sailoraid.R;

import static ltuproject.sailoraid.graphics.BitmapScaler.decodeSampledBitmapFromResource;

/**
 * Created by Henrik on 2017-12-05.
 */

public class ViewDisplayer{
    public enum ViewStates {SIMPLE, INCLINE, MAP, GRAPHIC}
  /*
      Positioning and scale of rotatable images
     */
    private RotatableGLView mInclineBoatView, mCompassView,mPressureNeedleView,mLeftDriftView,mRightDriftView,mWaveView,mDaggerView,mSpeedView;


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
    private TextView feedbackText;
    private Context contx;
    public ViewDisplayer(Context contx, TextView fbt){
        this.contx = contx;
        this.feedbackText = fbt;
    }

    /*
   Methods for switching between different layouts.
    */
    private void displayOnView(int id, RotatableGLView view){
        LinearLayout linearLayout = ((Activity) contx).findViewById(id);
        linearLayout.addView(view.getGlView());
    }
    private void hideFromView(int id, RotatableGLView view){
        LinearLayout linearPressureLayout = ((Activity) contx).findViewById(id);
        linearPressureLayout.removeView(view.getGlView());
        view.clearBitmap();
        view.getGlView().onPause();
        view.clearRenderer();
    }

    public void displayMapFocus(){
        mCurrentViewState = ViewStates.MAP;
        initInclineBoat(R.drawable.boat_alignement, BOAT_SCALE_X, BOAT_SCALE_Y);
        initCompass(R.drawable.rowboat, COMPASS_BOAT_SCALE_X, COMPASS_BOAT_SCALE_Y);
        initDrift(R.drawable.left_drift_arrow, R.drawable.right_drift_arrow, DRIFT_ARROW_SCALE_X, DRIFT_ARROW_SCALE_Y);
        displayOnView(R.id.inclineMapFocusViewHolder, mInclineBoatView);
        displayOnView(R.id.compassMapFocusViewHolder, mCompassView);
        displayOnView(R.id.leftDriftMapFocus, mLeftDriftView);
        displayOnView(R.id.rightDriftMapFocus, mRightDriftView);
        LinearLayout ll = ((Activity) contx).findViewById(R.id.feedbackMapView);
        feedbackText.setTextSize(34);
        feedbackText.setGravity(Gravity.CENTER);
        ll.addView(feedbackText);
        mLeftDriftView.moveGL(MAP_DRIFT_ARROW_CENTER,0);
        mRightDriftView.moveGL(-MAP_DRIFT_ARROW_CENTER,0);
       /* SupportMapFragment mapFragment = (SupportMapFragment)  getSupportFragmentManager()
                .findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(contx);*/
    }
    public void hideMapFocus(){
        hideFromView(R.id.inclineMapFocusViewHolder, mInclineBoatView);
        hideFromView(R.id.compassMapFocusViewHolder, mCompassView);
        hideFromView(R.id.leftDriftMapFocus, mLeftDriftView);
        hideFromView(R.id.rightDriftMapFocus, mRightDriftView);
        LinearLayout ll = ((Activity) contx).findViewById(R.id.feedbackMapView);
        ll.removeView(feedbackText);
        /*mRunner.stopMap();
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.mapFragment);
        mapFragment.onPause();*/
    }
    public void hideSimpleFocus(){
        LinearLayout ll = ((Activity) contx).findViewById(R.id.simplefeedbackTextLayout);
        ll.removeView(feedbackText);
        //mWaveHandler.removeCallbacks(mWaveRunner);
        hideFromView(R.id.simpleboatalignmentholder, mInclineBoatView);
        hideFromView(R.id.simplewaveholder, mWaveView);
    }
    public void hideGraphicFocus(){
        hideFromView(R.id.graphicDaggerHolder, mDaggerView);
        hideFromView(R.id.graphicPressureHolder, mPressureNeedleView);
        hideFromView(R.id.graphicSpeedHolder, mSpeedView);
        hideFromView(R.id.graphicDriftLeftHolder, mLeftDriftView);
        hideFromView(R.id.graphicDriftRightHolder, mRightDriftView);
        hideFromView(R.id.graphicInclineHolder, mInclineBoatView);
        hideFromView(R.id.graphicCompassHolder, mCompassView);
    }
    public void displaySimpleFocus(){
        mCurrentViewState = ViewStates.SIMPLE;
        initInclineBoat(R.drawable.boat_alignement, BOAT_SCALE_X, BOAT_SCALE_Y);
        initWave(R.drawable.wave, R.drawable.pico_dyn, WAVE_SCALE_X, WAVE_SCALE_Y, WAVE_BOAT_SCALE_X, WAVE_BOAT_SCALE_Y);
        LinearLayout ll = ((Activity) contx).findViewById(R.id.simplefeedbackTextLayout);
        feedbackText.setTextSize(34);
        feedbackText.setGravity(Gravity.CENTER);
        ll.addView(feedbackText);
        displayOnView(R.id.simpleboatalignmentholder, mInclineBoatView);
        displayOnView(R.id.simplewaveholder, mWaveView);
        mWaveView.moveGL2(WAVE_BOAT_X_POS, WAVE_BOAT_Y_POS);
        mWaveView.moveGL(WAVE_X_POS, WAVE_Y_POS);
    }
    public void displayInclineFocus(){
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
        LinearLayout ll = ((Activity) contx).findViewById(R.id.feedbackTextLayout);
        feedbackText.setTextSize(16);
        feedbackText.setGravity(Gravity.RIGHT);
        ll.addView(feedbackText);
        mLeftDriftView.moveGL(DRIFT_ARROW_CENTER,0);
        mRightDriftView.moveGL(-DRIFT_ARROW_CENTER,0);
    }
    public void displayGraphicFocus(){
        mCurrentViewState = ViewStates.GRAPHIC;
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
    public void hideInclineFocus(){
        hideFromView(R.id.boatalignmentholder, mInclineBoatView);
        hideFromView(R.id.driftImg, mCompassView);
        hideFromView(R.id.pressureMeter, mPressureNeedleView);
        hideFromView(R.id.leftDrift, mLeftDriftView);
        hideFromView(R.id.rightDrift, mRightDriftView);
        LinearLayout ll = ((Activity) contx).findViewById(R.id.feedbackTextLayout);
        ll.removeView(feedbackText);
    }
    private void initInclineBoat(int drawable, float scaleX, float scaleY){
        mInclineBoatView = new RotatableGLView(contx,decodeSampledBitmapFromResource(contx.getResources(),
                drawable, 180, 180),
                scaleX, scaleY);
    }
    private void initPressure(int drawable, float scaleX, float scaleY){
        mPressureNeedleView = new RotatableGLView(contx,
                decodeSampledBitmapFromResource(contx.getResources(), drawable, 20, 20),
                scaleX, scaleY);
    }
    private void initCompass(int drawable, float scaleX, float scaleY){
        mCompassView = new RotatableGLView(contx,
                decodeSampledBitmapFromResource(contx.getResources(), drawable, 60,60),
                scaleX, scaleY);
    }
    private void initDrift(int drawable, int drawable2, float scaleX, float scaleY){
        mLeftDriftView = new RotatableGLView(contx,
                decodeSampledBitmapFromResource(contx.getResources(), drawable, 10, 10),
                scaleX, scaleY);
        mRightDriftView = new RotatableGLView(contx,
                decodeSampledBitmapFromResource(contx.getResources(), drawable2, 10, 10),
                scaleX, scaleY);
    }
    private void initWave(int drawable, int drawable2, float scaleX, float scaleY, float scaleX2, float scaleY2){
        mWaveView = new RotatableGLView(contx,
                decodeSampledBitmapFromResource(contx.getResources(), drawable, 140, 140), decodeSampledBitmapFromResource(contx.getResources(), drawable2, 100,100),
                scaleX, scaleY, scaleX2, scaleY2);
    }
    private void initDagger(int drawable, float scaleX, float scaleY){
        mDaggerView = new RotatableGLView(contx,
                decodeSampledBitmapFromResource(contx.getResources(), drawable, 120, 180),
                scaleX, scaleY);
    }
    private void initSpeed(int drawable, float scaleX, float scaleY){
        mSpeedView = new RotatableGLView(contx,
                decodeSampledBitmapFromResource(contx.getResources(), drawable, 120, 20),
                scaleX, scaleY);
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
    public RotatableGLView getmCompassView() {
        return mCompassView;
    }

    public RotatableGLView getmWaveView() {
        return mWaveView;
    }

    public RotatableGLView getmSpeedView() {
        return mSpeedView;
    }

    public RotatableGLView getmRightDriftView() {
        return mRightDriftView;
    }

    public RotatableGLView getmPressureNeedleView() {
        return mPressureNeedleView;
    }

    public RotatableGLView getmLeftDriftView() {
        return mLeftDriftView;
    }

    public RotatableGLView getmInclineBoatView() {
        return mInclineBoatView;
    }

    public RotatableGLView getmDaggerView() {
        return mDaggerView;
    }

    public ViewStates getmCurrentViewState() {
        return mCurrentViewState;
    }
}
