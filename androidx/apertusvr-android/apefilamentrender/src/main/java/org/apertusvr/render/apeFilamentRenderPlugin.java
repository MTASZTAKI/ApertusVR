/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

package org.apertusvr.render;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.view.Choreographer;
import android.view.Surface;
import android.view.SurfaceView;

import androidx.lifecycle.Lifecycle;

import org.apertusvr.*;

import com.google.android.filament.*;
import com.google.android.filament.android.UiHelper;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.channels.Channels;
import java.nio.channels.ReadableByteChannel;
import java.util.ArrayList;
import java.util.Map;
import java.util.Objects;
import java.util.TreeMap;

public class apeFilamentRenderPlugin implements apePlugin {
    static {
        Filament.init();
    }

    private Context mContext;
    private Choreographer mChoreographer;
    private Lifecycle mLifecycle;
    private SurfaceView mSurfaceView;
    private AssetManager mAssets;
    private String mResourcePath;

    private UiHelper mUiHelper;
    private Engine mEngine;
    private Renderer mRenderer;
    private Scene mScene;
    private View mView;
    private Camera mCamera;

    private Material mColoredMaterial;
    private Material mTexturedMaterial;
    private TreeMap<String, MaterialInstance> mMaterialInstances;

    private TreeMap<String, apeFilaMesh> mMeshes;
    private TreeMap<String, apeFilaLight> mLights;

    private SwapChain mSwapChain = null;

    private FrameCallback mFrameCallback = new FrameCallback();
    private EventCallback mEventCallback = new EventCallback();

    private apeDoubleQueue<apeEvent> mEventDoubleQueue;
    private apeDoubleQueue<apeEvent> mLightEventDoubleQueue;

    private static final float MAT_EPS = 1e-8f;
    private static final apeColor CLEAR_COLOR = new apeColor(0.35f,0.35f,0.35f,1.0f);
    private static final apeDegree FOV = new apeDegree(45.0f);



    public apeFilamentRenderPlugin(Context context, Lifecycle lifecycle, SurfaceView surfaceView, AssetManager assets, String resourcePath) {
        mContext = context;
        mLifecycle = lifecycle;
        mSurfaceView = surfaceView;
        mAssets = assets;
        mResourcePath = resourcePath;
    }

    /* -- Lifecycle event handlers -- */

    @Override
    public void onCreate() {
        mMaterialInstances = new TreeMap<>();
        mChoreographer = Choreographer.getInstance();
        mEventDoubleQueue = new apeDoubleQueue<>(true);
        mLightEventDoubleQueue = new apeDoubleQueue<>(true);
        mLights = new TreeMap<>();
        mMeshes = new TreeMap<>();

        setupSurfaceView();
        setupFilament();
        setupView();

        /* event connection */
        apeEventManager.connectEvent(apeEvent.Group.NODE, mEventCallback);
        apeEventManager.connectEvent(apeEvent.Group.LIGHT, mEventCallback);
        apeEventManager.connectEvent(apeEvent.Group.GEOMETRY_FILE, mEventCallback);
        apeEventManager.connectEvent(apeEvent.Group.GEOMETRY_PLANE, mEventCallback);
    }

    @Override
    public void onStart() {

    }

    @Override
    public void onResume() {
        mChoreographer.postFrameCallback(mFrameCallback);
    }

    @Override
    public void onPause() {
        mChoreographer.removeFrameCallback(mFrameCallback);
    }

    @Override
    public void onDestroy() {

    }

    /* -- setup functions -- */

    private void setupSurfaceView() {
        mUiHelper = new UiHelper(UiHelper.ContextErrorPolicy.DONT_CHECK);
        mUiHelper.setRenderCallback(new SurfaceCallback());
        mUiHelper.attachTo(mSurfaceView);
    }

    private void setupFilament() {
        mEngine = Engine.create();
        mRenderer = mEngine.createRenderer();
        mScene = mEngine.createScene();
        mView = mEngine.createView();
        mCamera = mEngine.createCamera();
    }

    private void setupView () {
        mView.setClearColor(CLEAR_COLOR.r,CLEAR_COLOR.g,CLEAR_COLOR.b,CLEAR_COLOR.a);
        mView.setAmbientOcclusion(View.AmbientOcclusion.SSAO);
        mView.setCamera(mCamera);
        mView.setScene(mScene);
        View.DynamicResolutionOptions options = new View.DynamicResolutionOptions();
        options.enabled = true;
        mView.setDynamicResolutionOptions(options);
    }

