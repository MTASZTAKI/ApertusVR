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
