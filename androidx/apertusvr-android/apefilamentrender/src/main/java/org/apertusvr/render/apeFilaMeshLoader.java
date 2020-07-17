package org.apertusvr.render;

import android.content.res.AssetManager;
import android.util.Log;

import com.google.android.filament.Box;
import com.google.android.filament.Engine;
import com.google.android.filament.Entity;
import com.google.android.filament.EntityManager;
import com.google.android.filament.IndexBuffer;
import com.google.android.filament.MaterialInstance;
import com.google.android.filament.RenderableManager;
import com.google.android.filament.VertexBuffer;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.Channels;
import java.nio.channels.ReadableByteChannel;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;

final class apeFilaMeshLoader {
    static final private long MAX_UINT32 = 4294967295L;

    static final private long HEADER_FLAG_INTERLEAVED = 0x1L;
    static final private long HEADER_FLAG_SNORM16_UV  = 0x2L;
    static final private long HEADER_FLAG_COMPRESSED  = 0x4L;

    static void destroyMesh(Engine engine, apeFilaMesh mesh) {
        engine.destroyEntity(mesh.renderable);
        engine.destroyIndexBuffer(mesh.indexBuffer);
        engine.destroyVertexBuffer(mesh.vertexBuffer);
        EntityManager.get().destroy(mesh.renderable);
    }

    static void loadMesh(InputStream input, String name,
                         Map<String, MaterialInstance> materials,
                         Engine engine, apeFilaMesh target,
                         String defaultMatName) {

        // InputStream input;
        Header header;
        try {
            // input  = assets.open(name);
            header = readHeader(input);
            ReadableByteChannel channel = Channels.newChannel(input);

            ByteBuffer vertexBufferData = readSizedData(channel, header.verticesSizeInBytes);
            ByteBuffer indexBufferData = readSizedData(channel, header.indicesSizeInBytes);

            List<Part> parts = readParts(header, input);
            List<String> definedMaterials = readMaterials(input);

            IndexBuffer indexBuffer = createIndexBuffer(engine, header, indexBufferData);
            VertexBuffer vertexBuffer = createVertexBuffer(engine, header, vertexBufferData);

            @Entity int renderableEntity = createRenderable(
                    name, engine, header.aabb, indexBuffer,
                    vertexBuffer, parts, definedMaterials,
                    materials, defaultMatName);

            target.renderable = renderableEntity;
            target.indexBuffer = indexBuffer;
            target.vertexBuffer = vertexBuffer;
            target.aabb = header.aabb;
            target.parts = parts;
            target.definedMaterials = definedMaterials;
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }

    static void loadMesh(InputStream input, String name,
                         MaterialInstance material,
                         Engine engine, apeFilaMesh target,
                         String matName) {
        loadMesh(input, name, Collections.singletonMap(matName,material),engine,target,matName);
    }

    static void cloneMesh(String sourceMeshName, apeFilaMesh sourceMesh, apeFilaMeshClone target,
                          Map<String, MaterialInstance> materials,
                          Engine engine, String defaultMaterialName) {
        target.sourceMeshName = sourceMeshName;
        target.renderable = createRenderable(
                sourceMeshName,engine,sourceMesh.aabb,
                sourceMesh.indexBuffer,sourceMesh.vertexBuffer,
                sourceMesh.parts, sourceMesh.definedMaterials,
                materials, defaultMaterialName);
    }

    static void destroyClone(Engine engine, apeFilaMeshClone meshClone) {
        engine.destroyEntity(meshClone.renderable);
        EntityManager.get().destroy(meshClone.renderable);
    }

    private static Header readHeader(InputStream input) throws IOException {
        Header header = new Header();

        if(!readMagicNumber(input)) {
            Log.e("Filament","Invalid filamesh file");
        }

        header.versionNumber = readUIntLE(input);
        header.parts = readUIntLE(input);
        header.aabb = new Box(
                readFloat32LE(input), readFloat32LE(input), readFloat32LE(input),
                readFloat32LE(input), readFloat32LE(input), readFloat32LE(input));
        header.flags = readUIntLE(input);
        header.posOffset = readUIntLE(input);
        header.positionStride = readUIntLE(input);
        header.tangentOffset = readUIntLE(input);
        header.tangentStride = readUIntLE(input);
        header.colorOffset = readUIntLE(input);
        header.colorStride = readUIntLE(input);
        header.uv0Offset = readUIntLE(input);
        header.uv0Stride = readUIntLE(input);
        header.uv1Offset = readUIntLE(input);
        header.uv1Stride = readUIntLE(input);
        header.totalVertices = readUIntLE(input);
        header.verticesSizeInBytes = readUIntLE(input);
        header.indices16Bit = readUIntLE(input);
        header.totalIndices = readUIntLE(input);
        header.indicesSizeInBytes = readUIntLE(input);

        header.valid = true;
        return header;
    }


    private static ByteBuffer readSizedData(ReadableByteChannel channel, long sizeInBytes) {
        ByteBuffer buffer = ByteBuffer.allocateDirect((int)sizeInBytes);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        try {
            channel.read(buffer);
            buffer.flip();
        }
        catch (IOException e) {
            e.printStackTrace();
        }

        return buffer;
    }

    private static int readIntLE(InputStream input) throws IOException {
        return
                (input.read() & 0xff) | (
                        (input.read() & 0xff) << 8) | (
                        (input.read() & 0xff) << 16) | (
                        (input.read() & 0xff) << 24);
    }

    private static long readUIntLE(InputStream input) throws IOException {
        return ((long)readIntLE(input)) & 0xFFFFFFFFL;
    }

    private static float readFloat32LE(InputStream input) throws IOException {
        byte[] bytes = new byte[4];
        final int read = input.read(bytes, 0, 4);
        return ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN).getFloat();
    }

