package org.apertusvr.render;

import com.google.android.filament.IndirectLight;
import com.google.android.filament.Skybox;
import com.google.android.filament.Texture;

public class apeFilaIbl {

    apeFilaIbl(IndirectLight indirectLight, Texture texture, Skybox skybox, Texture skyboxTexure) {

    }

    apeFilaIbl() {
        indirectLight = null;
        indirectLightTexture = null;
        skybox = null;
        skyboxTexture = null;
    }

    IndirectLight indirectLight;
    Texture indirectLightTexture;
    Skybox skybox;
    Texture skyboxTexture;
}
