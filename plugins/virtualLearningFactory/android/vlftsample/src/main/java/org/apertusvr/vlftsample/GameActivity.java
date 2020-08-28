package org.apertusvr.vlftsample;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Choreographer;
import android.view.SurfaceView;

import org.apertusvr.ApertusJNI;
import org.apertusvr.apeDegree;
import org.apertusvr.apeEntity;
import org.apertusvr.apeNode;
import org.apertusvr.apeSceneNetwork;
import org.apertusvr.apeSceneManager;
import org.apertusvr.apeVector3;
import org.apertusvr.render.apeFilamentRenderPlugin;
import org.apertusvr.apeCoreConfig;
import org.apertusvr.apeColor;
import org.apertusvr.apeManualMaterial;
import org.apertusvr.apeConeGeometry;
import org.apertusvr.apeVector2;
import org.apertusvr.apeTextGeometry;
import org.apertusvr.apeSystem;

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

        if (!apeSystem.isRunning()) {
            apeSystem.start("vlftGuest",getAssets());
        }

        apeSceneNetwork.connectToRoom(roomName);
        apeNode userNode = createUserAvatar(userName);

        setContentView(surfaceView);
        initRenderPlugin(userNode);
    }


    @Override
    public void onBackPressed() {
        // forbid back button usage in app
        // this is not necessary if the ApertusVR start/stop methods are fixed.
    }

    private void initRenderPlugin(apeNode userNode) {
        renderPlugin = new apeFilamentRenderPlugin(
                this, getLifecycle(), surfaceView,
                getResources(), getAssets(), userNode);
        getLifecycle().addObserver(renderPlugin);
    }

    private apeNode createUserAvatar(String userName) {

        if(userName != null && userName.length() > 0) {
            String GUID = apeCoreConfig.getNetworkGUID();

            apeNode userNode = apeSceneManager.createNode(
                    userName + "_" + GUID, true, GUID);
            if (userNode != null && userNode.isValid()) {

                userNode.setPosition(new apeVector3(0f, 150f, 150f));

                apeManualMaterial userMaterial = Objects.requireNonNull(apeSceneManager.createEntity(
                        userName + "_" + GUID + "_Material",
                        apeEntity.Type.MATERIAL_MANUAL,
                        true, GUID)).cast(new apeManualMaterial.apeManualMaterialBuilder());

                if(userMaterial != null && userMaterial.isValid()) {
                    apeColor color = new apeColor(
                            (float) Math.random(),
                            (float) Math.random(),
                            (float) Math.random());

                    userMaterial.setDiffuseColor(color);
                    userMaterial.setSpecularColor(color);

                    apeNode userConeNode = apeSceneManager.createNode(
                            userName + "_" + GUID + "_ConeNode", true, GUID);

                    if (userConeNode != null && userConeNode.isValid()) {
                        userConeNode.setParentNode(userNode);
                        userConeNode.rotate(270f * apeDegree.deg2rad, apeVector3.RIGHT, apeNode.TransformationSpace.WORLD);

                        apeConeGeometry userCone = Objects.requireNonNull(apeSceneManager.createEntity(
                                userName + "_" + GUID + "_ConeGeometry",
                                apeEntity.Type.GEOMETRY_CONE,
                                true, GUID)).cast(new apeConeGeometry.apeConeBuilder());

                        if (userCone != null && userCone.isValid()) {
                            userCone.setParameters(10.0f,30.0f, 1.0f, apeVector2.ONE);
                            userCone.setParentNode(userConeNode);
                            userCone.setMaterial(userMaterial);
                        }
                    }

                    apeNode userNameTextNode = apeSceneManager.createNode(
                            userName + "_" + GUID + "_TextNode", true, GUID);
                    if (userNameTextNode != null && userNameTextNode.isValid()) {
                        userNameTextNode.setParentNode(userNode);
                        userNameTextNode.setPosition(new apeVector3(0f, 10f, 0f));

                        apeTextGeometry userNameText = Objects.requireNonNull(apeSceneManager.createEntity(
                                userName + "_" + GUID + "_TextGeometry", apeEntity.Type.GEOMETRY_TEXT, true, GUID))
                                .cast(new apeTextGeometry.apeTextGeometryBuilder());

                        if (userNameText != null && userNameText.isValid()) {
                            userNameText.setCaption(userName);
                            userNameText.setParentNode(userNameTextNode);
                        }
                    }
                }

                return userNode;
            }
        }

        return null;
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
    protected void onStop() {
        super.onStop();
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
            ApertusJNI.processEventDoubleQueue();
        }
    }
}