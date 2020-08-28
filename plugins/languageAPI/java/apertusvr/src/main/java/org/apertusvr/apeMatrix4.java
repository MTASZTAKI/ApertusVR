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

public final class apeMatrix4 {

    static {
        IDENTITY = new apeMatrix4(
                1f, 0f, 0f, 0f,
                0f, 1f, 0f, 0f,
                0f, 0f, 1f, 0f,
                0f, 0f, 0f, 1f
        );

        ZERO = new apeMatrix4(
                0f, 0f, 0f, 0f,
                0f, 0f, 0f, 0f,
                0f, 0f, 0f, 0f,
                0f, 0f, 0f, 0f
        );

        eps = 1e-08f;
    }

    public apeMatrix4() {
//        m = new float[] {
//                1f, 0f, 0f, 0f,
//                0f, 1f, 0f, 0f,
//                0f, 0f, 1f, 0f,
//                0f, 0f, 0f, 1f
//        };
        m = null;
    }

    public apeMatrix4(float m00, float m01, float m02, float m03,
                      float m10, float m11, float m12, float m13,
                      float m20, float m21, float m22, float m23,
                      float m30, float m31, float m32, float m33) {
        m = new float[] {
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33
        };
    }

    public apeMatrix4(@Size(16) float[] m) {
        this.m = m.clone();
    }

    public apeMatrix4(apeMatrix3 M3) {
        m = new float[] {
                M3.m[0], M3.m[1], M3.m[2], 0f,
                M3.m[3], M3.m[4], M3.m[5], 0f,
                M3.m[6], M3.m[7], M3.m[8], 0f,
                0f,      0f,      0f,      1f
        };
    }

    public apeMatrix4(final apeVector3 scale, final apeQuaternion rotation, final apeVector3 translate) {
        makeTransform(scale,rotation,translate);
    }

    public apeMatrix4 multiply(final apeMatrix4 rkM) {
        return new apeMatrix4(
                m[0] * rkM.m[0]  + m[1] * rkM.m[4]  + m[2] * rkM.m[8]  + m[3] * rkM.m[12],
                m[4] * rkM.m[0]  + m[5] * rkM.m[4]  + m[6] * rkM.m[8]  + m[7] * rkM.m[12],
                m[8] * rkM.m[0]  + m[9] * rkM.m[4]  + m[10] * rkM.m[8] + m[11] * rkM.m[12],
                m[12] * rkM.m[0] + m[13] * rkM.m[4] + m[14] * rkM.m[8] + m[15] * rkM.m[12],

                m[0] * rkM.m[1]  +  m[1] * rkM.m[5] + m[2] * rkM.m[9]  + m[3] * rkM.m[13],
                m[4] * rkM.m[1]  +  m[5] * rkM.m[5] + m[6] * rkM.m[9]  + m[7] * rkM.m[13],
                m[8] * rkM.m[1]  +  m[9] * rkM.m[5] + m[10] * rkM.m[9] + m[11] * rkM.m[13],
                m[12] * rkM.m[1] + m[13] * rkM.m[5] + m[14] * rkM.m[9] + m[15] * rkM.m[13],

                m[0] * rkM.m[2]  + m[1] * rkM.m[6]  + m[2] * rkM.m[10]  + m[3] * rkM.m[14],
                m[4] * rkM.m[2]  + m[5] * rkM.m[6]  + m[6] * rkM.m[10]  + m[7] * rkM.m[14],
                m[8] * rkM.m[2]  + m[9] * rkM.m[6]  + m[10] * rkM.m[10] + m[11] * rkM.m[14],
                m[12] * rkM.m[2] + m[13] * rkM.m[6] + m[14] * rkM.m[10] + m[15] * rkM.m[14],

                m[0] * rkM.m[3]  + m[1] * rkM.m[7]  + m[2] * rkM.m[11]  + m[3] * rkM.m[15],
                m[4] * rkM.m[3]  + m[5] * rkM.m[7]  + m[6] * rkM.m[11]  + m[7] * rkM.m[15],
                m[8] * rkM.m[3]  + m[9] * rkM.m[7]  + m[10] * rkM.m[11] + m[11] * rkM.m[15],
                m[12] * rkM.m[3] + m[13] * rkM.m[7] + m[14] * rkM.m[11] + m[15] * rkM.m[15]
                );
    }

