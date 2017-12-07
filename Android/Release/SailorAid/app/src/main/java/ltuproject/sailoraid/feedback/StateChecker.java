package ltuproject.sailoraid.feedback;


import android.content.Context;
import android.os.Handler;
import android.speech.tts.TextToSpeech;
import android.speech.tts.UtteranceProgressListener;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;

import ltuproject.sailoraid.R;

import static java.lang.Math.abs;

/**
 * Created by Henrik on 2017-12-05.
 */

public class StateChecker implements Runnable{
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

    private float inclineX, inclineY, bearingZ;
    private float direction, speed;
    private double drift;
    private float pressure, leftPressure, rightPressure, maxPressure;
    private float range;
    private float batteryPower;
    private float wavePeriod;
    private boolean isWaving = false;
    private float wavePos = 0;
    private List<Float> sogData = new ArrayList<>();
    private List<Float> waveData = new ArrayList<Float>();

    HashMap<String, String> map = new HashMap<String, String>();

    private boolean isSpeaking = false;
    private int startFlag = 0;

    private Context contx;
    private TextToSpeech txtToSpeech;
    private Handler mFeedbackHandler;
    private int mIntervel = 500;
    private boolean enableVoice = false;
    private boolean enableVibration = false;

    TextView feedbackText;
    public StateChecker(Context contx) {
        this.contx = contx;
    }

    public StateChecker(Context contx, TextView fbt){
        this.contx = contx;
        mFeedbackHandler = new Handler();
        feedbackText = fbt;
        txtToSpeech=new TextToSpeech(this.contx, new TextToSpeech.OnInitListener() {
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
    }

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
                } else if (abs(inclineX) > HEELINCLINELOWERLIMIT && range > RANGEMIDLIMIT && pressure < PRESSURELOWLIMIT && drift < DRIFTLOWERLIMIT){
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
            // run this at a defined interval
            mFeedbackHandler.postDelayed(this, mIntervel);
        }
    }

