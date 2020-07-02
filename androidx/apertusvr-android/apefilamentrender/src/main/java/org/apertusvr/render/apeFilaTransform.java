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
        tcm.setTransform(transform,transformMx.getArray());
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
