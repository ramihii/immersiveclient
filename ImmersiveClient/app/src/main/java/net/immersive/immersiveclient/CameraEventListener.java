package net.immersive.immersiveclient;

/**
 * Created by Rami Hiipakka on 6.3.2018.
 */

public interface CameraEventListener {
    void cameraCaptureStarted(int captureWidth, int captureHeight, int bytesPerPixel);

    void onCameraFrameReceived(byte[] data);

    void cameraCaptureStopped();
}
