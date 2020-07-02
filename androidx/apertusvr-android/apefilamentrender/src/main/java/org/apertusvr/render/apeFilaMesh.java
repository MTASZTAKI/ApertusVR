package org.apertusvr.render;

import com.google.android.filament.Box;
import com.google.android.filament.Entity;
import com.google.android.filament.IndexBuffer;
import com.google.android.filament.TransformManager;
import com.google.android.filament.VertexBuffer;

import org.apertusvr.apeNode;

import java.util.Objects;

class apeFilaMesh {
    apeFilaMesh() {
        this.indexBuffer = null;
        this.vertexBuffer = null;
        this.aabb = null;
    }

    apeFilaMesh(int renderable, IndexBuffer indexBuffer, VertexBuffer vertexBuffer, Box box) {
        this.renderable = renderable;
        this.indexBuffer = indexBuffer;
        this.vertexBuffer = vertexBuffer;
        this.aabb = box;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        apeFilaMesh that = (apeFilaMesh) obj;
        return renderable == that.renderable;
    }

    @Override
    public int hashCode() {
        return Objects.hash(renderable);
    }

    public void setParentTransform(apeFilaTransform transform, TransformManager tcm) {
        tcm.setParent(renderable, transform.transform);
    }

    @Entity int renderable;
    IndexBuffer indexBuffer;
    VertexBuffer vertexBuffer;
    Box aabb;
}
