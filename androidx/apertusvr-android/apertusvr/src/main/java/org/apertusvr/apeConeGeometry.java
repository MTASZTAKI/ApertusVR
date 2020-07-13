package org.apertusvr;

import androidx.annotation.Size;

public class apeConeGeometry extends apeGeometry {

    public class GeometryConeParameters
    {
        public float radius;
        public float height;
        public float tile;
        public apeVector2 numSeg;

        public GeometryConeParameters() {
            radius = 0.0f;
            height = 0.0f;
            tile = 0.0f;
            numSeg = new apeVector2();
        }

        public GeometryConeParameters(float radius, float height, float tile, apeVector2 numSeg) {
            this.radius = radius;
            this.height = height;
            this.tile = tile;
            this.numSeg = new apeVector2(numSeg);
        }

        GeometryConeParameters(@Size(5)float[] paramArray) {
            radius = paramArray[0];
            height = paramArray[1];
            tile = paramArray[2];
            numSeg = new apeVector2(paramArray[3],paramArray[4]);
        }
    }

    public apeConeGeometry(String name) {
        super(name, Type.GEOMETRY_CONE);
    }

    public void setParameters(float radius, float height, float tile, apeVector2 numSeg) {
        ApertusJNI.setConeGeometryParameters(mName,radius,height,tile,numSeg.x, numSeg.y);
    }

    public GeometryConeParameters getParameters() {
        return new GeometryConeParameters(ApertusJNI.getConeGeometryParameters(mName));
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setConeGeometryParentNode(mName,parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setConeGeometryMaterial(mName,material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getConeGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(materialName)];

        return new apeMaterial(materialName,materialType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setConeGeometryOwner(mName,ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getConeGeometryOwner(mName);
    }
}
