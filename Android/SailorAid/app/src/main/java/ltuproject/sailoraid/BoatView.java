package ltuproject.sailoraid;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Class that draws a boat that can change alignment depending on values received
 */
public class BoatView extends SurfaceView implements
        SurfaceHolder.Callback {

    private final Bitmap mBitmap;
    private final int mBitmapHeightAndWidth, mBitmapHeightAndWidthAdj;
    private float mX, mY, mRotation;
    private final SurfaceHolder mSurfaceHolder;
    private final Paint mPainter = new Paint();
    private Thread mDrawingThread;
    private float x,y,z;
    private static final float ROT_STEP = 1.0f;


    public BoatView(Context context, Bitmap bitmap) {
        super(context);
        mBitmapHeightAndWidth = (int) getResources().getDimension(
                R.dimen.image_height);
        this.mBitmap = Bitmap.createScaledBitmap(bitmap,
                mBitmapHeightAndWidth, mBitmapHeightAndWidth, false);
        mBitmapHeightAndWidthAdj = mBitmapHeightAndWidth / 2;
        mRotation = 1.0f;
        mPainter.setAntiAlias(true);

        mSurfaceHolder = getHolder();
        mSurfaceHolder.addCallback(this);
        mSurfaceHolder.setFormat(PixelFormat.TRANSLUCENT);
    }

    private void drawBoat(Canvas canvas) {
        super.draw(canvas);
        canvas.drawColor(0, android.graphics.PorterDuff.Mode.CLEAR);
        mRotation = ROT_STEP*x;
        canvas.rotate(mRotation, mBitmapHeightAndWidthAdj+50, mBitmapHeightAndWidth);
        canvas.drawBitmap(mBitmap, 100, 0, mPainter);
    }

    public void setXYZ(float x, float y, float z){
        this.x = x;
        this.y = y;
        this.z = z;
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
                        drawBoat(canvas);
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
