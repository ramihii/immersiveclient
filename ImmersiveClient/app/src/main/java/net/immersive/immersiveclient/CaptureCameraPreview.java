package net.immersive.immersiveclient;

import android.content.Context;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.os.Build;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;

public class CaptureCameraPreview extends SurfaceView implements SurfaceHolder.Callback, Camera.PreviewCallback {

    private static final String TAG = "CameraCapture";

    private Camera camera;
    private CameraEventListener listener;

    private int captureWidth;
    private int captureHeight;
    private int captureRate;

    private boolean usingBuffers = true;

    public CaptureCameraPreview(Context context, CameraEventListener listener) {
        super(context);

        if(listener == null)
            throw new IllegalArgumentException("CameraEventListener must not be null");

        this.listener = listener;

        SurfaceHolder holder = getHolder();
        holder.addCallback(this);
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    @Override
    public void onPreviewFrame(byte[] bytes, Camera camera) {
        if(usingBuffers)
            camera.addCallbackBuffer(bytes);

        listener.onCameraFrameReceived(bytes);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        int cameraIndex = 0; // TODO: remove hardcoded camera index
        try {
            //if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD)
                //camera = Camera.open(cameraIndex);
            //else
                camera = Camera.open();
        } catch(RuntimeException e) {
            Log.e(TAG, "Unable to open camera");
            return;
        }

        Log.i(TAG, "Opened camera");

        try {
            camera.setPreviewDisplay(holder);
        } catch(IOException e) {
            Log.e(TAG, "IOException while setting preview display");
            camera.release();
            camera = null;
            Log.i(TAG, "Released camera");
            return;
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int i, int i1, int i2) {
        if(camera == null) {
            Log.e(TAG, "No camera in surfaceChanged");
            return;
        }

        Camera.Parameters parameters = camera.getParameters();
        //parameters.setPreviewSize(800, 600); //TODO: no hardcoded preview size
        parameters.setPreviewFrameRate(30);
        camera.setParameters(parameters);

        parameters = camera.getParameters();
        captureWidth = parameters.getPreviewSize().width;
        captureHeight = parameters.getPreviewSize().height;
        Log.i(TAG, "width: " + captureWidth + ", height: " + captureHeight);
        captureRate = parameters.getPreviewFrameRate();
        int pixelFormat = parameters.getPreviewFormat();
        PixelFormat pixelInfo = new PixelFormat();
        PixelFormat.getPixelFormatInfo(pixelFormat, pixelInfo);
        Log.i(TAG, pixelInfo.toString());
        int cameraIndex = 0;
        boolean cameraIsFrontFacing = false;
        /*
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
            Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
            Camera.getCameraInfo(cameraIndex, cameraInfo);
            if(cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT)
                cameraIsFrontFacing = true;
        }
        */

        int bufSize = captureWidth * captureHeight * pixelInfo.bitsPerPixel / 8;

        Log.i(TAG, "bytesPerPixel: " + pixelInfo.bytesPerPixel + ", bitsPerPixel: " + pixelInfo.bitsPerPixel + ", bufSize: " + bufSize);

        configureCallback(this, true, 10, bufSize); // For the default NV21 format, bitsPerPixel = 12.

        camera.startPreview();

        listener.cameraCaptureStarted(captureWidth, captureHeight, 3);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if(camera != null) {
            camera.setPreviewCallback(null);
            camera.stopPreview();

            camera.release();
            camera = null;
        }

        listener.cameraCaptureStopped();
    }

    private void configureCallback(Camera.PreviewCallback cb, boolean useBuffersIfAvailable, int numBuffersIfAvailable, int bufferSize) {
        if(useBuffersIfAvailable) {
            camera.setPreviewCallbackWithBuffer(cb);

            for(int i = 0; i < numBuffersIfAvailable; i++)
                camera.addCallbackBuffer(new byte[bufferSize]);

            usingBuffers = true;
        } else {
            camera.setPreviewCallback(cb);

            usingBuffers = false;
        }
    }
}