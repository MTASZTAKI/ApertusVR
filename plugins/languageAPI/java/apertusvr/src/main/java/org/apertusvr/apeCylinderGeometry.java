package org.apertusvr;

import androidx.annotation.Size;

public class apeCylinderGeometry extends apeGeometry {

    public class GeometryCylinderParameters
    {
        public float radius;
        public float height;
        public float tile;

        public GeometryCylinderParameters() {
            radius = 0.0f;
            height = 0.0f;
            tile = 0.0f;
        }

        public GeometryCylinderParameters(float radius, float height, float tile) {
            this.radius = radius;
            this.height = height;
            this.tile = tile;
        }

        GeometryCylinderParameters(@Size(3)float[] paramArray) {
            radius = paramArray[0];
            height = paramArray[1];
            tile = paramArray[2];
        }
    }

    public apeCylinderGeometry(String name) {
        super(name, Type.GEOMETRY_CONE);
    }

    public void setParameters(float radius, float height, float tile) {
        ApertusJNI.setCylinderGeometryParameters(mName,radius,height,tile);
    }

    public GeometryCylinderParameters getParameters() {
        return new GeometryCylinderParameters(ApertusJNI.getCylinderGeometryParameters(mName));
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setCylinderGeometryParentNode(mName,parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setCylinderGeometryMaterial(mName,material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getCylinderGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(materialName)];

        return new apeMaterial(materialName,materialType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setCylinderGeometryOwner(mName,ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getCylinderGeometryOwner(mName);
    }
}
