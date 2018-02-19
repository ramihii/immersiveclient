package net.immersive.immersiveclient;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    /*
    private GLSurfaceView mSurfaceView;
    private GLSurfaceView mGLView;
    */

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        /*
        if(hasGLES20()) {
            mGLView = new GLSurfaceView(this);
            mGLView.setEGLContextClientVersion(2);
            mGLView.setPreserveEGLContextOnPause(true);
            mGLView.setRenderer(null);
        } else {
        }*/

        //setContentView(mGLView);

        Hello hello = new Hello();
        int a = hello.addInts(5, 15);
        int b = hello.mulInts(5, 15);
        String ss = hello.helloStr();

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(String.format(Locale.US,"%s a: %d b: %d s: %s", stringFromJNI(), a, b, ss));
    }

    /*
    private boolean hasGLES20() {
        ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return info.reqGlEsVersion >= 0x20000;
    }

    @Override
    protected void onResume() {
        super.onResume();

        if(mSurfaceView != null)
            mSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();

        if(mSurfaceView != null)
            mSurfaceView.onPause();
    }
    */

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
