package org.apertusvr;

import android.content.res.AssetManager;

public class apeSystem {

    protected apeSystem() {}

    public static void start(String confPath, AssetManager assets) {
        ApertusJNI.startApertusVR(confPath, assets);
        running = true;
    }

    public static void stop() {
        ApertusJNI.stopApertusVR();
        running = false;
    }

    public static boolean isRunning() {
        return running;
    }

    private static boolean running = false;
}
