package org.apertusvr.app;

import org.apertusvr.ApertusJNI;
import org.apertusvr.apeEvent;
import org.apertusvr.apeEventCallback;
import org.apertusvr.apeEventManager;
import org.apertusvr.apeFileGeometry;
import org.apertusvr.apeGeometry;
import org.apertusvr.apeLight;
import org.apertusvr.apeNode;
import org.apertusvr.apeSceneManager;
import org.apertusvr.apeVector3;
import org.jetbrains.annotations.NotNull;

import org.apertusvr.render.apeFilamentRenderPlugin;


import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Choreographer;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

//    private static final String TAG = "Logging example";

    ApertusJNI apeJNI = new ApertusJNI();
    Choreographer choreographer;
    private FrameCallback frameCallback = new FrameCallback();
    boolean apeStarted = false;

    EventCallback eventCallback = new EventCallback();

    /* rendering */
    private apeFilamentRenderPlugin renderPlugin;
    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        choreographer = Choreographer.getInstance();
        surfaceView = new SurfaceView(this);
        setContentView(surfaceView);

        if (!apeStarted) {
            apeJNI.startApertusVR(getAssets());
            apeStarted = true;
        }

        String renderResources = getFilesDir() + "/models";
        renderPlugin = new apeFilamentRenderPlugin(
                this, getLifecycle(), surfaceView, renderResources, getResources());
        getLifecycle().addObserver(renderPlugin);

        apeEventManager.connectEvent(apeEvent.Group.LIGHT, eventCallback);
        apeEventManager.connectEvent(apeEvent.Group.NODE, eventCallback);
        apeEventManager.connectEvent(apeEvent.Group.GEOMETRY_FILE,eventCallback);
    }

    @Override
    protected void onResume() {
        super.onResume();
        choreographer.postFrameCallback(frameCallback);
    }

    @Override
    protected void onPause() {
        super.onPause();
        choreographer.removeFrameCallback(frameCallback);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        choreographer.removeFrameCallback(frameCallback);
        if(apeStarted) {
            apeJNI.stopApertusVR();
            apeStarted = false;
        }
    }


    class FrameCallback implements Choreographer.FrameCallback {

//        private long lastFrameTimeNanos = System.nanoTime();

        @Override
        public void doFrame(long frameTimeNanos) {
//            long deltaTime = frameTimeNanos - lastFrameTimeNanos;
//            lastFrameTimeNanos = frameTimeNanos;
            choreographer.postFrameCallback(this);

            if(apeStarted) {
                ApertusJNI.processEventDoubleQueue();
            }
        }
    }

    class EventCallback implements apeEventCallback {

        public EventCallback() {}

        @Override
        public void onEvent(@NotNull apeEvent input) {

            Log.d("javalog",input.type.toString() + " event, with subject name: " + input.subjectName);

//            if(input.type == apeEvent.Type.NODE_POSITION) {
//                apeNode node = new apeNode(input.subjectName);
//
//                Log.d("javalog","Related node geometries:");
//                if(node.isValid()) {
//                    apeGeometry[] relatedGeoms = node.getRelatedGeometries();
//                    for (apeGeometry geom : relatedGeoms) {
//                        Log.d("javalog",geom.getName());
//                    }
//                }
//            }
        }
    }
}
