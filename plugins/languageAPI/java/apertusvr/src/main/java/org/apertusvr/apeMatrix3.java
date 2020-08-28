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

public final class apeMatrix3 {

    static {
        IDENTITY = new apeMatrix3(
                1f, 0f, 0f,
                0f, 1f, 0f,
                0f, 0f, 1f
        );

        ZERO = new apeMatrix3(
                0f, 0f, 0f,
                0f, 0f, 0f,
                0f, 0f, 0f
        );

        eps = 1e-08f;
    }

    public apeMatrix3() {
//        m = new float[] {
//                1f, 0f, 0f,
//                0f, 1f, 0f,
//                0f, 0f, 1f
//        };

        m = null;
    }

    public apeMatrix3(float m00, float m01, float m02,
                      float m10, float m11, float m12,
                      float m20, float m21, float m22) {
        m = new float[] {
                m00, m01, m02,
                m10, m11, m12,
                m20, m21, m22
        };
    }

    public apeMatrix3(final apeQuaternion Q) {
        Q.toRotationalMatrix(this);
    }

    public apeMatrix3(@Size(9) float[] m) {
        this.m = m.clone();
    }

    public apeMatrix3(final apeMatrix3 m3x3) {
        this.m = m3x3.m.clone();
    }

    public apeMatrix3 add(final apeMatrix3 rkM) {
        return new apeMatrix3(
                m[0] + rkM.m[0], m[1] + rkM.m[1], m[2] + rkM.m[2],
                m[3] + rkM.m[3], m[4] + rkM.m[4], m[5] + rkM.m[5],
                m[6] + rkM.m[6], m[7] + rkM.m[7], m[8] + rkM.m[8]
                );
    }

    public apeMatrix3 subtract(final apeMatrix3 rkM) {
        return new apeMatrix3(
                m[0] - rkM.m[0], m[1] - rkM.m[1], m[2] - rkM.m[2],
                m[3] - rkM.m[3], m[4] - rkM.m[4], m[5] - rkM.m[5],
                m[6] - rkM.m[6], m[7] - rkM.m[7], m[8] - rkM.m[8]
        );
    }

    public apeMatrix3 multiply(final apeMatrix3 rkM) {
        return new apeMatrix3(
                m[0] * rkM.m[0] + m[1] * rkM.m[3] + m[2] * rkM.m[6],
                m[0] * rkM.m[1] + m[1] * rkM.m[4] + m[2] * rkM.m[7],
                m[0] * rkM.m[2] + m[1] * rkM.m[5] + m[2] * rkM.m[8],

                m[3] * rkM.m[0] + m[4] * rkM.m[3] + m[5] * rkM.m[6],
                m[3] * rkM.m[1] + m[4] * rkM.m[4] + m[5] * rkM.m[7],
                m[3] * rkM.m[2] + m[4] * rkM.m[5] + m[5] * rkM.m[8],

                m[6] * rkM.m[0] + m[7] * rkM.m[3] + m[8] * rkM.m[6],
                m[6] * rkM.m[1] + m[7] * rkM.m[4] + m[8] * rkM.m[7],
                m[6] * rkM.m[2] + m[7] * rkM.m[5] + m[8] * rkM.m[8]
        );
    }

    public apeVector3 apply(final apeVector3 rkV) {
        return new apeVector3(
                m[0] * rkV.x + m[1] * rkV.y + m[2] * rkV.z,
                m[3] * rkV.x + m[4] * rkV.y + m[5] * rkV.z,
                m[6] * rkV.x + m[7] * rkV.y + m[8] * rkV.z
        );
    }

    public float determinant() {
        float cofact0 = m[4] * m[8] - m[7] * m[5];
        float cofact1 = m[3] * m[8] - m[6] * m[5];
        float cofact2 = m[3] * m[7] - m[6] * m[4];

        return m[0] * cofact0 - m[1] * cofact1 + m[2] * cofact2;
    }

    public boolean inverse(apeMatrix3 rkInverse, float tolerance) {
        float cof00 =  m[4] * m[8] - m[7] * m[5];
        float cof01 = -m[3] * m[8] + m[6] * m[5];
        float cof02 =  m[3] * m[7] - m[6] * m[4];

        float cof10 = -m[1] * m[8] + m[7] * m[2];
        float cof11 =  m[0] * m[8] - m[6] * m[2];
        float cof12 = -m[0] * m[7] + m[6] * m[1];

        float cof20 =  m[1] * m[5] - m[4] * m[2];
        float cof21 = -m[0] * m[5] + m[3] * m[2];
        float cof22 =  m[0] * m[4] - m[3] * m[1];

        float det = m[0] * cof00 + m[1] * cof01 + m[2] * cof02;

        if(Math.abs(det) < tolerance) return false;

        float invDet = 1f / det;

        rkInverse = new apeMatrix3(
                cof00 * invDet, cof10 * invDet, cof20 * invDet,
                cof01 * invDet, cof11 * invDet, cof21 * invDet,
                cof02 * invDet, cof12 * invDet, cof22 * invDet
        );

        return true;
    }

    public boolean inverse(apeMatrix3 rkInverse) {
        return inverse(rkInverse, eps);
    }


    public float get(int i, int j) {
        return m[j + 3*i];
    }

    public void set(int i, int j, float val) {
        m[j + 3*i] = val;
    }

    public @Size(3) float[] getRow(int i) {
        int k = 3*i;
        return new float[]{m[k],m[k+1],m[k+2]};
    }

    public @Size(3) float[] getColumn(int j) {
        return new float[]{m[j],m[j+3],m[j+6]};
    }

    public float trace() {
        return m[0] + m[4] + m[8];
    }

    public boolean equals(final apeMatrix3 rkM, float tolerance) {
        for (int i = 0; i < 9; ++i) {
            if(Math.abs(m[i] - rkM.m[i]) > tolerance) return false;
        }

        return true;
    }

    public boolean equals(final apeMatrix3 rkM) {
        return equals(rkM, eps);
    }

    public apeMatrix3 transpose() {
        return new apeMatrix3(
                m[0],m[3],m[6],
                m[1],m[4],m[7],
                m[2],m[5],m[8]
                );
    }



    public @Size(9) float[] m;

    private static final float eps;

    public static final apeMatrix3 IDENTITY;
    public static final apeMatrix3 ZERO;
}
