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


import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Choreographer;
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button startBtn = findViewById(R.id.start_button);

        final AssetManager assetManager = getAssets();
        choreographer = Choreographer.getInstance();

        startBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                apeJNI.startApertusVR(assetManager);
                apeStarted = true;
            }
        });

        apeEventManager.connectEvent(
                apeEvent.Group.LIGHT,
                eventCallback);

        apeEventManager.connectEvent(
                apeEvent.Group.NODE,
                eventCallback
        );

        apeEventManager.connectEvent(
                apeEvent.Group.GEOMETRY_FILE,
                eventCallback
        );

        Log.d("javalog","HELLO JAVA LOG");

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        String hello = "Hello ApertusVR";
        tv.setText(hello);
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