    /*
     Based on current feedbackstate start a speaker and vibrator thread.
    */
    private void talkFeedback(){
        IntervalVibrator mVibrator =  new IntervalVibrator(contx,0, 0, 0);
        String txt = "";
        switch (mFeedbackState) {
            case HEEL:
                txt = "All hands! \n Abandon ship!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.red));
                feedbackText.setBackground(contx.getDrawable(R.color.black));
                mVibrator = new IntervalVibrator(contx, 1, 1000, 500);
                break;
            case WRSPEED:
                txt = "Yargh matey! \n New world record speed!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.green));
                feedbackText.setBackground(contx.getDrawable(R.color.darkblue));
                mVibrator = new IntervalVibrator(contx, 10, 1000, 300);
                break;
            case LRSPEED:
                txt = "Ahoy land crab! \n New laser record speed!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.darkgreen));
                feedbackText.setBackground(contx.getDrawable(R.color.laserblue));
                mVibrator = new IntervalVibrator(contx, 5, 200, 400);
                break;
            case REEFING:
                txt = "Furl the jib \n Lower the mainsail";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.orange));
                feedbackText.setBackground(contx.getDrawable(R.color.blue));
                mVibrator = new IntervalVibrator(contx, 3, 200, 700);
                break;
            case HIKE:
                txt = "Hike more! \n You can do it!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.darkblue));
                feedbackText.setBackground(contx.getDrawable(R.color.green));
                mVibrator = new IntervalVibrator(contx, 3, 200, 700);
                break;
            case KEELHAUL:
                txt = "Lower the centerboard";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.darkblue));
                feedbackText.setBackground(contx.getDrawable(R.color.darkgreen));
                mVibrator = new IntervalVibrator(contx, 3, 200, 700);
                break;
            case HAULING:
                txt = "Ahoy! \n Lift Centerboard";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.blue));
                feedbackText.setBackground(contx.getDrawable(R.color.yellow));
                mVibrator = new IntervalVibrator(contx, 2, 1200, 800);
                break;
            case RUNNINGLOW:
                txt = "Lower the centerboard! \n Death roll imminent!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.red));
                feedbackText.setBackground(contx.getDrawable(R.color.blue));
                mVibrator = new IntervalVibrator(contx, 2, 200, 600);
                break;
            case RUNNINGHIGH:
                txt = "Dead ahead! \n Full speed!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.red));
                feedbackText.setBackground(contx.getDrawable(R.color.green));
                mVibrator = new IntervalVibrator(contx, 2, 200, 600);
                break;
            case DRIFT:
                txt = "Ship Adrift! \n Lower Centerboard more!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.orange));
                feedbackText.setBackground(contx.getDrawable(R.color.black));
                mVibrator = new IntervalVibrator(contx, 2, 100, 500);
                break;
            case CLEAR:
                txt = "Your an able seaman \n Congrats!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.darkgreen));
                feedbackText.setBackground(contx.getDrawable(R.color.orange));
                mVibrator = new IntervalVibrator(contx, 3, 200, 500);
                break;
            case LANDCRAB:
                txt = "Speed up land crab!";
                feedbackText.setText(txt);
                feedbackText.setTextColor(contx.getColor(R.color.darkgreen));
                feedbackText.setBackground(contx.getDrawable(R.color.orange));
                mVibrator = new IntervalVibrator(contx, 3, 200, 500);
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
    public void stop(){
        mFeedbackHandler.removeCallbacks(this);
        if(txtToSpeech != null) {
            txtToSpeech.stop();
            txtToSpeech.shutdown();
        }
    }

    private void filterSpeed(float speed){
        sogData.add(speed);
        if(sogData.size() >= 4){
            this.speed = movingAverageFilter(sogData, 3);
            sogData.remove(0);
            sogData.remove(sogData.size()-1);
            sogData.add(speed);
        }
    }

    public float calculateWaveFrequency(List<Float> data) {
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

    public float movingAverageFilter(List<Float> data, int filterLength){
        float sum = 0;
        for (Float value : data){
            sum+=value;
        }
        return sum/data.size();
    }

    public void setBatteryPower(float batteryPower) {
        this.batteryPower = batteryPower;
    }

    public void setBearingZ(float bearingZ) {
        this.bearingZ = bearingZ;
    }

    public void setDirection(float direction) {
        this.direction = direction;
    }

    public void setDrift(double drift) {
        this.drift = drift;
    }

    public void setEnableVoice(boolean enableVoice) {
        this.enableVoice = enableVoice;
    }

    public void setEnableVibration(boolean enableVibration) {
        this.enableVibration = enableVibration;
    }

    public void setInclineX(float inclineX) {
        this.inclineX = inclineX;
    }

    public void setInclineY(float inclineY) {
        this.inclineY = inclineY;
    }

    public void setMaxPressure(float maxPressure) {
        this.maxPressure = maxPressure;
    }

    public void setRange(float range) {
        this.range = range;
    }

    public void setSpeed(float speed) {
        filterSpeed(speed);
    }

    public void setFeedbackText(TextView feedbackText) {
        this.feedbackText = feedbackText;
    }

    public void setWavePeriod(float wavePeriod) {
        waveData.add(wavePeriod);
        if (waveData.size() > 500) {
            this.wavePeriod = calculateWaveFrequency(waveData);
            for (int i = 0; i < 25; i++) {
                waveData.remove(0);
            }
            this.wavePeriod = calculateWaveFrequency(waveData);
        }
    }

    public void setWaving(boolean waving) {
        isWaving = waving;
    }

    public float getBatteryPower() {
        return batteryPower;
    }

    public float getInclineX() {
        return inclineX;
    }

    public double getDrift() {
        return drift;
    }

    public float getBearingZ() {
        return bearingZ;
    }

    public float getInclineY() {
        return inclineY;
    }

    public float getMaxPressure() {
        return maxPressure;
    }

    public float getRange() {
        return range;
    }

    public float getSpeed() {
        return speed;
    }

    public float getWavePeriod() {
        return wavePeriod;
    }

}
