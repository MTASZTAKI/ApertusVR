package org.apertusvr.vlftsample;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Choreographer;
import android.view.SurfaceView;

import org.apertusvr.ApertusJNI;
import org.apertusvr.apeSceneNetwork;
import org.apertusvr.render.apeFilamentRenderPlugin;

import java.util.Objects;

public class GameActivity extends AppCompatActivity {

    private static final String LOG_TAG = "javalog.GameActivity";


    Choreographer choreographer;
    private FrameCallback frameCallback = new FrameCallback();


    /* rendering */
    private apeFilamentRenderPlugin renderPlugin;
    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Objects.requireNonNull(getSupportActionBar()).hide();

        setContentView(R.layout.activity_game);
        choreographer = Choreographer.getInstance();
        surfaceView = new SurfaceView(this);

        Intent intent = getIntent();
        String userName = intent.getStringExtra(MainActivity.EXTRA_USERNAME);
        String roomName = intent.getStringExtra(MainActivity.EXTRA_ROOMNAME);

        if (MainActivity.apeStarted) {
            apeSceneNetwork.connectToRoom(roomName);
        }

        setContentView(surfaceView);
        initRenderPlugin();
    }

    private void initRenderPlugin() {
        renderPlugin = new apeFilamentRenderPlugin(
                this, getLifecycle(), surfaceView,
                getResources(), getAssets());
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

        if(MainActivity.apeStarted) {
            // TODO: ...
            Log.d(LOG_TAG,"STOP APEVR");
            ApertusJNI apeJNI = new ApertusJNI();
            apeJNI.stopApertusVR();
            MainActivity.apeStarted = false;
        }
    }

    class FrameCallback implements Choreographer.FrameCallback {
        @Override
        public void doFrame(long frameTimeNanos) {
            choreographer.postFrameCallback(this);
            ApertusJNI.processEventDoubleQueue();
        }
    }
}