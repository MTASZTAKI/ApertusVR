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
import com.google.android.filament.IndexBuffer;
import com.google.android.filament.VertexBuffer;

import org.apertusvr.apeVector2;

final class apeFilaPlaneMesh extends apeFilaMesh {

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
