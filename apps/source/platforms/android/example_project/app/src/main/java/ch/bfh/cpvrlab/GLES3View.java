/**
 * \file      GLES3View.java
 * \date      Spring 2017
 * \brief   Android Java toplevel windows interface into the SLProject demo
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
 * \authors   Marcus Hudritsch, Zingg Pascal
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

// Please do not change the name space. The SLProject app is identified in the app-store with it.
package ch.bfh.cpvrlab;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLES3View extends GLSurfaceView {
    private static final String TAG = "SLProject";
    private static final int VT_NONE = 0;
    private static final int VT_MAIN = 1;
    private static final int VT_SCND = 2;
    private static final int VT_FILE = 3;

    public GLES3View(Context context) {
        super(context);

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        //More detailed: Configure context with ConfigChooser class
        //setEGLConfigChooser(new ConfigChooser(8, 8, 8, 0, 16, 0));

        setEGLContextClientVersion(3);
        //More detailed: Create context with ContextFactory class
        //setEGLContextFactory(new ContextFactory());

        // Set the renderer responsible for frame rendering
        setRenderer(new Renderer());

        // From Android r15
        setPreserveEGLContextOnPause(true);

        // Render only when needed. Without this it would render continuously with lots of power consumption
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    /**
     * The renderer implements the major callback for the OpenGL ES rendering:
     * - onSurfaceCreated calls SLProjects onInit
     * - onSurfaceChanged calls SLProjects onResize
     * - onDrawFrame      calls SLProjects onUpdateAndPaint
     * Be aware that the renderer runs in a separate thread. Calling something in the
     * activity cross thread invocations.
     */
    private static class Renderer implements GLSurfaceView.Renderer {
        private Handler mainLoop;

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            Log.i(TAG, "Renderer.onSurfaceCreated");
            int w = GLES3Lib.view.getWidth();
            int h = GLES3Lib.view.getHeight();
            AppAndroidJNI.onInit(w, h,
                    GLES3Lib.dpi,
                    GLES3Lib.App.getApplicationContext().getFilesDir().getAbsolutePath());

            // Get main event handler of UI thread
            mainLoop = new Handler(Looper.getMainLooper());
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Log.i(TAG, "Renderer.onSurfaceChanged");
            AppAndroidJNI.onResize(width, height);
            GLES3Lib.view.requestRender();
        }

        public void onDrawFrame(GL10 gl) {
            int videoType = AppAndroidJNI.getVideoType();
            int sizeIndex = AppAndroidJNI.getVideoSizeIndex();
            boolean usesRotation = AppAndroidJNI.usesRotation();
            boolean usesLocation = AppAndroidJNI.usesLocation();

            if (videoType == VT_MAIN || videoType == VT_SCND)
                mainLoop.post(() -> GLES3Lib.activity.cameraStart(videoType, sizeIndex));
            else mainLoop.post(() -> GLES3Lib.activity.cameraStop());

            if (usesRotation)
                mainLoop.post(() -> GLES3Lib.activity.rotationSensorStart());
            else mainLoop.post(() -> GLES3Lib.activity.rotationSensorStop());

            if (usesLocation)
                mainLoop.post(() -> GLES3Lib.activity.locationSensorStart());
            else mainLoop.post(() -> GLES3Lib.activity.locationSensorStop());

            if (videoType == VT_FILE)
                AppAndroidJNI.grabVideoFileFrame();

            //////////////////////////////////////////////////////
            boolean doRepaint = AppAndroidJNI.onUpdate();
            //////////////////////////////////////////////////////

            // Only request new rendering for non-live video
            // For live video the camera service will call requestRenderer
            if (doRepaint && (videoType == VT_NONE || videoType == VT_FILE))
                GLES3Lib.view.requestRender();

            if (videoType != VT_NONE)
                GLES3Lib.lastVideoImageIsConsumed.set(true);
        }
    }
}
