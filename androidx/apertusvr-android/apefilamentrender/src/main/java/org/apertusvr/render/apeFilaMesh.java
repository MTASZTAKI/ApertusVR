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
        this.parentTransform = null;
    }

    apeFilaMesh(int renderable, IndexBuffer indexBuffer, VertexBuffer vertexBuffer, Box box) {
        this.renderable = renderable;
        this.indexBuffer = indexBuffer;
        this.vertexBuffer = vertexBuffer;
        this.aabb = box;
        this.parentTransform = null;
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
        tcm.setParent(tcm.getInstance(renderable), transform.transform);
    }

    @Entity int renderable;
    IndexBuffer indexBuffer;
    VertexBuffer vertexBuffer;
    Box aabb;
    apeFilaTransform parentTransform;
}
