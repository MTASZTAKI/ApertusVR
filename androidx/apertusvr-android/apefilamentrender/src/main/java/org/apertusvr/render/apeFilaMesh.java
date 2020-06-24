package org.apertusvr.render;

import com.google.android.filament.Box;
import com.google.android.filament.Entity;
import com.google.android.filament.IndexBuffer;
import com.google.android.filament.VertexBuffer;

import org.apertusvr.apeNode;

class apeFilaMesh {
    apeFilaMesh() {
        this.indexBuffer = null;
        this.vertexBuffer = null;
        this.aabb = null;
        parentNode = null;
    }

    apeFilaMesh(int renderable, IndexBuffer indexBuffer, VertexBuffer vertexBuffer, Box box) {
        this.renderable = renderable;
        this.indexBuffer = indexBuffer;
        this.vertexBuffer = vertexBuffer;
        this.aabb = box;
        parentNode = null;
    }

    @Entity int renderable;
    IndexBuffer indexBuffer;
    VertexBuffer vertexBuffer;
    apeNode parentNode;
    Box aabb;
}
