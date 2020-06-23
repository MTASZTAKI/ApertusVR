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

public final class apeFileGeometry extends apeGeometry {

    public apeFileGeometry(String name) {
        super(name, Type.GEOMETRY_FILE);
    }

    public void setFileName (String fileName) {
        ApertusJNI.setFileGeometryFileName(mName, fileName);
    }

    public String getFileName () {
        return ApertusJNI.getFileGeometryFileName(mName);
    }

    public void setParentNode(apeNode parentNode) {
        ApertusJNI.setFileGeometryParentNode(mName, parentNode.getName());
    }

    public void setMaterial(apeMaterial material) {
        ApertusJNI.setFileGeometryMaterial(mName, material.getName());
    }

    public apeMaterial getMaterial() {
        String materialName = ApertusJNI.getFileGeometryMaterial(mName);
        apeEntity.Type materialType = apeEntity.Type.values()[ApertusJNI.getEntityType(mName)];

        return new apeMaterial(materialName,materialType);
    }

    public void exportMesh() {
        ApertusJNI.exportFileGeometryMesh(mName);
    }

    public boolean isExportMesh() {
        return ApertusJNI.isFileGeometryExportMesh(mName);
    }

    public void mergeSubMeshes() {
        ApertusJNI.mergeFileGeometrySubMeshes(mName);
    }

    public boolean isMergeSubMeshes() {
        return ApertusJNI.isFileGeometryMergeSubMeshes(mName);
    }

    public void setVisibilityFlag(int flag) {
        ApertusJNI.setFileGeometryVisibilityFlag(mName, flag);
    }

    public int getVisibilityFlag() {
        return ApertusJNI.getFileGeometryVisibilityFlag(mName);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setFileGeometryOwner(mName, ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getFileGeometryOwner(mName);
    }

    public void setUnitScale(float unitScale) {
        ApertusJNI.setFileGeometryUnitScale(mName,unitScale);
    }

    public float getUnitScale() {
        return ApertusJNI.getFileGeometryUnitScale(mName);
    }
}
