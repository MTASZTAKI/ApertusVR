package org.apertusvr.render;

import com.google.android.filament.Box;
import com.google.android.filament.IndexBuffer;
import com.google.android.filament.VertexBuffer;

import org.apertusvr.apeVector2;

class apeFilaPlaneMesh extends apeFilaMesh {

    apeFilaPlaneMesh() {
        super();
        size = new apeVector2(1f,1f);
    }

    apeFilaPlaneMesh(int renderable, IndexBuffer indexBuffer, VertexBuffer vertexBuffer, Box box) {
        super(renderable,indexBuffer,vertexBuffer,box);
        size = new apeVector2(1f,1f);
    }

    apeFilaPlaneMesh(apeVector2 size) {
        super();
        this.size = size;
    }

    apeFilaPlaneMesh(int renderable, IndexBuffer indexBuffer, VertexBuffer vertexBuffer, Box box, apeVector2 size) {
        super(renderable,indexBuffer,vertexBuffer,box);
        this.size = size;
    }

    apeVector2 size;
}
