package org.apertusvr.app;

import org.apertusvr.ApertusJNI;
import org.apertusvr.apeEvent;
import org.apertusvr.apeEventCallback;
import org.apertusvr.apeEventManager;
import org.apertusvr.apeFileGeometry;
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

        String helloStr = apeJNI.stringFromJNIPlugin("Hello world!");

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

//        apeEventManager.connectEvent(
//                apeEvent.Group.NODE,
//                eventCallback
//        );

        apeEventManager.connectEvent(
                apeEvent.Group.GEOMETRY_FILE,
                eventCallback
        );

        Log.d("javalog","HELLO JAVA LOG");

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        //tv.setText("Hello ApertusVR"/*stringFromJNI(assetManager)*/);
        tv.setText(helloStr);
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
            // Log.d("javalog",input.group.name() + " - " + input.type.name() + " event caught from java with subject name: \"" + input.subjectName + "\"");

            if(input.group == apeEvent.Group.LIGHT) {

                Log.d("javalog", "Light event caught, with subject name:" + input.subjectName);
                Log.d("javalog", "The event's type is: " + input.type.name());

                if (input.type == apeEvent.Type.LIGHT_TYPE) {

                    apeLight light = new apeLight(input.subjectName);

                    if(light.isValid()){
                        Log.d("javalog", light.getName() + " light is valid with type: " + light.getLightType().name());
                    }
                }
//                else if (input.type == apeEvent.Type.LIGHT_PARENTNODE) {
//                    apeLight light = (apeLight) apeSceneManager.getEntity(input.subjectName);
//                    assert light != null : "ligth == null";
//
//                    apeNode parentNode = light.getParentNode();
//                    Log.d("javalog", input.subjectName + "'s parent node is: " + parentNode.getName());
//                }
            }
            else if(input.group == apeEvent.Group.GEOMETRY_FILE) {
                Log.d("javalog", "GeoemtryFile event caught, with subject name: " + input.subjectName);
                Log.d("javalog", "event type is: " + input.type.name());

                apeFileGeometry fileGeometry = new apeFileGeometry(input.subjectName);
                assert fileGeometry.isValid();

                // Log.d("javalog", "filename is:" + fileGeometry.getFileName());

                if(input.type == apeEvent.Type.GEOMETRY_FILE_CREATE) {
                    // Log.d("javalog", "Geometry file" + fileGeometry.getName() + " was created");
                }
                else if(input.type == apeEvent.Type.GEOMETRY_FILE_FILENAME) {
                    Log.d("javalog", "Geometry file: " + fileGeometry.getName() + "'s filename: " + fileGeometry.getFileName());
                }
                else if(input.type == apeEvent.Type.GEOMETRY_FILE_EXPORT) {
                    Log.d("javalog","The geometry's file name is: " + fileGeometry.getFileName());
                }
                else if(input.type == apeEvent.Type.GEOMETRY_FILE_PARENTNODE) {
                    Log.d("javalog", "The geometry's parentnode is: " + fileGeometry.getParentNode().getName());
                }

            }


            if(input.type == apeEvent.Type.NODE_POSITION) {
                apeNode node = apeSceneManager.getNode(input.subjectName);
                assert node != null : "node == null";

                apeVector3 position = node.getDerivedPosition();
                Log.d("javalog",node.getName() + " node position is: " + position);
            }

        }
    }
}
