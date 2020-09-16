package org.apertusvr;

import androidx.annotation.Size;

public class apeBoxGeometry extends apeGeometry {

    public static class GeometryBoxParameters
    {
        public apeVector3 dimensions;

        public GeometryBoxParameters() {
            dimensions = new apeVector3();
        }

        public GeometryBoxParameters(apeVector3 dimensions) {

            this.dimensions = new apeVector3(dimensions);
        }

        GeometryBoxParameters(@Size(3)float[] paramArray) {
            dimensions = new apeVector3(paramArray[0],paramArray[1],paramArray[2]);
        }
    }

    public static class apeBoxBuilder implements apeBuilder<apeBoxGeometry> {

        @Override
        public apeBoxGeometry build(String name, Type type) {
            if (type == Type.GEOMETRY_BOX) {
                return new apeBoxGeometry(name);
            }

            return null;
        }
    }

    public apeBoxGeometry(String name) {
        super(name, Type.GEOMETRY_BOX);
    }

    public void setParameters(apeVector3 dimensions) {
        ApertusJNI.setBoxGeometryParameters(mName,dimensions.x, dimensions.y,dimensions.z);
    }

    public GeometryBoxParameters getParameters() {
        return new GeometryBoxParameters(ApertusJNI.getBoxGeometryParameters(mName));
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setBoxGeometryParentNode(mName,parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setBoxGeometryMaterial(mName,material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getBoxGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(materialName)];

        return new apeMaterial(materialName,materialType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setBoxGeometryOwner(mName,ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getBoxGeometryOwner(mName);
    }
}
