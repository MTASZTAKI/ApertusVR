/*MIT License
Copyright (c) 2018 MTA SZTAKI
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

package org.apertusvr;

import java.util.Objects;

public final class apeNode {

    enum TransformationSpace {
        LOCAL,
        PARENT,
        WORLD,
        INVALID
    }

    public apeNode(String name) {
        mName = name;
    }

    public String getName() {
        return mName;
    }

    public apeVector3 getPosition() {
        float[] position = ApertusJNI.getNodePosition(mName);
        return new apeVector3(position);
    }

    public apeVector3 getDerivedPosition() {
        float[] derivedPositioin = ApertusJNI.getNodeDerivedPosition(mName);
        return new apeVector3(derivedPositioin);
    }

    public apeQuaternion getOrientation() {
        return new apeQuaternion(ApertusJNI.getNodeOrientation(mName));
    }

    public apeQuaternion getDerivedOrientation() {
        return new apeQuaternion(ApertusJNI.getNodeDerivedOrientation(mName));
    }

    public apeVector3 getScale() {
        return new apeVector3(ApertusJNI.getNodeScale(mName));
    }

    public apeVector3 getDerivedScale() {
        return new apeVector3(ApertusJNI.getNodeDerivedScale(mName));
    }

//    public apeMatrix4 getModelMatrix() {
//        return new apeMatrix4(ApertusJNI.getNodeModelMatrix(mName));
//    }
//
//    public apeMatrix4 getDerivedModelMatrix() {
//        return new apeMatrix4(ApertusJNI.getNodeDerivedModelMatrix(mName));
//    }

    public boolean getChildrenVisibility() {
        return ApertusJNI.getNodeChildrenVisibility(mName);
    }

    public boolean isVisible() {
        return ApertusJNI.isNodeVisible(mName);
    }

    public boolean isFixedYaw() {
        return ApertusJNI.isNodeFixedYaw(mName);
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setNodeParentNode(mName,parentNode.getName());
    }

    public apeNode getParentNode() {
        return new apeNode(ApertusJNI.getNodeParentNode(mName));
    }

    public apeNode[] getChildNodes() {
        return ApertusJNI.getNodeChildNodes(mName);
    }

    public void detachFromParentNode() {
        ApertusJNI.detachNodeFromParentNode(mName);
    }

    public boolean hasChildNode() {
        return ApertusJNI.hasNodeChildNodes(mName);
    }

    public boolean isChildNode(apeNode childNode) {
        return ApertusJNI.isNodeChildNode(mName, childNode.getName());
    }

    public void setPosition(apeVector3 position) {
        ApertusJNI.setNodePosition(mName,position.x,position.y,position.z);
    }

    public void setOrientation(apeQuaternion orientation) {
        ApertusJNI.setNodeOrientation(mName,orientation.w, orientation.x, orientation.y, orientation.z);
    }

    public void setScale(apeVector3 scale) {
        ApertusJNI.setNodeScale(mName,scale.x,scale.y,scale.z);
    }

    public void translate(apeVector3 transformVector, apeNode.TransformationSpace nodeTransformSpace) {
        ApertusJNI.translateNode(mName,transformVector.x,transformVector.y,transformVector.z,nodeTransformSpace.ordinal());
    }

    public void rotate(float angle, apeVector3 axis, apeNode.TransformationSpace nodeTransformSpace) {
        ApertusJNI.rotateNode(mName, angle, axis.x, axis.y, axis.z, nodeTransformSpace.ordinal());
    }

    public void setChildrenVisibility(boolean visible) {
        ApertusJNI.setNodeChildrenVisibility(mName, visible);
    }

    public void setFixedYaw(boolean fix) {
        ApertusJNI.setNodeFixedYaw(mName,fix);
    }

    public void showBoundingBox(boolean show) {
        ApertusJNI.showNodeBoundingBox(mName,show);
    }

    public void setInheritOrientation(boolean enable) {
        ApertusJNI.setNodeInheritOrientation(mName,enable);
    }

    public boolean isInheritOrientation() {
        return ApertusJNI.isNodeInheritOrientation(mName);
    }

    public void setInitalState() {
        ApertusJNI.setNodeInitialState(mName);
    }

    public boolean isReplicated() {
        return ApertusJNI.isNodeReplicated(mName);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setNodeOwner(mName,ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getNodeOwner(mName);
    }

    public String getCreator() {
        return ApertusJNI.getNodeCreator(mName);
    }

    public boolean isValid() {
        return ApertusJNI.isNodeValid(mName);
    }

//    public apeGeometry[] getRelatedGeometries() {
//        long[] geometryPtrs = ApertusJNI.getNodeRelatedGeometries(mName);
//        apeGeometry[] geometries = new apeGeometry[geometryPtrs.length];
//
//        for(int i = 0; i < geometryPtrs.length; ++i) {
//            String geomName = ApertusJNI.getNameFromPtr(geometryPtrs[i]);
//            apeEntity.Type geomType = apeEntity.Type.values()[ApertusJNI.getEntityType(geomName)];
//            geometries[i] = new apeGeometry(geomName, geomType);
//        }
//
//        return geometries;
//    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        apeNode otherNode = (apeNode) obj;
        return mName.equals(otherNode.mName);
    }

    @Override
    public int hashCode() {
        return Objects.hash(mName);
    }

    private String mName;
}