    private void processLightEventDoubleQueue() {
        mLightEventDoubleQueue.swap();
        while(!mLightEventDoubleQueue.emptyPop()) {
            apeEvent event = mLightEventDoubleQueue.front();

            apeLight light = (apeLight) apeSceneManager.getEntity(event.subjectName);
            if (light != null) {
                if (event.type == apeEvent.Type.LIGHT_CREATE) {
                    apeFilaLight filaLight = new apeFilaLight();
                    filaLight.light = EntityManager.get().create();
                    mLights.put(event.subjectName, filaLight);
                    mScene.addEntity(filaLight.light);
                }
                else if (event.type == apeEvent.Type.LIGHT_DELETE) {
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                    if (filaLight != null) {
                        mScene.removeEntity(filaLight.light);
                        mEngine.destroyEntity(filaLight.light);
                        EntityManager.get().destroy(filaLight.light);
                        mLights.remove(event.subjectName);
                    }
                }
                else if (event.type == apeEvent.Type.LIGHT_TYPE) {
                    apeLight.LightType apeLightType = light.getLightType();
                    LightManager.Type filaLightType = apeFilaLight.ape2filaType(apeLightType);
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                    if(filaLightType != null && filaLight != null) {
                        filaLight.lightBuilder = new LightManager.Builder(filaLightType);
                    }
                }
                else if (event.type == apeEvent.Type.LIGHT_ATTENUATION) {
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                }
                else if (event.type == apeEvent.Type.LIGHT_DIRECTION) {
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                    if(filaLight != null && filaLight.lightBuilder != null) {
                        apeVector3 direction = light.getLightDirection();
                        filaLight.lightBuilder.direction(direction.x,direction.y,direction.z);
                        filaLight.built = false;
                    }
                }
                else if (event.type == apeEvent.Type.LIGHT_DIFFUSE) {
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                    if(filaLight != null && filaLight.lightBuilder != null) {
                        apeColor color = light.getDiffuseColor();
                        filaLight.lightBuilder.color(color.r,color.g,color.b);
                        filaLight.built = false;
                    }
                }
                else if (event.type == apeEvent.Type.LIGHT_SPECULAR) {
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                    apeColor specular = light.getSpecularColor();
                    if(filaLight != null && filaLight.lightBuilder != null) {
                        float x = specular.r * specular.g * specular.b;
                        float intensity = (float) Math.pow(10,6*Math.log10(1 + 9*x));
                        filaLight.lightBuilder.intensity(intensity);
                        filaLight.built = false;
                    }
                }
                else if (event.type == apeEvent.Type.LIGHT_SPOTRANGE) {
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                    if(filaLight != null && filaLight.lightBuilder != null) {
                        apeLight.LightSpotRange spotRange = light.getLightSpotRange();
                        filaLight.lightBuilder
                                .falloff(spotRange.falloff)
                                .spotLightCone(
                                        spotRange.innerAngle.degree,
                                        spotRange.outerAngle.degree);
                        filaLight.built = false;
                    }
                }
                else if (event.type == apeEvent.Type.LIGHT_PARENTNODE) {
                    apeFilaLight filaLight = mLights.get(event.subjectName);
                    if(filaLight != null) {
                        filaLight.parentNode = light.getParentNode();
                    }
                }
            }
            mLightEventDoubleQueue.pop();
        }

        for (Map.Entry<String,apeFilaLight> entry : mLights.entrySet()) {
            apeFilaLight filaLight = entry.getValue();
            if (!filaLight.built) {
                filaLight.lightBuilder.build(mEngine, filaLight.light);
                filaLight.built = true;
            }
        }
    }

    private void processEventDoubleQueue() {
        mEventDoubleQueue.swap();
        while(!mEventDoubleQueue.emptyPop()) {
            apeEvent event = mEventDoubleQueue.front();

            if(event.group == apeEvent.Group.GEOMETRY_FILE) {
                apeFileGeometry fileGeometry = new apeFileGeometry(event.subjectName);
                if (event.type == apeEvent.Type.GEOMETRY_FILE_CREATE) {
                    mMeshes.put(event.subjectName, null);
                }
                else if(event.type == apeEvent.Type.GEOMETRY_FILE_DELETE) {

                }
                else if (event.type == apeEvent.Type.GEOMETRY_FILE_FILENAME) {
                    apeFilaMesh filaMesh = mMeshes.get(event.subjectName);
                    if(filaMesh != null) {
                        String fileName = fileGeometry.getFileName();
                        try {
                            String fileNameRaw = fileName.split(".")[0];

                            String mtlPath = mResourcePath + fileNameRaw + ".mtl";
                            FileInputStream fis = new FileInputStream(mtlPath);


                            String filameshPath = mResourcePath + fileNameRaw + ".filamesh";
                            apeFilaMeshLoader.loadMesh(
                                    new FileInputStream(filameshPath),
                                    event.subjectName,
                                    mMaterialInstances,
                                    mEngine,
                                    filaMesh
                            );
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
                else if (event.type == apeEvent.Type.GEOMETRY_FILE_PARENTNODE) {

                }

            }
            else if (event.group == apeEvent.Group.GEOMETRY_PLANE) {

            }


            mEventDoubleQueue.front();
        }
    }

    /* -- Callback functors -- */

    class EventCallback implements apeEventCallback {

        @Override
        public void onEvent(apeEvent event) {
            if(event.group == apeEvent.Group.LIGHT) {
                mLightEventDoubleQueue.push(event);
            } else {
                mEventDoubleQueue.push(event);
            }
        }
    }

    class FrameCallback implements Choreographer.FrameCallback {

        @Override
        public void doFrame(long frameTimeNanos) {
            processLightEventDoubleQueue();
        }
    }

    class SurfaceCallback implements UiHelper.RendererCallback {
        @Override
        public void onNativeWindowChanged(Surface surface) {
            if(mSwapChain != null) mEngine.destroySwapChain(mSwapChain);
            mSwapChain = mEngine.createSwapChain(surface);
        }

        @Override
        public void onDetachedFromSurface() {
            if(mSwapChain != null) {
                mEngine.destroySwapChain(mSwapChain);
                mEngine.flushAndWait();
                mSwapChain = null;
            }
        }

        @Override
        public void onResized(int width, int height) {
            double aspect = (double)width/height;
            mCamera.setProjection(FOV.degree, aspect,0.1,20.0, Camera.Fov.VERTICAL);
            mView.setViewport(new Viewport(0,0,width,height));
        }
    }

    private ByteBuffer readUncompressedAsset(String assetName) throws IOException {
        AssetFileDescriptor fd = mAssets.openFd(assetName);
        InputStream input = fd.createInputStream();
        ByteBuffer dst = ByteBuffer.allocate((int)fd.getLength());

        ReadableByteChannel src = Channels.newChannel(input);
        src.read(dst);
        src.close();

        dst.rewind();
        return dst;
    }
}
