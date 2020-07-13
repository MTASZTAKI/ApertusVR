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


import android.annotation.SuppressLint;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Choreographer;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Objects;

public class MainActivity extends AppCompatActivity {

//    private static final String TAG = "Logging example";

    ApertusJNI apeJNI = new ApertusJNI();
    Choreographer choreographer;
    private FrameCallback frameCallback = new FrameCallback();
    boolean apeStarted = false;


    /* rendering */
    private apeFilamentRenderPlugin renderPlugin;
    private SurfaceView surfaceView;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Objects.requireNonNull(getSupportActionBar()).hide();

        setContentView(R.layout.activity_main);
        choreographer = Choreographer.getInstance();
        surfaceView = new SurfaceView(this);

        setContentView(surfaceView);

        surfaceView.setOnTouchListener(new myOnTouchListener());

        Log.d("javalog",getFilesDir().toString());
        if (!apeStarted) {
            apeJNI.startApertusVR("sampleScene",getAssets());
            apeStarted = true;
        }

        initRenderPlugin();
    }

    private void initRenderPlugin() {
        String renderResources = getFilesDir() + "/models";
        String resourcePrefix = "../../samples/virtualLearningFactory/models/";
        renderPlugin = new apeFilamentRenderPlugin(
                this, getLifecycle(), surfaceView,
                resourcePrefix,renderResources, getResources());
                getLifecycle().addObserver(renderPlugin);
    }

    @Override
    protected void onStart() {
        super.onStart();
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
        @Override
        public void doFrame(long frameTimeNanos) {
            choreographer.postFrameCallback(this);

            if(apeStarted) {
                ApertusJNI.processEventDoubleQueue();
            }
        }
    }

    class myOnTouchListener implements View.OnTouchListener {

        @SuppressLint("ClickableViewAccessibility")
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            float x = event.getX();
            float y = event.getY();
////            Log.d("javalog","SurfaceView size: " + v.getWidth() + " " + v.getHeight());
////            Log.d("javalog","Touch at: " + x + " " + y);
//
//            if (event.getAction() == MotionEvent.ACTION_DOWN) {
//                Log.d("javalog", "ACTION_DOWN");
//            }
//            else if (event.getAction() == MotionEvent.ACTION_MOVE) {
//                Log.d("javalog", "ACTION_MOVE");
//            } else if (event.getAction() == MotionEvent.ACTION_UP) {
//                Log.d("javalog", "ACTION_UP");
//            }

            return true;
        }
    }
}
