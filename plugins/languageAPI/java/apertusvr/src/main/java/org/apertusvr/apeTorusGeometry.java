package org.apertusvr;

import androidx.annotation.Size;

public class apeTorusGeometry extends apeGeometry {

    public static class GeometryTorusParameters
    {
        public float radius;
        public float sectionRadius;
        public apeVector2 tile;

        public GeometryTorusParameters() {
            radius = 0.0f;
            sectionRadius = 0.0f;
            tile = new apeVector2();
        }

        public GeometryTorusParameters(float radius, float sectionRadius, apeVector2 tile) {
            this.radius = radius;
            this.sectionRadius = sectionRadius;
            this.tile = new apeVector2(tile);
        }

        GeometryTorusParameters(@Size(4)float[] paramArray) {
            radius = paramArray[0];
            sectionRadius = paramArray[1];
            tile = new apeVector2(paramArray[2],paramArray[3]);
        }
    }

    public static class apeTorusBuilder implements apeBuilder<apeTorusGeometry> {

        @Override
        public apeTorusGeometry build(String name, Type type) {
            if (type == Type.GEOMETRY_TORUS) {
                return new apeTorusGeometry(name);
            }

            return null;
        }
    }

    public apeTorusGeometry(String name) {
        super(name, Type.GEOMETRY_TORUS);
    }

    public void setParameters(float radius, float sectionRadius, apeVector2 tile) {
        ApertusJNI.setTorusGeometryParameters(mName,radius,sectionRadius,tile.x, tile.y);
    }

    public GeometryTorusParameters getParameters() {
        return new GeometryTorusParameters(ApertusJNI.getTorusGeometryParameters(mName));
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setTorusGeometryParentNode(mName,parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setTorusGeometryMaterial(mName,material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getTorusGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(materialName)];

        return new apeMaterial(materialName,materialType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setTorusGeometryOwner(mName,ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getTorusGeometryOwner(mName);
    }
}
