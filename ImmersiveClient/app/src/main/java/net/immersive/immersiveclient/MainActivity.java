package net.immersive.immersiveclient;

import android.content.res.AssetManager;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageButton;

public class MainActivity extends AppCompatActivity implements CameraEventListener {

    private Immersive immersive;
    //private Camera mCamera = null;
    //private CameraView mCameraView = null;
    private CaptureCameraPreview camcap = null;
    private HelloGL helloGL = null;

    static {
        System.loadLibrary("immersive");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().setFormat(PixelFormat.TRANSLUCENT);

        Immersive immersive = new Immersive();
        setContentView(R.layout.activity_main);

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
    }

    @Override
    public void cameraCaptureStarted(int captureWidth, int captureHeight, int bytesPerPixel) {
        helloGL.initBuffer(captureWidth, captureHeight, bytesPerPixel);
    }

    @Override
    public void onCameraFrameReceived(byte[] data) {
        helloGL.receiveFrame(data);
        helloGL.requestRender();
    }

    @Override
    public void cameraCaptureStopped() {

    }
}
