package net.immersive.immersiveclient;

import java.nio.ByteBuffer;

/**
 * Created by doom on 19.2.2018.
 */

public class Immersive {
    public static final int PIX_FMT_OTHER = 0;
    public static final int PIX_FMT_RGB = 1;

    public static native void cppInit();
    public static native void cppDraw(int bufferWidth, int bufferHeight, int format, int bytesPerPixel, ByteBuffer buffer);
}
