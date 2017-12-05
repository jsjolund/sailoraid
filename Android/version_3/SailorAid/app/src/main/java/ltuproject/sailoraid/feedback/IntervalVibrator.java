package ltuproject.sailoraid.feedback;

import android.content.Context;
import android.os.Handler;
import android.os.Vibrator;

/**
 * Created by Henrik on 2017-12-04.
 */


public class IntervalVibrator implements Runnable{
    private Vibrator vibrateFeedback;
    private int times, interval;
    private long length;
    private int start;
    private Context conxt;
    private Handler mVibratorHandler;
    public IntervalVibrator(Context contx, int times, long length, int interval){
        this.conxt = contx;
        this.vibrateFeedback = (Vibrator) conxt.getSystemService(contx.getApplicationContext().VIBRATOR_SERVICE);
        this.times = times;
        this.length = length;
        this.interval = interval;
        this.start = 1;
        mVibratorHandler = new Handler();
    }
    @Override
    public void run() {
        try {
            this.vibrateFeedback.vibrate(length);
        } finally {
            // 100% guarantee that this always happens, even if
            // your update method throws an exception
            if (start <times){
                mVibratorHandler.postDelayed(this, interval);
                start++;
            }
        }
    }
}