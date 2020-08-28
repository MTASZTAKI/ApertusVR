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

import org.jetbrains.annotations.NotNull;

public final class apeVector3 {

    static {
        ZERO = new apeVector3(0f,0f,0f);
        UP = new apeVector3(0f,1f,0f);
        RIGHT = new apeVector3(1f,0f,0f);
        FORWARD = new apeVector3(0f,0f,1f);
    }

    public apeVector3() {
        x = y = z = 0;
    }

    public apeVector3(float x, float y, float z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public apeVector3(@NotNull apeVector3 v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    public apeVector3(@NotNull @Size(3) float[] xyz) {
        x = xyz[0];
        y = xyz[1];
        z = xyz[2];
    }

    public float squaredLength() {
        return x * x + y * y + z * z;
    }

    public float length() {
        return (float) Math.sqrt(x * x + y * y + z * z);
    }

    public float distance(final apeVector3 rkVector) {
        float dx = this.x - rkVector.x;
        float dy = this.y - rkVector.y;
        float dz = this.z - rkVector.z;
        return (float) Math.sqrt(dx * dx + dy * dy + dz * dz);
    }

    public apeVector3 crossProduct(final apeVector3 rkVector) {
        return new apeVector3(
                y * rkVector.z - z * rkVector.y,
                z * rkVector.x - x * rkVector.z,
                x * rkVector.y - y * rkVector.x);
    }

    public float dotProduct(final apeVector3 rkVector) {
        return x * rkVector.x + y * rkVector.y + z * rkVector.z;
    }

    public apeVector3 add(final apeVector3 v2) {
        return new apeVector3(x + v2.x, y + v2.y, z + v2.z);
    }

    public apeVector3 scale(float c) {
        return new apeVector3(c * x, c * y, c * z);
    }

    public boolean equals(apeVector3 v2, float tolerance) {
        return  (Math.abs(x - v2.x) < tolerance) &&
                (Math.abs(y - v2.y) < tolerance) &&
                (Math.abs(z - v2.z) < tolerance);
    }

    public boolean equals(apeVector3 v2) {
        return this.equals(v2, eps);
    }

    public boolean lessThan(apeVector3 rkVector) {
        return ((x < rkVector.x) && (y < rkVector.y) && (z < rkVector.z));
    }

    public boolean greaterThan(apeVector3 rkVector) {
        return ((x > rkVector.x) && (y > rkVector.y) && (z > rkVector.z));
    }

    public float normalize() {
        float length = (float) Math.sqrt(x * x + y * y + z * z);
        if (length > eps) {
            float InvLength = 1f / length;
            x *= length;
            y *= length;
            z *= length;
        }

        return length;
    }

    float getX() {
        return x;
    }

    float getY() {
        return y;
    }

    float getZ() {
        return z;
    }

    @NonNull
    @Override
    public String toString() {
        return x + ", " + y + ", " + z;
    }

    public String toJsonString() {
        return "{ \"x\": " + x + ", \"y\": " + y + ", \"z\": " + z + " }";
    }

    public @Size(3) float[] toArray() {
        return new float[] {x, y, z};
    }

    public float x, y, z;

    private static final float eps = 1e-08f;

    public static final apeVector3 ZERO;
    public static final apeVector3 UP;
    public static final apeVector3 RIGHT;
    public static final apeVector3 FORWARD;
}