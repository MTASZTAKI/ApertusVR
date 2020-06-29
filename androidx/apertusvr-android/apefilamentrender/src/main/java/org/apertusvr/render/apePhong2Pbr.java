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

    public static float specularExponent2roughness(float Ns) {
        return (float) Math.exp(-Ns / 100f)-0.1f;
    }

    private static final float MAT_EPS = 1e-6f;
}
