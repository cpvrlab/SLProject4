package ch.bfh.cpvrlab;

import android.content.Context;

public class AppAndroidJNI {

    static {
        System.loadLibrary(BuildConfig.APP_LIBRARY_NAME);
    }

    public static final int VIDEO_TYPE_NONE = 0;    // No video at all is used
    public static final int VIDEO_TYPE_MAIN = 1;    // Maps to Androids back facing camera
    public static final int VIDEO_TYPE_SCND = 2;    // Maps to Androids front facing camera
    public static final int VIDEO_TYPE_FILE = 3;    // Maps to Androids front facing camera

    public static native void onInit(int width, int height, int dotsPerInch, String FilePath);

    public static native boolean onUpdate();

    public static native boolean onPaintAllViews();

    public static native void onResize(int width, int height);

    public static native void onMouseDown(int button, int x, int y);

    public static native void onMouseUp(int button, int x, int y);

    public static native void onMouseMove(int x, int y);

    public static native void onTouch2Down(int x1, int y1, int x2, int y2);

    public static native void onTouch2Move(int x1, int y1, int x2, int y2);

    public static native void onTouch2Up(int x1, int y1, int x2, int y2);

    public static native void onDoubleClick(int button, int x, int y);

    public static native void onRotationQUAT(float quatX, float quatY, float quatZ, float quatW);

    public static native void onClose();

    public static native boolean usesRotation();

    public static native boolean usesLocation();

    public static native void onLocationLatLonAlt(double latitudeDEG, double longitudeDEG, double altitudeM, float accuracyM);

    public static native int getVideoType();

    public static native int getVideoSizeIndex();

    public static native void grabVideoFileFrame();

    public static native void copyVideoImage(int imgWidth, int imgHeight, byte[] imgBuffer);

    public static native void copyVideoYUVPlanes(int srcW, int srcH,
                                                 byte[] y, int ySize, int yPixStride, int yLineStride,
                                                 byte[] u, int uSize, int uPixStride, int uLineStride,
                                                 byte[] v, int vSize, int vPixStride, int vLineStride);

    public static native void onSetupExternalDir(String externalDirPath);

    public static native void setCameraSize(int sizeIndex, int sizeIndexMax, int width, int height);

    public static native void setDeviceParameter(String parameter, String value);

    public static native void initMediaPipeAssetManager(Context androidContext, String cacheDirPath);

}
