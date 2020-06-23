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

package org.apertusvr;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

public final class apeColor {

    public apeColor() {
        r = g = b = a = 0;
    }

    public apeColor(float r, float g, float b, float a) {
        this.r = r;
        this.g = g;
        this.b = b;
        this.a = a;
    }

    public apeColor(@Size(4) float[] rgba) {
        r = rgba[0];
        g = rgba[1];
        b = rgba[2];
        a = rgba[3];
    }

    public apeColor(apeColor other) {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }

    public boolean equals(apeColor other, float tolerance) {
        return
            Math.abs(r - other.r) < tolerance &&
            Math.abs(g - other.g) < tolerance &&
            Math.abs(b - other.b) < tolerance &&
            Math.abs(a - other.a) < tolerance;
    }

    public boolean equals(apeColor other) {
        return equals(other, eps);
    }

    public float getR() {
        return r;
    }

    public float getG() {
        return g;
    }

    public float getB() {
        return b;
    }

    public float getA() {
        return a;
    }

    @NonNull
    @Override
    public String toString() {
        return r + ", " + g + ", " + b + ", " + a;
    }

    public String toJsonString() {
        return "{ \"r\": " + r + ", \"g\": " + g + ", \"b\": " + b + ", \"a\": " + a + " }";
    }

    public @Size(4) float[] toArray() {
        return new float[] {r, g, b, a};
    }

    public float r, g, b, a;

    private static final float eps;

    static {
        eps = 1e-08f;
    }
}
