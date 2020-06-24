package org.apertusvr.render;

import com.google.android.filament.Entity;
import com.google.android.filament.LightManager;

import org.apertusvr.apeColor;
import org.apertusvr.apeLight;
import org.apertusvr.apeNode;
import org.apertusvr.apeVector3;

class apeFilaLight {
    apeFilaLight() {
        built = false;
        lightBuilder = null;
        parentNode = null;
    }

    @Entity int light;
    LightManager.Builder lightBuilder;
    boolean built;
    apeNode parentNode;

    static LightManager.Type ape2filaType(apeLight.LightType apeLightType) {
        switch (apeLightType) {
            case SPOT:
                return LightManager.Type.SPOT;
            case DIRECTIONAL:
                return LightManager.Type.DIRECTIONAL;
            case POINT:
                return LightManager.Type.POINT;
            case INVALID:
                return null;
        }

        return null;
    }
}
