package net.immersive.immersiveclient;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.content.res.AssetManager;

import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Rami Hiipakka on 14.2.2018.
 */

public class HelloGL extends GLSurfaceView {


    private static Context assetContext;
    private HelloGL.Renderer mRenderer;
    private static Immersive immersive;

    public HelloGL(Context context, Immersive immersive) {
        super(context);
        assetContext = context;
        this.immersive = immersive;
        mRenderer = new HelloGL.Renderer(this);
        setEGLContextClientVersion(2);
        setRenderer((Renderer)mRenderer);

        //Set to RENDERMODE_CONTINUOUSLY to call native code continuously
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        System.out.println("Rendermode: " + this.getRenderMode());

    }

    public static class Renderer implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

        private ByteBuffer pixelBuffer;
        private int bufferWidth;
        private int bufferHeight;
        private int bytesPerPixel;

        private GLSurfaceView view;
        private SurfaceTexture mSTexture;

        public Renderer(GLSurfaceView view) {
            this.view = view;

            bytesPerPixel = 3;

            Bitmap bm = BitmapFactory.decodeResource(view.getResources(), R.raw.test);

            int width = bufferWidth = bm.getWidth();
            int height = bufferHeight = bm.getHeight();

            int[] pixels = new int[width * height];
            bm.getPixels(pixels, 0, width, 0, 0, width, height);

            pixelBuffer = ByteBuffer.allocateDirect(bufferWidth * bufferHeight * bytesPerPixel);
            for(int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int pixel = pixels[y * width + x];

                    byte red = (byte)((pixel >> 16) & 255);
                    byte green = (byte)((pixel >> 8) & 255);
                    byte blue = (byte)(pixel & 255);

                    pixelBuffer.put(red);
                    pixelBuffer.put(green);
                    pixelBuffer.put(blue);
                }
            }

            pixelBuffer.flip();

        }

        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {

            mSTexture = new SurfaceTexture(-1);
            if(mSTexture == null) {
                Log.e("HelloGL", "Creating SurfaceTexture failed");
                return;
            }
            immersive.cppInit(assetContext.getAssets(),assetContext.getFilesDir().getAbsolutePath());
            mSTexture.setOnFrameAvailableListener(this);
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {

        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            Immersive.cppDraw(bufferWidth, bufferHeight, Immersive.PIX_FMT_RGB, bytesPerPixel, pixelBuffer);
        }

        @Override
        public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        }
    }
}
