package org.apertusvr;

public class apeCoreConfig {

    protected apeCoreConfig() {}

    public static String getConfigFolderPath() {
        return ApertusJNI.getCoreConfigConfigFolderPath();
    }
    public static String getNetworkGUID() {
        return ApertusJNI.getCoreConfigNetworkGUID();
    }
}
