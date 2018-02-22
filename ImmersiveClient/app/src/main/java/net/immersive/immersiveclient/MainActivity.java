package net.immersive.immersiveclient;

import android.content.res.AssetManager;
import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageButton;

public class MainActivity extends AppCompatActivity {

    private Immersive immersive;
    private Camera mCamera = null;
    private CameraView mCameraView = null;
    private HelloGL helloGL = null;

    static {
        System.loadLibrary("immersive");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Immersive immersive = new Immersive();
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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

        immersive.cppInit(this.getAssets(),this.getFilesDir().getAbsolutePath());

        helloGL = new HelloGL(this);
        FrameLayout hello_view = (FrameLayout) findViewById(R.id.hellogl);
        hello_view.addView(helloGL);

        ImageButton imgClose = (ImageButton)findViewById(R.id.imgClose);
        imgClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                System.exit(0);
            }
        });


    }
}
