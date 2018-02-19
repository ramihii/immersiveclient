package net.immersive.immersiveclient;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Rami Hiipakka on 14.2.2018.
 */

public class HelloGL extends GLSurfaceView {

    static {
        System.loadLibrary("immersive");
    }

    private HelloGL.Renderer mRenderer;

    public HelloGL(Context context) {
        super(context);
        mRenderer = new HelloGL.Renderer(this);
        setEGLContextClientVersion(2);
        setRenderer((Renderer)mRenderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public static class Renderer implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

        private GLSurfaceView view;
        private SurfaceTexture mSTexture;

        public Renderer(GLSurfaceView view) {
            this.view = view;
        }

        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            mSTexture = new SurfaceTexture(-1);
            if(mSTexture == null) {
                Log.e("HelloGL", "Creating SurfaceTexture failed");
                return;
            }
            mSTexture.setOnFrameAvailableListener(this);
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {

        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            Immersive.cppDraw();
        }

        @Override
        public void onFrameAvailable(SurfaceTexture surfaceTexture) {

        }
    }
}
