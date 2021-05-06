package org.apertusvr;

import java.util.stream.IntStream;

public final class apeTextGeometry extends apeGeometry {

    public static class apeTextGeometryBuilder implements apeBuilder<apeTextGeometry> {

        @Override
        public apeTextGeometry build(String name, Type type) {
            if (type == Type.GEOMETRY_TEXT) {
                return new apeTextGeometry(name);
            }

            return null;
        }
    }

    public apeTextGeometry(String name) {
        super(name, apeEntity.Type.GEOMETRY_TEXT);
    }

    public String getCaption() {
        return ApertusJNI.getTextGeometryCaption(mName);
    }

    public void setCaption(String caption) {
        ApertusJNI.setTextGeometryCaption(mName, caption);
    }

    public void clearCaption() {
        ApertusJNI.clearTextGeometryCaption(mName);
    }

    public boolean isVisible() {
        return ApertusJNI.isTextGeometryVisible(mName);
    }

    public void setVisible(boolean enabled) {
        ApertusJNI.setTextGeometryVisible(mName, enabled);
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setTextGeometryParentNode(mName, parentNode.getName());
    }

    public void showOnTop(boolean show) {
        ApertusJNI.showTextGeometryOnTop(mName, show);
    }

    public boolean isShownOnTop() {
        return ApertusJNI.isTextGeometryShownOnTop(mName);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setTextGeometryOwner(mName, ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getTextGeometryOwner(mName);
    }
}
