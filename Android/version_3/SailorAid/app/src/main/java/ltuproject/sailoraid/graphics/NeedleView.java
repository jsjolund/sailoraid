package ltuproject.sailoraid.graphics;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.opengl.GLSurfaceView;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import ltuproject.sailoraid.R;

/**
 * Created by Henrik on 2017-09-06.
 */

public class NeedleView extends GLSurfaceView implements
        SurfaceHolder.Callback {

    private final Bitmap mBitmap;
    private final int mBitmapHeight, mBitmapWidth;
    private float mRotation;
    private final SurfaceHolder mSurfaceHolder;
    private final Paint mPainter = new Paint();
    private Thread mDrawingThread;
    private float x,y,z;
    private static final float ROT_STEP = 1.0f;


    public NeedleView(Context context, Bitmap bitmap) {
        super(context);

        mBitmapHeight = (int) getResources().getDimension(
                R.dimen.n_image_height);
        mBitmapWidth = (int) getResources().getDimension(
                R.dimen.n_image_width);
        this.mBitmap = Bitmap.createScaledBitmap(bitmap,
                mBitmapWidth, mBitmapHeight, false);
        mRotation = 1.0f;
        this.x = 0;

        mPainter.setAntiAlias(true);

        mSurfaceHolder = getHolder();
        mSurfaceHolder.addCallback(this);
        mSurfaceHolder.setFormat(PixelFormat.TRANSLUCENT);
    }

    private void drawNeedle(Canvas canvas) {
        super.draw(canvas);
        canvas.drawColor(0, android.graphics.PorterDuff.Mode.CLEAR);
        mRotation = ROT_STEP*x*10;
        canvas.rotate(mRotation, mBitmapWidth, 600+mBitmapHeight/2);
        canvas.drawBitmap(mBitmap, 0, 600, mPainter);
    }

    public void setPressure(float x){
        this.x = x;
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                               int height) {
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mDrawingThread = new Thread(new Runnable() {
            public void run() {
                Canvas canvas = null;
                while (!Thread.currentThread().isInterrupted()) {
                    canvas = mSurfaceHolder.lockCanvas();
                    if (null != canvas) {
                        drawNeedle(canvas);
                        mSurfaceHolder.unlockCanvasAndPost(canvas);
                    }
                }
            }
        });
        mDrawingThread.start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (null != mDrawingThread)
            mDrawingThread.interrupt();
    }

}
