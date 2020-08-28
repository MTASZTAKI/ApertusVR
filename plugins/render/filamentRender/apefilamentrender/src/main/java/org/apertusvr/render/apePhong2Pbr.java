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

import org.apertusvr.apeColor;

public final class apePhong2Pbr {
    private apePhong2Pbr() {}

    public static float specular2metallic(apeColor specular) {
        if (specular.r > 0.6f && specular.g > 0.6f && specular.b > 0.6f ||
                ((Math.abs(specular.r - specular.g) > MAT_EPS) &&
                        Math.abs(specular.g - specular.b) > MAT_EPS)) {
            return 1.0f;
        }
        else {
            return 0.0f;
        }
    }

    private static float clamp(float x, float a, float b) {
        if (x < a) return a;
        else if (x > b) return b;
        else return x;
    }

    public static float specularExponent2roughness(float Ns) {
        return (float) Math.exp(-clamp(Ns,0f,1024f) / 100f)-0.1f;
    }

    public static float lightSpecular2intensity(apeColor specular) {
        float x = specular.a;
        return (float) Math.pow(10, 6 * Math.log10(1 + 9 * x));
    }

    private static final float MAT_EPS = 1e-6f;
}