    public apeMatrix4 add(final apeMatrix4 rkM) {
        return new apeMatrix4(
                m[0]  + rkM.m[0],  m[1]   + rkM.m[1], m[2]   + rkM.m[2],  m[3]   + rkM.m[3],
                m[4]  + rkM.m[4],  m[5]   + rkM.m[5], m[6]   + rkM.m[6],  m[7]   + rkM.m[7],
                m[8]  + rkM.m[8],  m[9]   + rkM.m[9], m[10]  + rkM.m[10], m[11] + rkM.m[11],
                m[12] + rkM.m[12], m[13]  + rkM.m[13],m[14]  + rkM.m[14], m[15] + rkM.m[15]
        );
    }

    public apeMatrix4 subtract(final apeMatrix4 rkM) {
        return new apeMatrix4(
                m[0]  - rkM.m[0],  m[1]   - rkM.m[1], m[2]   - rkM.m[2],  m[3]   - rkM.m[3],
                m[4]  - rkM.m[4],  m[5]   - rkM.m[5], m[6]   - rkM.m[6],  m[7]   - rkM.m[7],
                m[8]  - rkM.m[8],  m[9]   - rkM.m[9], m[10]  - rkM.m[10], m[11] - rkM.m[11],
                m[12] - rkM.m[12], m[13] - rkM.m[13], m[14] - rkM.m[14],  m[15] - rkM.m[15]
        );
    }

    public apeVector4 apply(final apeVector4 v) {
        return new apeVector4(
                m[0] * v.x  + m[1] * v.y  + m[2] * v.z   + m[3] * v.w,
                m[4] * v.x  + m[5] * v.y  + m[6] * v.z   + m[7] * v.w,
                m[8] * v.x  + m[9] * v.y  + m[10] * v.z  + m[11] * v.w,
                m[12] * v.x + m[13] * v.y + m[14] * v.z  + m[15] * v.w
                );
    }

    public apeVector3 apply(final apeVector3 v) {
        float invW = 1.0f / (m[12] *v.x + m[13] * v.y + m[14] * v.z + m[15]);

        return new apeVector3(
                (m[0] * v.x  + m[1] * v.y + m[2] * v.z + m[3]) * invW,
                (m[4] * v.x  + m[5] * v.y + m[6] * v.z + m[7]) * invW,
                (m[8] * v.x  + m[9] * v.y + m[10] * v.z + m[11]) * invW
        );
    }

    public float get(int i, int j) {
        return m[j + i*4];
    }

    public void set(int i, int j, float val) {
        m[j + i*4] = val;
    }

    public @Size(4) float[] getRow(int i) {
        int k = 4 *i;
        return new float[]{m[k],m[k+1],m[k+2],m[k+3]};
    }

    public @Size(4) float[] getColumn(int j) {
        return new float[] {m[j],m[j+4],m[j+8],m[j+12]};
    }

    public float trace() {
        return m[0] + m[5] + m[10] + m[15];
    }

    public boolean equals(final apeMatrix4 M2, float tolerance) {
        for (int i = 0; i < 16; ++i) {
            if(Math.abs(m[i] - M2.m[i]) > tolerance) return false;
        }

        return true;
    }

    public boolean equals(final apeMatrix4 M2) {
        return equals(M2,eps);
    }

    public apeMatrix4 transpose() {
        return new apeMatrix4(
                m[0], m[4], m[8],  m[12],
                m[1], m[5], m[9],  m[13],
                m[2], m[6], m[10], m[14],
                m[3], m[7], m[11], m[15]
        );
    }

    public void makeTransform(final apeVector3 scale, final apeQuaternion rotation, final apeVector3 translate) {
        apeMatrix3 rot3x3 = new apeMatrix3(rotation);

        m = new float[] {
                rot3x3.m[0] * scale.x, rot3x3.m[1] * scale.y, rot3x3.m[2] * scale.z, translate.x,
                rot3x3.m[3] * scale.x, rot3x3.m[4] * scale.y, rot3x3.m[5] * scale.z, translate.y,
                rot3x3.m[6] * scale.x, rot3x3.m[7] * scale.y, rot3x3.m[8] * scale.z, translate.z,
                0f,                    0f,                    0f,                    1f
        };
    }

