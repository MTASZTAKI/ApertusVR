package org.apertusvr.render;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import com.google.android.filament.MaterialInstance;
import com.google.android.filament.Texture;
import com.google.android.filament.TextureSampler;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.Arrays;

public class apeMtlLoader {

    static void loadMtl(String meshName, FileInputStream fis, String texturePath) {
        MtlRecord currentMtl = null;
        InputStreamReader isr = new InputStreamReader(fis);
        BufferedReader fileReader = new BufferedReader(isr);

    }

    private class MtlRecord {

        MtlRecord(String name) {
            mtlName = name;
            Ka = new float[] {0f, 0f, 0f};
            Kd = new float[] {0f, 0f, 0f};
            Ks = new float[] {0f, 0f, 0f};
            Ns = 4;
            illum = 2;
        }

        String mtlName;
        @Size(3) float[] Ka;
        @Size(3) float[] Kd;
        @Size(3) float[] Ks;
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

//    private MaterialInstance mtl2pbr(MtlRecord mtlRecord, String texturePath, boolean textureIsAsset) {
//        MaterialInstance pbrMat;
//        if(mtlRecord.map_Kd != null && !mtlRecord.map_Kd.equals("")) {
//            pbrMat = mTexturedMaterial.createInstance();
//
//            Texture baseColor = null;
//
//            if(textureIsAsset) {
//                baseColor = TextureLoader.loadTextureAsset(
//                        mEngine,
//                        mAssets,
//                        texturePath + "/" + mtlRecord.map_Kd,
//                        TextureLoader.TextureType.COLOR);
//            } else {
//                baseColor = TextureLoader.loadTextureFile(
//                        mEngine,
//                        mContext,
//                        texturePath + "/" + mtlRecord.map_Kd,
//                        TextureLoader.TextureType.COLOR);
//            }
//
//            TextureSampler sampler = new TextureSampler();
//
//            pbrMat.setParameter("albedo", baseColor, sampler);
//
//        } else {
//            pbrMat = mColoredMaterial.createInstance();
//
//            pbrMat.setParameter("albedo",mtlRecord.Kd[0],mtlRecord.Kd[1],mtlRecord.Kd[2]);
//        }
//
//        pbrMat.setParameter("roughness",  (float) Math.exp(-mtlRecord.Ns / 100f)-0.1f);
//        Log.d("javalog",mtlRecord.toString());
//        Log.d("javalog",mtlRecord.mtlName + " roughness: "+(float) Math.exp(-mtlRecord.Ns / 100f));
//
//        if((mtlRecord.Ks[0] > 0.6f && mtlRecord.Ks[1] > 0.6f && mtlRecord.Ks[2] > 0.6f) ||
//                (Math.abs(mtlRecord.Ks[0] - mtlRecord.Ks[1]) > MAT_EPS) && (Math.abs(mtlRecord.Ks[0] - mtlRecord.Ks[1]) > MAT_EPS)) {
//            pbrMat.setParameter("metallic",1.0f);
//        } else {
//            pbrMat.setParameter("metallic",0.0f);
//        }
//
//        return pbrMat;
//    }

}
