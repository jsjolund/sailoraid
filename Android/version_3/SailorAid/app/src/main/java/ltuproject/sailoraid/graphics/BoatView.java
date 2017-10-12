package ltuproject.sailoraid.graphics;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import ltuproject.sailoraid.R;

/**
 * Class that draws a boat that can change alignment depending on values received
 */
public class BoatView extends GLSurfaceView implements
        SurfaceHolder.Callback {
   // private final Bitmap mBitmap;
    //private final int mBitmapHeightAndWidth, mBitmapHeightAndWidthAdj;
    private float mX, mY, mRotation;
    //private final SurfaceHolder mSurfaceHolder;
    private final Paint mPainter = new Paint();
    private Thread mDrawingThread;
    private float x,y,z;
    private static final float ROT_STEP = 1.0f;
    private GLSurfaceView glBoat;
    public BoatView(Context context) {
        super(context);

        glBoat = new GLSurfaceView(context);

        // Creating and attaching the renderer.
        OpenGLRenderer renderer = new OpenGLRenderer();
        glBoat.setRenderer(renderer);

        // Create a new plane.
        SimplePlane plane = new SimplePlane(1, 1);

        // Move and rotate the plane.
        plane.z = 1.7f;
        plane.rx = -65;

        // Load the texture.
        plane.loadBitmap(BitmapFactory.decodeResource(getResources(),
                R.drawable.boat_alignement));

        // Add the plane to the renderer.
        renderer.addMesh(plane);
        /*mBitmapHeightAndWidth = (int) getResources().getDimension(
               R.dimen.image_height);
        this.mBitmap = Bitmap.createScaledBitmap(bitmap,
                mBitmapHeightAndWidth, mBitmapHeightAndWidth, false);
                */
        //mBitmapHeightAndWidthAdj = mBitmapHeightAndWidth / 2;

    }

    public GLSurfaceView getGlBoat(){
        return glBoat;
    }

    private void drawBoat(Canvas canvas) {
        super.draw(canvas);
        /*canvas.drawColor(0, android.graphics.PorterDuff.Mode.CLEAR);
        mRotation = ROT_STEP*x;
        canvas.rotate(mRotation, mBitmapHeightAndWidthAdj+50, mBitmapHeightAndWidth);
        this.draw(canvas);*/
    }

    public void setXYZ(float x, float y, float z){
        this.x = x;
        this.y = y;
        this.z = z;
        //drawBoat(mSurfaceHolder.lockCanvas());
    }

   /* @Override
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
*/
  /*  @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (null != mDrawingThread)
            mDrawingThread.interrupt();
    }
*/

}
