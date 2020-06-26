package org.apertusvr;

import androidx.annotation.Size;

import org.jetbrains.annotations.NotNull;

public class apePlaneGeometry extends apeGeometry {

    static class GeometryPlaneParameters {

        public GeometryPlaneParameters () {
            numSeg = new apeVector2();
            size = new apeVector2();
            tile = new apeVector2();
        }

        public GeometryPlaneParameters(apeVector2 numSeg, apeVector2 size, apeVector2 tile) {
            this.numSeg = numSeg;
            this.size = size;
            this.tile = tile;
        }

        GeometryPlaneParameters(@Size(6) float[] paramsArray) {
            numSeg = new apeVector2(paramsArray[0],paramsArray[1]);
            size = new apeVector2(paramsArray[2],paramsArray[3]);
            tile = new apeVector2(paramsArray[4],paramsArray[5]);
        }

        @NotNull
        @Override
        public String toString() {
            return "GeometryPlaneParameters{" +
                    "numSeg=" + numSeg +
                    ", size=" + size +
                    ", tile=" + tile +
                    '}';
        }

        public apeVector2 numSeg;
        public apeVector2 size;
        public apeVector2 tile;
    }


    public apePlaneGeometry(String name) {
        super(name, Type.GEOMETRY_PLANE);
    }

    void setParameters(apeVector2 numSeg, apeVector2 size, apeVector2 tile) {
        ApertusJNI.setPlaneGeometryParameters(mName, numSeg.x, numSeg.y, size.x, size.y, tile.x, tile.y);
    }

    public apeVector2 getNumSeg() {
        return new apeVector2(ApertusJNI.getPlaneGeometryNumSeg(mName));
    }

    public apeVector2 getSize() {
        return new apeVector2(ApertusJNI.getPlaneGeometrySize(mName));
    }

    public apeVector2 getTile() {
        return new apeVector2(ApertusJNI.getPlaneGeometryTile(mName));
    }

    public GeometryPlaneParameters getParameters() {
        return new GeometryPlaneParameters(ApertusJNI.getPlaneGeometryParameters(mName));
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setPlaneGeometryParentNode(mName, parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setPlaneGeometryMaterial(mName, material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getPlaneGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(mName)];

        return new apeMaterial(materialName,materialType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setPlaneGeometryOwner(mName, ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getPlaneGeometryOwner(mName);
    }
}
