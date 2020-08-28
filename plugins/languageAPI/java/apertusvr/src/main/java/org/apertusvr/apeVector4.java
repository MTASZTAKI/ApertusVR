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

import androidx.annotation.Size;

import org.jetbrains.annotations.NotNull;

public final class apeVector4 {

    public apeVector4() {
        x = y = z = w = 0;
    }

    public apeVector4(float x, float y, float z, float w) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w;
    }

    apeVector4(@NotNull @Size(4) float[] xyzw) {
        x = xyzw[0];
        y = xyzw[1];
        z = xyzw[2];
        w = xyzw[3];
    }

    public apeVector4(final apeVector4 v) {
        this.x = v.x;
        this.y = v.y;
        this.z = v.z;
        this.w = v.w;
    }

    public float length() {
        return (float) Math.sqrt(x * x + y * y + z * z + w * w);
    }

    public float squaredLength() {
        return x * x + y * y + z * z + w * w;
    }

    public float distance(final apeVector4 rkVector) {
        float dx = this.x - rkVector.x;
        float dy = this.y - rkVector.y;
        float dz = this.z - rkVector.z;
        float dw = this.w - rkVector.w;
        return (float) Math.sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
    }

    public float dotProduct(final apeVector4 rkVector) {
        return x * rkVector.x + y * rkVector.y + z * rkVector.z + w * rkVector.w;
    }

    public apeVector4 add(final apeVector4 v2) {
        return new apeVector4(x + v2.x, y + v2.y, z + v2.z, w + v2.w);
    }

    public apeVector4 scale(float c) {
        return new apeVector4(c * x, c * y, c * z, c * w);
    }

    public boolean equals(apeVector4 v2, float tolerance) {
        return  (Math.abs(x - v2.x) < tolerance) &&
                (Math.abs(y - v2.y) < tolerance) &&
                (Math.abs(z - v2.z) < tolerance) &&
                (Math.abs(w - v2.w) < tolerance);
    }

    public boolean equals(apeVector4 v2) {
        return this.equals(v2, eps);
    }

    @NotNull
    @Override
    public String toString() {
        return x + ", " + y + ", " + z + ", " + w;
    }

    public String toJsonString() {
        return "{ \"x\": " + x + ", \"y\": " + y + ", \"z\": " + z + ", \"w\": " + w + " }";
    }

    public float getX() {
        return x;
    }

    public float getY() {
        return y;
    }

    public float getZ() {
        return z;
    }

    public float getW() {
        return w;
    }

    public @Size(4) float[] toArray() {
        return new float[] {x, y, z ,w};
    }


    public float x, y, z, w;

    private static final float eps = 1e-08f;
}
