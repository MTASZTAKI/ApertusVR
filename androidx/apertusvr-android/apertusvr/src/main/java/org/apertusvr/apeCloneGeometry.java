package org.apertusvr;

public class apeCloneGeometry extends apeGeometry {

    public apeCloneGeometry(String name) {
        super(name, Type.GEOMETRY_CLONE);
    }

    public void setSourceGeometry(apeGeometry sourceGeometry) {
        ApertusJNI.setCloneGeometrySourceGeometry(mName, sourceGeometry.getName());
    }

    public void setSourceGeometryGroupName(String groupGeometryName) {
        ApertusJNI.setCloneGeometrySourceGeometryGroupName(mName, groupGeometryName);
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setCloneGeometryParentNode(mName, parentNode.getName());
    }

    public apeGeometry getSourceGeometry() {
        String sourceGeometryName = ApertusJNI.getCloneGeometrySourceGeometryName(mName);
        Type sourceGeometryType = apeEntity.Type.values()[ApertusJNI.getEntityType(sourceGeometryName)];

        return new apeGeometry(sourceGeometryName, sourceGeometryType);
    }

    public String getSourceGeometryName() {
        return ApertusJNI.getCloneGeometrySourceGeometryName(mName);
    }

    public String getSourceGeometryGroupName() {
        return ApertusJNI.getCloneGeometrySourceGeometryGroupName(mName);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setCloneGeometryOwner(mName, ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getCloneGeometryOwner(mName);
    }
}
