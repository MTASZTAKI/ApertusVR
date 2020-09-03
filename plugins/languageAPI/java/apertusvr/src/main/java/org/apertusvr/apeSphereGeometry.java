package org.apertusvr;

import androidx.annotation.Size;

public class apeSphereGeometry extends apeGeometry {

    public class GeometrySphereParameters
    {
        public float radius;

        public apeVector2 tile;

        public GeometrySphereParameters() {
            radius = 0.0f;
            tile = new apeVector2();
        }

        public GeometrySphereParameters(float radius, apeVector2 tile) {
            this.radius = radius;
            this.tile = new apeVector2(tile);
        }

        GeometrySphereParameters(@Size(3)float[] paramArray) {
            radius = paramArray[0];
            this.tile = new apeVector2(paramArray[1],paramArray[2]);
        }
    }

    public apeSphereGeometry(String name) {
        super(name, Type.GEOMETRY_SPHERE);
    }

    public void setParameters(float radius,  apeVector2 tile) {
        ApertusJNI.setSphereGeometryParameters(mName,radius,tile.x,tile.y);
    }

    public GeometrySphereParameters getParameters() {
        return new GeometrySphereParameters(ApertusJNI.getSphereGeometryParameters(mName));
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setSphereGeometryParentNode(mName,parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setSphereGeometryMaterial(mName,material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getSphereGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(materialName)];

        return new apeMaterial(materialName,materialType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setSphereGeometryOwner(mName,ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getSphereGeometryOwner(mName);
    }
}
