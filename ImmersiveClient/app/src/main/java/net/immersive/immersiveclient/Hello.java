package net.immersive.immersiveclient;

/**
 * Created by Rami Hiipakka on 13.2.2018.
 */

public class Hello {
    static {
        System.loadLibrary("immersive");
    }

    @SuppressWarnings("unused")
    public int someInt() {
        return 123;
    }

    public native int addInts(int a, int b);
    public native int mulInts(int a, int b);
    public native String helloStr();
}
