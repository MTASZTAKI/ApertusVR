package org.apertusvr.app;

import org.apertusvr.ApertusJNI;

import org.apertusvr.apeSystem;
import org.apertusvr.render.apeFilamentRenderPlugin;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.Choreographer;
import android.view.SurfaceView;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Objects;


public class MainActivity extends AppCompatActivity {

    private static final String LOG_TAG = "javalog.MainActivity";

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

        if (!apeSystem.isRunning()) {
            apeSystem.start("androidSampleScene", getAssets());
        }

        initRenderPlugin();
    }

    private void initRenderPlugin() {
        renderPlugin = new apeFilamentRenderPlugin(
                this, getLifecycle(), surfaceView,
                getResources(), getAssets(), null);
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
        if(apeSystem.isRunning()) {
            apeSystem.stop();
        }
    }


    class FrameCallback implements Choreographer.FrameCallback {
        @Override
        public void doFrame(long frameTimeNanos) {
            choreographer.postFrameCallback(this);

            if(apeSystem.isRunning()) {
                ApertusJNI.processEventDoubleQueue();
            }
        }
    }
}
