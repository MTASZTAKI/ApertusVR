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

public final class apeQuaternion {

    public apeQuaternion() {
        w = 1f;
        x = y = z = 0;
    }

    public apeQuaternion(float w, float x, float y, float z) {
        this.w = w;
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public apeQuaternion(final float[] wxyz) {
        this.w = wxyz[0];
        this.x = wxyz[1];
        this.y = wxyz[2];
        this.z = wxyz[3];
    }

    public apeQuaternion(final apeRadian angle, final apeVector3 axis) {
        fromAngleAxis(angle,axis);
    }

    public apeQuaternion(final apeMatrix3 kRot) {
        fromRotationalMatrix(kRot);
    }

    public apeQuaternion product(final apeQuaternion rkQ) {
        return new apeQuaternion(
                w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
                w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
                w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
                w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x);
    }

    public apeQuaternion add(final apeQuaternion q2) {
        return new apeQuaternion(w + q2.w, x + q2.x, y + q2.y, z + q2.z);
    }

    public apeQuaternion subtract(final apeQuaternion rkQ) {
        return new apeQuaternion(w - rkQ.w, x - rkQ.x, y - rkQ.y, z - rkQ.z);
    }

    public apeQuaternion scale(float s) {
        return new apeQuaternion(s * w, s * x, s * y, s * z);
    }

    public apeVector3 apply(apeVector3 v) {
        apeVector3 qvec = new apeVector3(x,y,z);
        apeVector3 uv = qvec.crossProduct(v);
        apeVector3 uuv = qvec.crossProduct(uv);
        uv = uv.scale(2.0f * w);
        uuv = uuv.scale(2.0f);
        return v.add(uv).add(uuv);
    }

    public void fromAngleAxis(final apeRadian angle, final apeVector3 axis) {
        float halfAngle = 0.5f * angle.radian;
        float sinHalfAngle = (float)Math.sin(halfAngle);

        w = (float)Math.cos(halfAngle);
        x = sinHalfAngle * axis.x;
        y = sinHalfAngle * axis.y;
        z = sinHalfAngle * axis.z;
    }

    public boolean equals(final apeQuaternion q2, float tolerance) {
        return  (Math.abs(w - q2.w) < tolerance) &&
                (Math.abs(x - q2.x) < tolerance) &&
                (Math.abs(y - q2.y) < tolerance) &&
                (Math.abs(z - q2.z) < tolerance);
    }

    public boolean equals(final apeQuaternion q2) {
        return this.equals(q2, eps);
    }

    public float norm() {
        return (float)Math.sqrt(w * w + x * x + y * y + z * z);
    }

    public float normalize() {
        float length = (float)Math.sqrt(w * w + x * x + y * y + z * z);
        float lengthInv = 1 / length;

        w *= lengthInv;
        x *= lengthInv;
        y *= lengthInv;
        z *= lengthInv;

        return length;
    }

    public apeQuaternion inverse() {
        float squaredAbs = w * w + x * x + y * y + z * z;
        if(squaredAbs < eps) return new apeQuaternion(0f,0f,0f,0f);

        float squaredAbsInv = 1f / squaredAbs;
        return new apeQuaternion(
                w * squaredAbsInv,
                -x * squaredAbsInv,
                -y * squaredAbsInv,
                -z * squaredAbsInv);
    }

    public float dot(final apeQuaternion q2) {
        return w * q2.w + x * q2.x + y * q2.y + z * q2.z;
    }

    public void toRotationalMatrix(apeMatrix3 kRot) {
        float fTx = x + x;
        float fTy = y + y;
        float fTz = z + z;
        float fTwx = fTx * w;
        float fTwy = fTy * w;
        float fTwz = fTz * w;
        float fTxx = fTx * x;
        float fTxy = fTy * x;
        float fTxz = fTz * x;
        float fTyy = fTy * y;
        float fTyz = fTz * y;
        float fTzz = fTz * z;

//        kRot.set(0,0,1.0f - (fTyy + fTzz));
//        kRot.set(0,1, fTxy - fTwz);
//        kRot.set(0,2,fTxz + fTwy);
//        kRot.set(1,0, fTxy + fTwz);
//        kRot.set(1,1, 1.0f - (fTxx + fTzz));
//        kRot.set(1,2,fTyz - fTwx);
//        kRot.set(2,0,fTxz - fTwy);
//        kRot.set(2,1,fTyz + fTwx);
//        kRot.set(2,2,1.0f - (fTxx + fTyy));

        kRot.m = new float[] {
                1.0f - (fTyy + fTzz),   fTxy - fTwz,            fTxz + fTwy,
                fTxy + fTwz,            1.0f - (fTxx + fTzz),   fTyz - fTwx,
                fTxz - fTwy,            fTyz + fTwx,            1.0f - (fTxx + fTyy)
        };
    }

    public void fromRotationalMatrix(final apeMatrix3 kRot) {
        float trace = kRot.trace();
        float root;

        if(trace > eps) {
            root = (float)Math.sqrt(trace + 1f);
            w = 0.5f * root;
            root = 0.5f / root;
            x = (kRot.get(2,1) - kRot.get(1,2)) * root;
            y = (kRot.get(0,2) - kRot.get(2,0)) * root;
            z = (kRot.get(1,0) - kRot.get(0,1)) * root;
        } else if ((kRot.m[0] > kRot.m[4]) && kRot.m[0] > kRot.m[8]) {
            root = (float) Math.sqrt(1.0f + kRot.m[0] - kRot.m[4] - kRot.m[8]);
            x = 0.5f * root;
            root = 0.5f / root;
            w = (kRot.get(2,1) - kRot.get(1,2)) * root;
            y = (kRot.get(0,1) - kRot.get(1,0)) * root;
            z = (kRot.get(0,2) - kRot.get(2,0)) * root;
        } else if (kRot.m[4] > kRot.m[8]) {
            root = (float) Math.sqrt(1.0f + kRot.m[4] - kRot.m[0] - kRot.m[8]);
            y = 0.5f * root;
            root = 0.5f / root;
            w = (kRot.get(0,2) - kRot.get(2,0)) * root;
            x = (kRot.get(0,1) - kRot.get(1,0)) * root;
            z = (kRot.get(1,2) - kRot.get(2,1)) * root;
        } else {
            root = (float) Math.sqrt(1.0f + kRot.m[8] - kRot.m[0] - kRot.m[4]);
            z = 0.5f * root;
            root = 0.5f / root;
            w = kRot.get(1,0) - kRot.get(0,1) * root;
            x = kRot.get(0,2) - kRot.get(2,0) * root;
            y = kRot.get(1,2) - kRot.get(2,1) * root;
        }
    }

    public float getW() {
        return w;
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

    public @Size(4) float[] toArray() {
        return (new float[] {w, x, y, z});
    }

    public void getAngleAxis(apeRadian angle, apeVector3 axis) {
        angle = new apeRadian((float) Math.acos(w) * 2.0f);
        float vecNorm = (float)Math.sqrt(1f - w);
        axis = new apeVector3(x/vecNorm,y/vecNorm,z/vecNorm);
    }

    @NonNull
    @Override
    public String toString() {
        return w + "," + x + ", " + y + ", " + z;
    }

    public String toJsonString() {
        return "{ \"w\": " + w + ", \"x\": " + x + ", \"y\": " + y + ", \"z\": " + z + " }";
    }

    public float w, x, y, z; // w + xi + yj + zk

    private static final float eps = 1e-08f;
}
