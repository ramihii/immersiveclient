package net.immersive.immersiveclient;

import android.content.res.AssetManager;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.view.View.OnTouchListener;
import android.view.WindowManager;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;

import org.opencv.core.Core;
import org.opencv.core.Mat;

public class MainActivity extends AppCompatActivity implements CvCameraViewListener2, OnTouchListener {

    //private Immersive immersive;
    //private Camera mCamera = null;
    //private CameraView mCameraView = null;
    //private CaptureCameraPreview camcap = null;
    //private HelloGL helloGL = null;

    private static final String TAG = "Immersive";

    static {
        System.loadLibrary("aruco_java");
        //System.loadLibrary("acuro");
        System.loadLibrary("opencv_java3");
    }

    private CameraBridgeViewBase mOpenCvCameraView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.activity_main);

        Log.i("OpenCV", Core.getBuildInformation());

        mOpenCvCameraView = findViewById(R.id.camera_view);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.setOnTouchListener(this);
        mOpenCvCameraView.enableView();

        /*
        getWindow().setFormat(PixelFormat.TRANSLUCENT);

        Immersive immersive = new Immersive();
        setContentView(R.layout.activity_main);
        */

        /*
        try {
            mCamera = Camera.open();
        } catch(Exception e) {
            Log.d("ERROR", "Failed to get camera: " + e.getMessage());
        }

        if(mCamera != null) {
            mCameraView = new CameraView(this, mCamera);
            FrameLayout camera_view = (FrameLayout)findViewById(R.id.camera_view);
            camera_view.addView(mCameraView);
        }
        */

        /*
        helloGL = new HelloGL(this, immersive);
        FrameLayout hello_view = (FrameLayout) findViewById(R.id.hellogl);
        hello_view.addView(helloGL);

        camcap = new CaptureCameraPreview(this, this);
        FrameLayout camera_view = (FrameLayout)findViewById(R.id.camera_view);
        camera_view.addView(camcap);

        ImageButton imgClose = (ImageButton)findViewById(R.id.imgClose);
        imgClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                System.exit(0);
            }
        });
        */
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mOpenCvCameraView.disableView();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        //mGray = new Mat();
        //mRgba = new Mat();
    }

    @Override
    public void onCameraViewStopped() {
        //mGray.release();
        //mRgba.release();
    }

    @Override
    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        Mat mRgba = inputFrame.rgba();
        //Mat mGray = inputFrame.gray();

        // Return the image matrix we want to display
        return mRgba;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        Log.d(TAG, "onTouch invoked");

        return false;
    }

    //@Override
    //public void cameraCaptureStarted(int captureWidth, int captureHeight, int bytesPerPixel) {
    //    helloGL.initBuffer(captureWidth, captureHeight, bytesPerPixel);
    //}

    //@Override
    //public void onCameraFrameReceived(byte[] data) {
    //    helloGL.receiveFrame(data);
    //    helloGL.requestRender();
    //}

    //@Override
    //public void cameraCaptureStopped() {

    //}
}
