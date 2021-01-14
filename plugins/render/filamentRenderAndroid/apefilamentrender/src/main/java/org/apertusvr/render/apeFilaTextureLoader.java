package org.apertusvr.render;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.google.android.filament.Engine;
import com.google.android.filament.Texture;
import com.google.android.filament.android.TextureHelper;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

class apeFilaTextureLoader {

    enum TextureType {
        COLOR,
        NORMAL,
        DATA
    }

    static boolean SKIP_BITMAP_COPY = true;

    public static Texture loadTextureAsset(Engine engine, AssetManager assets, String fileName, TextureType textureType) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPremultiplied = textureType == TextureType.COLOR;

        InputStream is = null;
        try {
            is = assets.open(fileName);
        } catch (IOException e) {
            e.printStackTrace();
        }

        Bitmap bitmap = BitmapFactory.decodeStream(is, null, options);
        assert bitmap != null;

        return loadTexture(engine, bitmap, textureType);
    }

    public static Texture loadTextureFile(Engine engine, Context context, String filePath, TextureType textureType) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPremultiplied = textureType == TextureType.COLOR;

        FileInputStream fis = null;
        try {
            fis = new FileInputStream(filePath);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        Bitmap bitmap = BitmapFactory.decodeStream(fis,null, options);
        assert bitmap != null;

        return loadTexture(engine, bitmap, textureType);
    }

    private static Texture loadTexture(Engine engine, Bitmap bitmap, TextureType textureType) {
        Texture texture = new Texture.Builder()
                .width(bitmap.getWidth())
                .height(bitmap.getHeight())
                .sampler(Texture.Sampler.SAMPLER_2D)
                .format(internalFormat(textureType))
                .levels(0xff) // tells Filament to figure out the number of mip levels
                .build(engine);

        if(SKIP_BITMAP_COPY) {
            TextureHelper.setBitmap(engine, texture, 0, bitmap);
        } else {
            ByteBuffer buffer = ByteBuffer.allocateDirect(bitmap.getByteCount());
            bitmap.copyPixelsToBuffer(buffer);
            buffer.flip();

            Texture.PixelBufferDescriptor descriptor = new Texture.PixelBufferDescriptor(
                    buffer,
                    format(bitmap),
                    type(bitmap)
            );

            texture.setImage(engine, 0, descriptor);
        }

        texture.generateMipmaps(engine);

        return texture;
    }

    static private Texture.InternalFormat internalFormat(TextureType type) {
        Texture.InternalFormat result;
        switch (type) {
            case COLOR:  result = Texture.InternalFormat.SRGB8_A8;
                break;
            case NORMAL:  result = Texture.InternalFormat.RGBA8;
                break;
            case DATA:  result = Texture.InternalFormat.RGBA8;
                break;
            default:
                throw new IllegalStateException("Unexpected value: " + type);
        }

        return result;
    }

    static private Texture.Format format(Bitmap bitmap) {
        Texture.Format result;
        switch (bitmap.getConfig().name()) {
            case "ALPHA_8": result = Texture.Format.ALPHA;
                break;
            case "RGB_565":  result = Texture.Format.RGB;
                break;
            case "ARGB_8888":
            case "RGBA_F16":
                result = Texture.Format.RGBA;
                break;
            default: throw new IllegalArgumentException("Unkown bitmap configuration");
        }

        return result;
    }

    static private Texture.Type type(Bitmap bitmap) {
        Texture.Type result;
        switch (bitmap.getConfig().name()) {
            case "ALPHA_8": result = Texture.Type.USHORT;
                break;
            case "RGB_565": result = Texture.Type.USHORT;
                break;
            case "ARGB_8888": result = Texture.Type.UBYTE;
                break;
            case "RGBA_F16": result = Texture.Type.HALF;
                break;
            default: throw  new IllegalArgumentException("Unsupported bitmap configuration");
        }

        return result;
    }
}
