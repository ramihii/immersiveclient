package net.immersive.immersiveclient;

import java.nio.ByteBuffer;
import android.content.res.AssetManager;

/**
 * Created by doom on 19.2.2018.
 */

public class Immersive {
    public static final int PIX_FMT_OTHER = 0;
    public static final int PIX_FMT_RGB = 1;

	/** NOTE: this method is NOT safe to use with multiple instances of this class.
	 *  Underlying native code assumes only one instance of this class exists.*/
    public native void cppInit(AssetManager assetManager, String filePath);

    public static native void cppDraw(int bufferWidth, int bufferHeight, int format, int bytesPerPixel, ByteBuffer buffer);
}