    void extract3x3Matrix(apeMatrix3 M3x3) {
        M3x3 = new apeMatrix3(
                m[0], m[1], m[2],
                m[4], m[5], m[6],
                m[8], m[9], m[10]
        );
    }

    void decomposition(apeVector3 scale, apeQuaternion rotation, apeVector3 translate) {
//        apeMatrix3 m3x3 = null;
//        extract3x3Matrix(m3x3);
//
//        apeMatrix3 matQ;
//        apeVector3 vecU;
//        m3x3.QDUDecomposition(matQ, scale, vecU);
//
//        rotation = new apeQuaternion(matQ);
//        translate = new apeVector3(m[3],m[7],m[11]);

        float length0 = (float) Math.sqrt(m[0] * m[0] + m[4] * m[4] + m[8] * m[8]);
        float length1 = (float) Math.sqrt(m[1] * m[1] + m[5] * m[5] + m[9] * m[9]);
        float length2 = (float) Math.sqrt(m[2] * m[2] + m[6] * m[6] + m[10] * m[10]);

        scale = new apeVector3(length0, length1, length2);
        apeMatrix3 rotMat3x3 = new apeMatrix3(
                m[0]/length0, m[1]/length1, m[2]/length2,
                m[4]/length0, m[5]/length1, m[6]/length2,
                m[8]/length0, m[9]/length1, m[10]/length2
        );

        rotation = new apeQuaternion(rotMat3x3);
        translate = new apeVector3(m[3],m[7],m[11]);
    }

    boolean inverse(apeMatrix4 rkInverse, float tolerance) {
        float m00 = m[0],  m01 = m[1],  m02 = m[2],  m03 = m[3];
        float m10 = m[4],  m11 = m[5],  m12 = m[6],  m13 = m[7];
        float m20 = m[8],  m21 = m[9],  m22 = m[10], m23 = m[11];
        float m30 = m[12], m31 = m[13], m32 = m[14], m33 = m[15];

        float v0 = m20 * m31 - m21 * m30;
        float v1 = m20 * m32 - m22 * m30;
        float v2 = m20 * m33 - m23 * m30;
        float v3 = m21 * m32 - m22 * m31;
        float v4 = m21 * m33 - m23 * m31;
        float v5 = m22 * m33 - m23 * m32;

        float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
        float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
        float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
        float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

        float det = t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03;

        if(Math.abs(det) < tolerance) return false;

        float invDet = 1f / det;

        float d00 = t00 * invDet;
        float d10 = t10 * invDet;
        float d20 = t20 * invDet;
        float d30 = t30 * invDet;

        float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;

        float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m21 * m10 - m20 * m11;
        v1 = m22 * m10 - m20 * m12;
        v2 = m23 * m10 - m20 * m13;
        v3 = m22 * m11 - m21 * m12;
        v4 = m23 * m11 - m21 * m13;
        v5 = m23 * m12 - m22 * m13;

        float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        rkInverse = new apeMatrix4(
                d00, d01, d02, d03,
                d10, d11, d12, d13,
                d20, d21, d22, d23,
                d30, d31, d32, d33
        );

        return true;
    }

    boolean inverse(apeMatrix4 rkInverse) {
        return inverse(rkInverse, eps);
    }


    @NonNull
    @Override
    public String toString() {
        return
                m[0]  + " "  + m[1]  + " " + m[2]  + " " + m[3]  + "\n" +
                m[4]  + " "  + m[5]  + " " + m[6]  + " " + m[7]  + "\n" +
                m[8]  + " "  + m[9]  + " " + m[10] + " " + m[11] + "\n" +
                m[12] + " "  + m[13] + " " + m[14] + " " + m[15] + "\n";
    }

    public float[] getArray() {
        return m.clone();
    }

    public float[] m;

    private static final float eps;

    public static final apeMatrix4 IDENTITY;
    public static final apeMatrix4 ZERO;
}
