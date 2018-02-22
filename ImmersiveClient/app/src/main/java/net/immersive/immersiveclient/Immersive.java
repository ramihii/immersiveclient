package net.immersive.immersiveclient;

import android.content.res.AssetManager;

/**
 * Created by doom on 2/19/18.
 */

public class Immersive {

    public native void cppInit(AssetManager assetManager, String filePath);
    public static native void cppDraw();

}
