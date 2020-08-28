package org.apertusvr.render;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import androidx.core.util.Pair;

import com.google.android.filament.Engine;
import com.google.android.filament.IndirectLight;
import com.google.android.filament.Skybox;
import com.google.android.filament.Texture;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

public class apeFilaIblLoader {

    static void destroyIbl(Engine engine, apeFilaIbl ibl) {
        if (ibl == null) return;

        engine.destroySkybox(ibl.skybox);
        engine.destroyTexture(ibl.skyboxTexture);
        engine.destroyIndirectLight(ibl.indirectLight);
        engine.destroyTexture(ibl.indirectLightTexture);
        ibl = null;
    }

    static void loadIblAsset(AssetManager assets, String name, Engine engine, apeFilaIbl target) {
        try {
            Pair<IndirectLight, Texture> iblPair = loadIndirectLightAsset(assets, name, engine);
            Pair<Skybox,Texture> skybPair = loadSkyboxAsseet(assets, name, engine);
            if(target != null && iblPair != null && skybPair != null) {
                //return new apeFilaIbl(iblPair.first, iblPair.second, skybPair.first, skybPair.second);
                target.indirectLight = iblPair.first;
                target.indirectLightTexture = iblPair.second;
                target.skybox = skybPair.first;
                target.skyboxTexture = skybPair.second;
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static private Pair<IndirectLight,Texture> loadIndirectLightAsset(
            AssetManager assets,
            String name,
            Engine engine) throws IOException {
        Pair<Integer,Integer> size = peekAssetSize(assets,name+"/m0_nx.rgb32f");

        if(size.first != null && size.second != null) {
            int w = size.first;
            int h = size.second;
            Texture.Builder builder = new Texture.Builder()
                    .width(w)
                    .height(h)
                    .levels((int) (Math.log((float) w) / Math.log(2.0f)) + 1)
                    .format(Texture.InternalFormat.R11F_G11F_B10F)
                    .sampler(Texture.Sampler.SAMPLER_CUBEMAP);
            Texture texture = builder.build(engine);

            for (int i = 0; i < texture.getLevels(); i++) {
                String prefix = "m" + i + "_";
                if (!loadCubeMapAsset(texture, assets, name, engine, prefix, i)) break;
            }


            IndirectLight.Builder ilBuilder = new IndirectLight.Builder();
            ilBuilder.reflections(texture).intensity(30000f); //??

            IndirectLight il = ilBuilder.build(engine);
            return new Pair<IndirectLight, Texture>(il,texture);
        }
        else {
            return null;
        }
    }

    static private Pair<Integer, Integer> peekAssetSize(AssetManager assets, String name) throws IOException {
        InputStream input = assets.open(name);
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inJustDecodeBounds = true;
        BitmapFactory.decodeStream(input, null, opts);
        return new Pair<Integer, Integer>(opts.outWidth,opts.outHeight);
    }

    static private Pair<Skybox, Texture> loadSkyboxAsseet(AssetManager assets, String name, Engine engine) throws IOException {
        Pair<Integer, Integer> size = peekAssetSize(assets, name+"/nx.rgb32f");
        Texture.Builder textureBuilder = new Texture.Builder();
        if(size.first != null && size.second != null) {
            textureBuilder
                    .width(size.first)
                    .height(size.second)
                    .levels(1)
                    .format(Texture.InternalFormat.R11F_G11F_B10F)
                    .sampler(Texture.Sampler.SAMPLER_CUBEMAP);
            Texture texture = textureBuilder.build(engine);

            loadCubeMapAsset(texture, assets, name, engine, "", 0);

            Skybox.Builder skyboxBuilder = new Skybox.Builder();
            skyboxBuilder.environment(texture);
            Skybox skybox = skyboxBuilder.build(engine);

            return new Pair<Skybox, Texture>(skybox, texture);
        }

        return null;
    }

    static private boolean loadCubeMapAsset(
            Texture texture,
            AssetManager assets,
            String name,
            Engine engine,
            String prefix,
            int level
    ) {
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inPremultiplied = false;

        // R11G11B10F is always 4 bytes per pixel
        int faceSize = texture.getWidth(level) * texture.getHeight(level) * 4;
        int[] offsets = new int[6];
        for(int i = 0; i < 6; i++) offsets[i] = i * faceSize;

        // Allocate enough memory for all the cubemap faces
        ByteBuffer storage = ByteBuffer.allocateDirect(faceSize * 6);

        String[] suffices = new String[] {"px","nx","py","ny","pz","nz"};

        for (String suffix: suffices) {
            try {
                InputStream input = assets.open(name + "/" + prefix + suffix + ".rgb32f");
                Bitmap bitmap = BitmapFactory.decodeStream(input,null,opts);
                if(bitmap != null) bitmap.copyPixelsToBuffer(storage);
            } catch (IOException e) {
                return false;
            }
        }

        storage.flip();

        Texture.PixelBufferDescriptor buffer = new Texture.PixelBufferDescriptor(storage, Texture.Format.RGB, Texture.Type.UINT_10F_11F_11F_REV);
        texture.setImage(engine, level, buffer, offsets);

        return true;
    }

}
