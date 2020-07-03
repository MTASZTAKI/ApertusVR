package org.apertusvr.render;

import com.google.android.filament.Engine;
import com.google.android.filament.Entity;
import com.google.android.filament.EntityManager;
import com.google.android.filament.LightManager;
import com.google.android.filament.TransformManager;

import org.apertusvr.apeColor;
import org.apertusvr.apeLight;
import org.apertusvr.apeNode;
import org.apertusvr.apeVector3;

class apeFilaLight {
    apeFilaLight(Engine engine) {
        light = EntityManager.get().create();
        engine.getTransformManager().create(light);
        built = false;
        lightBuilder = null;
    }

    void destroy(Engine engine) {
        engine.destroyEntity(light);
        EntityManager.get().destroy(light);
    }

    void setParentTransform(apeFilaTransform transform, TransformManager tcm) {
        tcm.setParent(tcm.getInstance(light),transform.transform);
    }

    @Entity int light;
    LightManager.Builder lightBuilder;
    boolean built;
    LightManager.Type type;

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