    private static List<Part> readParts(Header header, InputStream input) throws IOException {
        ArrayList<Part> result = new ArrayList<Part>();
        int size = (int)header.parts;
        for (int i = 0; i < size; i++) {
            Part p = new Part();
            p.offset = readUIntLE(input);
            p.indexCount = readUIntLE(input);
            p.minIndex = readUIntLE(input);
            p.maxIndex = readUIntLE(input);
            p.materialID = readUIntLE(input);
            p.aabb = new Box(
                    readFloat32LE(input), readFloat32LE(input), readFloat32LE(input),
                    readFloat32LE(input), readFloat32LE(input), readFloat32LE(input));

            result.add(p);
        }

        return result;
    }

    private static boolean readMagicNumber(InputStream input) throws IOException {
        String FILAMESH_FILE_IDENTIFIER = "FILAMESH";
        byte[] tmp = new byte[FILAMESH_FILE_IDENTIFIER.length()];
        final int read = input.read(tmp);
        String tmpS = new String(tmp, StandardCharsets.UTF_8); //!!
        return tmpS.equals(FILAMESH_FILE_IDENTIFIER); //!!
    }

    private static List<String> readMaterials(InputStream input) throws IOException {
        ArrayList<String> result = new ArrayList<String>();
        final int size = (int)readUIntLE(input);
        for (int i = 0; i < size; i++) {
            byte[] data = new byte[(int)readUIntLE(input)];
            final int rd = input.read(data);

            // skip null terminator
            final long skp = input.skip(1);

            String dataS = new String(data,StandardCharsets.UTF_8);
            result.add(dataS);
        }

        return result;
    }

    private static IndexBuffer createIndexBuffer(Engine engine, Header header, ByteBuffer data) {
        IndexBuffer.Builder.IndexType indexType =
                header.indices16Bit != 0L ?
                        IndexBuffer.Builder.IndexType.USHORT :
                        IndexBuffer.Builder.IndexType.UINT;

        IndexBuffer result = new IndexBuffer.Builder()
                .bufferType(indexType)
                .indexCount((int)header.totalIndices)
                .build(engine);
        result.setBuffer(engine, data);

        return result;
    }

    private static boolean uvNormalized(Header header) {
        return (header.flags & HEADER_FLAG_SNORM16_UV) != 0L;
    }

