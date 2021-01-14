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
import com.google.android.filament.EntityInstance;
import com.google.android.filament.EntityManager;
import com.google.android.filament.TransformManager;

import org.apertusvr.apeMatrix4;

import java.util.Objects;

public class apeFilaTransform {

    public apeFilaTransform(TransformManager tcm) {
        entity = EntityManager.get().create();
        transform = tcm.create(entity);
    }

    public apeFilaTransform(int entity, int transform) {
        this.entity = entity;
        this.transform = transform;
    }

    public void setTransform(apeMatrix4 transformMx, TransformManager tcm) {
        tcm.setTransform(transform,transformMx.m);
    }

    public void setParent(apeFilaTransform other, TransformManager tcm) {
        tcm.setParent(transform, other.transform);
    }

    public void detach(TransformManager tcm) {
        tcm.setParent(transform, EntityInstance.NULL);
    }

    public void destroy(Engine engine) {
        engine.destroyEntity(entity);
        engine.getTransformManager().destroy(entity);
        EntityManager.get().destroy(entity);
    }


    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        apeFilaTransform that = (apeFilaTransform) obj;
        return entity == that.entity &&
                transform == that.transform;
    }

    @Override
    public int hashCode() {
        return Objects.hash(entity, transform);
    }

    @Entity int entity;
    @EntityInstance int transform;
}
