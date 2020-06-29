package org.apertusvr.render;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.google.android.filament.Material;
import com.google.android.filament.MaterialInstance;
import com.google.android.filament.Texture;
import com.google.android.filament.TextureSampler;

import org.apertusvr.apeColor;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.Map;

final class apeFilaMtlLoader {

    private Material mColoredMaterial;
    private Material mTexturedMaterial;
    private apeFilamentRenderPlugin mOwner;

    apeFilaMtlLoader(Material coloredMaterial, Material texturedMaterial, apeFilamentRenderPlugin owner) {
        mColoredMaterial = coloredMaterial;
        mTexturedMaterial = texturedMaterial;
        mOwner = owner;
    }

    void loadMtl(
            String meshName, FileInputStream fis, String texturePath,
            Map<String, MaterialInstance> materialInstances) throws IOException {
        InputStreamReader isr = new InputStreamReader(fis);
        BufferedReader fileReader = new BufferedReader(isr);

        MtlRecord currentMtl = null;
        while (fileReader.ready()) {
            String line = fileReader.readLine();

            if (line.contains("newmtl ")) {
                String mtlName = line.substring(7);
                if (currentMtl == null) {
                    currentMtl = new MtlRecord(mtlName);
                }
                else {
                    MaterialInstance pbrMat = mtl2pbr(currentMtl, texturePath, false);
                    materialInstances.put(meshName + currentMtl.mtlName, pbrMat);
                    currentMtl = new MtlRecord(mtlName);
                }
            }
            else if (line.contains("map_Kd ")) {
                assert currentMtl != null;
                currentMtl.map_Kd = line.substring(7);
            }
            else if (line.contains("Ka ") && !line.contains("map_Ka")) {
                String[] vals = line.substring(3).split(" ");
                float Kar = Float.parseFloat(vals[0]);
                float Kag = Float.parseFloat(vals[1]);
                float Kab = Float.parseFloat(vals[2]);
                assert currentMtl != null;
                currentMtl.Ka = new float[]{Kar, Kag, Kab};
            }
            else if (line.contains("Kd ")) {
                String[] vals = line.substring(3).split(" ");
                float Kdr = Float.parseFloat(vals[0]);
                float Kdg = Float.parseFloat(vals[1]);
                float Kdb = Float.parseFloat(vals[2]);
                assert currentMtl != null;
                currentMtl.Kd = new float[]{Kdr, Kdg, Kdb};
            }
            else if (line.contains("Ks ") && !line.contains("map_Ks")) {
                String[] vals = line.substring(3).split(" ");
                float Ksr = Float.parseFloat(vals[0]);
                float Ksg = Float.parseFloat(vals[1]);
                float Ksb = Float.parseFloat(vals[2]);
                assert currentMtl != null;
                currentMtl.Ks = new float[]{Ksr, Ksg, Ksb};
            }
            else if (line.contains("illum ")) {
                String val = line.substring(6);
                assert currentMtl != null;
                currentMtl.illum = Integer.parseInt(val);
            }
            else if (line.contains("Ns ") && !line.contains("map_Ns")) {
                String val = line.substring(3);
                assert currentMtl != null;
                currentMtl.Ns = Integer.parseInt(val);
            }
        }

        assert currentMtl != null;
        MaterialInstance pbrMat = mtl2pbr(currentMtl, texturePath, false);
        materialInstances.put(meshName + currentMtl.mtlName, pbrMat);
    }

    private static class MtlRecord {

        MtlRecord(String name) {
            mtlName = name;
            Ka = new float[]{0f, 0f, 0f};
            Kd = new float[]{0f, 0f, 0f};
            Ks = new float[]{0f, 0f, 0f};
            Ns = 4;
            illum = 2;
        }

        String mtlName;
        @Size(3)
        float[] Ka;
        @Size(3)
        float[] Kd;
        @Size(3)
        float[] Ks;
        int illum;
        String map_Kd;
        int Ns;

        @NonNull
        @Override
        public String toString() {
            return "name " + mtlName + "\n" +
                    "Ka " + Arrays.toString(Ka) + "\n" +
                    "Kd " + Arrays.toString(Kd) + "\n" +
                    "Ks " + Arrays.toString(Ks) + "\n" +
                    "illum " + illum + "\n" +
                    "map_Kd" + map_Kd + "\n" +
                    "Ns " + Ns;
        }
    }

//    private static class PbrRecord {
//        PbrRecord() {}
//
//        @Size(3) float[] albedo;
//        float metallic;
//        float roughness;
//    }

    private MaterialInstance mtl2pbr(MtlRecord mtlRecord, String texturePath, boolean textureIsAsset) {
        MaterialInstance pbrMat;

        if (mtlRecord.map_Kd != null && !mtlRecord.map_Kd.equals("")) {
            pbrMat = mTexturedMaterial.createInstance();

            Texture baseColor = null;

            if (textureIsAsset) {
                baseColor = apeFilaTextureLoader.loadTextureAsset(
                        mOwner.getEngine(),
                        mOwner.getAssets(),
                        texturePath + "/" + mtlRecord.map_Kd,
                        apeFilaTextureLoader.TextureType.COLOR);
            }
            else {
                baseColor = apeFilaTextureLoader.loadTextureFile(
                        mOwner.getEngine(),
                        mOwner.getContext(),
                        texturePath + "/" + mtlRecord.map_Kd,
                        apeFilaTextureLoader.TextureType.COLOR);
            }

            TextureSampler sampler = new TextureSampler();
            pbrMat.setParameter("albedo", baseColor, sampler);

        }
        else {
            pbrMat = mColoredMaterial.createInstance();
            pbrMat.setParameter("albedo", mtlRecord.Kd[0], mtlRecord.Kd[1], mtlRecord.Kd[2]);
        }

        pbrMat.setParameter("roughness",
                apePhong2Pbr.specularExponent2roughness(mtlRecord.Ns));

        final float MAT_EPS = 1e-6f;

        pbrMat.setParameter("metallic",
                apePhong2Pbr.specular2metallic(
                        new apeColor(mtlRecord.Ks[0], mtlRecord.Ks[1], mtlRecord.Ks[2])));

        return pbrMat;
    }
}