    private static VertexBuffer createVertexBuffer(Engine engine, Header header, ByteBuffer data) {
        VertexBuffer.AttributeType uvType =
                !uvNormalized(header) ?
                        VertexBuffer.AttributeType.HALF2 :
                        VertexBuffer.AttributeType.SHORT2;

        VertexBuffer.Builder vbBuilder = new VertexBuffer.Builder()
                .bufferCount(1)
                .vertexCount((int)header.totalVertices)
                .normalized(VertexBuffer.VertexAttribute.COLOR)
                .normalized(VertexBuffer.VertexAttribute.TANGENTS)
                .attribute(VertexBuffer.VertexAttribute.POSITION, 0, VertexBuffer.AttributeType.HALF4, (int)header.posOffset,(int)header.positionStride)
                .attribute(VertexBuffer.VertexAttribute.TANGENTS,0, VertexBuffer.AttributeType.SHORT4, (int)header.tangentOffset,(int)header.tangentStride)
                .attribute(VertexBuffer.VertexAttribute.COLOR, 0, VertexBuffer.AttributeType.UBYTE4, (int)header.colorOffset, (int)header.colorStride)
                .attribute(VertexBuffer.VertexAttribute.UV0,0, uvType, (int)header.uv0Offset,(int)header.uv0Stride)
                .normalized(VertexBuffer.VertexAttribute.UV0,uvNormalized(header));

        if(header.uv1Offset != MAX_UINT32 && header.uv1Stride != MAX_UINT32) {
            vbBuilder
                    .attribute(VertexBuffer.VertexAttribute.UV1, 0, uvType, (int)header.uv1Offset, (int)header.uv1Stride)
                    .normalized(VertexBuffer.VertexAttribute.UV1, uvNormalized(header));
        }

        VertexBuffer result =  vbBuilder.build(engine);
        result.setBufferAt(engine, 0, data);
        return result;
    }

    private static int createRenderable(
            String name,
            Engine engine,
            Box aabb,
            IndexBuffer indexBuffer,
            VertexBuffer vertexBuffer,
            List<Part> parts,
            List<String> definedMaterials,
            Map<String,MaterialInstance> materials,
            String defaultMatName) {

        RenderableManager.Builder builder = new RenderableManager.Builder(parts.size());
        builder.boundingBox(aabb);

        for (int i = 0; i < parts.size(); i++) {
            builder.geometry(i,
                    RenderableManager.PrimitiveType.TRIANGLES,
                    vertexBuffer,
                    indexBuffer,
                    (int)parts.get(i).offset,
                    (int)parts.get(i).minIndex,
                    (int)parts.get(i).maxIndex,
                    (int)parts.get(i).indexCount);

            String matS = definedMaterials.get((int)parts.get(i).materialID);
            MaterialInstance material = materials.get(name + matS);
            if(material != null) {
                builder.material(i, material);
            }
            else {
                builder.material(i, Objects.requireNonNull(materials.get(defaultMatName)));
            }
        }

        @Entity int result =  EntityManager.get().create();
        builder.build(engine, result);
        return result;
    }

    private static class Header {
        boolean valid = false;
        long versionNumber = 0L;
        long parts = 0L;
        Box aabb = new Box();
        long flags = 0L;
        long posOffset = 0L;
        long positionStride = 0L;
        long tangentOffset = 0L;
        long tangentStride = 0L;
        long colorOffset = 0L;
        long colorStride = 0L;
        long uv0Offset = 0L;
        long uv0Stride = 0L;
        long uv1Offset = 0L;
        long uv1Stride = 0L;
        long totalVertices = 0L;
        long verticesSizeInBytes = 0L;
        long indices16Bit = 0L;
        long totalIndices = 0L;
        long indicesSizeInBytes = 0L;
    }

    protected static class Part {
        long offset = 0L;
        long indexCount = 0L;
        long minIndex = 0L;
        long maxIndex = 0L;
        long materialID = 0L;
        Box aabb = new Box();
    }
}
