package org.apertusvr;

import androidx.annotation.Size;

public class apeTubeGeometry extends apeGeometry {

    public static class GeometryTubeParameters
    {
        public float height;
        public float tile;

        public GeometryTubeParameters() {
            height = 0.0f;
            tile = 0.0f;

        }

        public GeometryTubeParameters(float height, float tile) {
            this.height = height;
            this.tile = tile;

        }

        GeometryTubeParameters(@Size(2)float[] paramArray) {
            height = paramArray[0];
            tile = paramArray[1];
        }
    }

    public static class apeTubeBuilder implements apeBuilder<apeTubeGeometry> {

        @Override
        public apeTubeGeometry build(String name, Type type) {
            if (type == Type.GEOMETRY_TUBE) {
                return new apeTubeGeometry(name);
            }

            return null;
        }
    }

    public apeTubeGeometry(String name) {
        super(name, Type.GEOMETRY_TUBE);
    }

    public void setParameters(float height, float tile) {
        ApertusJNI.setTubeGeometryParameters(mName,height,tile);
    }

    public GeometryTubeParameters getParameters() {
        return new GeometryTubeParameters(ApertusJNI.getTubeGeometryParameters(mName));
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setTubeGeometryParentNode(mName,parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setTubeGeometryMaterial(mName,material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getTubeGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(materialName)];

        return new apeMaterial(materialName,materialType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setTubeGeometryOwner(mName,ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getTubeGeometryOwner(mName);
    }
}
