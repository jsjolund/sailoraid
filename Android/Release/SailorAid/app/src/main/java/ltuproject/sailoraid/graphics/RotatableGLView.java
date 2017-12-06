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
public class RotatableGLView extends GLSurfaceView {
    // private final Bitmap mBitmap;
    //private final int mBitmapHeightAndWidth, mBitmapHeightAndWidthAdj;
    private float mX, mY, mRotation;
    //private final SurfaceHolder mSurfaceHolder;
    private final Paint mPainter = new Paint();
    private Thread mDrawingThread;
    private float x,y,z;
    private static final float ROT_STEP = 1.0f;
    private GLSurfaceView glView;
    private Bitmap mBitmap;
    private SimplePlane plane;
    private SimplePlane plane2;
    private OpenGLRenderer renderer;
    public RotatableGLView(Context context, Bitmap bitmap, float width, float height) {
        super(context);
        glView = new GLSurfaceView(context);
        // Creating and attaching the renderer.
        renderer = new OpenGLRenderer();

        glView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        glView.setRenderer(renderer);
        glView.setRenderMode(RENDERMODE_WHEN_DIRTY);
        glView.getHolder().setFormat(PixelFormat.RGBA_8888);
        glView.setZOrderOnTop(true);
        // Create a new plane.
        //setZOrderOnTop(true);
        plane = new SimplePlane(width, height);
        // Move and rotate the plane.
        //plane.z = 1.7f;
        //plane.rx = -65;
        // Load the texture.
        plane.loadBitmap(bitmap);   
        // Add the plane to the renderer.
        renderer.addMesh(plane);
    }
    public RotatableGLView(Context context, Bitmap bitmap, Bitmap bitmap2, float width, float height, float width2, float height2) {
        super(context);
        glView = new GLSurfaceView(context);
        // Creating and attaching the renderer.

        renderer = new OpenGLRenderer();

        glView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        glView.setRenderer(renderer);
        glView.setRenderMode(RENDERMODE_WHEN_DIRTY);
        glView.getHolder().setFormat(PixelFormat.RGBA_8888);
        glView.setZOrderOnTop(true);
        // Create a new plane.
        //setZOrderOnTop(true);
        plane = new SimplePlane(width, height);

        plane2 = new SimplePlane(width2, height2);
        // Move and rotate the plane.
        //plane.z = 1.7f;
        //plane.rx = -65;
        // Load the texture.
        plane.loadBitmap(bitmap);
        plane2.loadBitmap(bitmap2);
        // Add the plane to the renderer.
        renderer.addMesh(plane);
        renderer.addMesh(plane2);
    }

    /**
     * Rotate plane by r degrees. FEATURE also rotates all other planes added on the same renderer
     * @param r
     */
    public void rotateGl(float r){
        plane.rz = r;
        if (plane2 != null){
            plane2.rz = -r;
        }
    }

    /**
     * Rotate plane 2 by r degrees.
     * @param r
     */
    public void rotateGl2(float r){
        if (plane2 != null){
            plane2.rz= r;
        }

    }

    public void moveGL(float x, float y){
        plane.x = x;
        plane.y = y;
    }

    public void moveGL2(float x, float y){
        plane2.x = x;
        plane2.y = y;
    }

    public void resizeGL(float scaleX, float scaleY){
        plane.setScale(scaleX, scaleY);
    }
    public void moveGLZ(float z){
        plane.z = z;
    }
    public GLSurfaceView getGlView(){
        return glView;
    }

    public void clearRenderer(){
        renderer.removeMesh();
    }

    public Bitmap getmBitmap(){
        return mBitmap;
    }
    public void clearBitmap() {
        plane.clearBitmap();
    }
}
