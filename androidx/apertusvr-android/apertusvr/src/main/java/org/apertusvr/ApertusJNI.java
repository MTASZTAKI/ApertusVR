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

import android.content.res.AssetManager;

import androidx.annotation.Size;

public final class ApertusJNI {

    static {
        System.loadLibrary("apeJNIPlugin");
        System.loadLibrary("apeJNIStarter");
    }

    public static final String NA_STR = "";

    public native String stringFromJNIPlugin(String jhello);

    /* apeSystem functions */

    public native void startApertusVR(String confPath, AssetManager assetManager);
    public native void stopApertusVR();


    /* --------------------------------------------------- */
    /* -------------- JNI wrapper functions -------------- */
    /* --------------------------------------------------- */

    /* --- Entity --- */
    static native int getEntityType(String nativeEntity);
    static native boolean isEntityValid(String nativeEntity);
    static native boolean isEntityValidWithType(String nativeEntity, int typeAsInt);
    static native String getNameFromPtr(long entityPtr);

    /* --- Node --- */
    static native boolean isNodeValid(String nativeNode);
    static native @Size(3) float[] getNodePosition(String nativeNode);
    static native @Size(3) float[] getNodeDerivedPosition(String nativeNode);
    static native @Size(4) float[] getNodeOrientation(String nativeNode);
    static native @Size(4) float[] getNodeDerivedOrientation(String nativeNode);
    static native @Size(3) float[] getNodeScale(String nativeNode);
    static native @Size(3) float[] getNodeDerivedScale(String nativeNode);
    static native @Size(16) float[] getNodeModelMatrix(String nativeNode);
    static native @Size(16) float[] getNodeDerivedModelMatrix(String nativeNode);
    static native boolean getNodeChildrenVisibility(String nativeNode);
    static native boolean isNodeVisible(String nativeNode);
    static native boolean isNodeFixedYaw(String nativeNode);
    static native void setNodeParentNode(String nativeNode, String nativeParentNode);
    static native void detachNodeFromParentNode(String nativeNode);
    static native String getNodeParentNode(String nativeNode);
    static native apeNode[] getNodeChildNodes(String nativeNode);
    static native boolean hasNodeChildNodes(String nativeNode);
    static native boolean isNodeChildNode(String nativeNode, String nativeChildNode);
    static native void setNodePosition(String nativeNode, float x, float y, float z);
    static native void setNodeOrientation(String nativeNode, float w, float x, float y, float z);
    static native void setNodeScale(String nativeNode, float x, float y, float z);
    static native void translateNode(String nativeNode, float x, float y, float z, int transformSpace);
    static native void rotateNode(String nativeNode, float angle, float x, float y, float z, int transformSpace);
    static native void setNodeChildrenVisibility(String nativeNode, boolean visible);
    static native void setNodeFixedYaw(String nativeNode, boolean fix);
    static native void showNodeBoundingBox(String nativeNode, boolean show);
    static native void setNodeInheritOrientation(String nativeNode, boolean enable);
    static native boolean isNodeInheritOrientation(String nativeNode);
    static native void setNodeInitialState(String nativeNode);
    static native boolean isNodeReplicated(String nativeNode);
    static native void setNodeOwner(String nativeNode, String ownerID);
    static native String getNodeOwner(String nativeNode);
    static native String getNodeCreator(String nativeNode);

    /* --- Light --- */
    public static native int getLightType(String nativeLigh);
    static native @Size(4) float[] getLightDiffuseColor(String nativeLigh);
    static native @Size(4) float[] getLightSpecularColor(String nativeLigh);
    static native @Size(3) float[] getLightSpotRange(String nativeLight);
    static native float[] getLightAttenuation(String nativeLight);
    static native float[] getLightDirection(String nativeLight);
    static native void setLightType(String nativeLight, int lightType);
    static native void setLightDiffuseColor(String nativeLight, float r, float g, float b, float a);
    static native void setLightSpecularColor(String nativeLight, float r, float g, float b, float a);
    static native void setLightSpotRange(String nativeLight, float innerAngle, float outerAngle, float falloff);
    static native void setLightAttenuation(String nativeLight, float range, float constant, float linear, float quadratic);
    static native void setLightDirection(String nativeLight, float x, float y, float z);
    static native void setLightParentNode(String nativeLight, String parentName);
    static native String getLightParentNode(String nativeLight);

    /* --- Camera --- */
    static native float getCameraFocalLength(String nativeCamera);
    static native void setCameraFocalLength(String nativeCamera, float focalLength);
    static native @Size(2) float[] getCameraFrustumOffset(String nativeCamera);
    static native void setCameraFrustumOffset(String nativeCamera, float x, float y);
    static native float getCameraFOVy(String nativeCamera);
    static native void setCameraFOVy(String nativeCamera, float fovY);
    static native float getCameraNearClipDistance(String nativeCamera);
    static native void setCameraNearClipDistance(String nativeCamera, float nearClipDistance);
    static native float getCameraFarClipDistance(String nativeCamera);
    static native void setCameraFarClipDistance(String nativeCamera, float farClipDistance);
    static native float getCameraAspectRatio(String nativeCamera);
    static native void setCameraAspectRatio(String nativeCamera, float aspectRatio);
    static native void setCameraAutoAspectRatio(String nativeCamera, boolean enable);
    static native boolean isCameraAutoAspectRatio(String nativeCamera);
    static native @Size(16) float[] getCameraProjection(String nativeCamera);
    static native void setCameraProjection(String nativeCamera, @Size(16) float[] projection);
    static native void setCameraParentNode(String nativeCamera, String nativeParentNode);
    static native String getCameraParentNode(String nativeCamera);
    static native void setCameraProjectionType(String nativeCamera,int projectionType);
    static native int getCameraProjectionType(String nativeCamera);
    static native void setCameraOrthoWindowSize(String nativeCamera, float width, float height);
    static native @Size(2) float[] getCameraOrthoWindowSize(String nativeCamera);
    static native void setCameraWindow(String nativeCamera, String window);
    static native String getCameraWindow(String nativeCamera);
    static native void setCameraVisibilityMask(String nativeCamera, int mask);
    static native int getCameraVisibilityMask(String nativeCamera);

    /* --- Geometry --- */
    static native String getGeometryParentNode(String nativeGeometry);
    static native boolean isGeometryIntersectionEnabled(String nativeGeometry);
    // static native apeEntity[] getGeometryIntersections(String nativeGeometry);

    /* --- FileGeometry --- */
    static native void setFileGeometryFileName(String nativeFileGeometry, String fileName);
    static native String getFileGeometryFileName(String nativeFileGeometry);
    static native void setFileGeometryParentNode(String nativeFileGeometry, String parentNodeName);
    static native void setFileGeometryMaterial(String nativeFileGeometry, String nativeMaterial);
    static native String getFileGeometryMaterial(String nativeFileGeometry);
    static native void exportFileGeometryMesh(String nativeFileGeometry);
    static native boolean isFileGeometryExportMesh(String nativeFileGeometry);
    static native void mergeFileGeometrySubMeshes(String nativeFileGeometry);
    static native boolean isFileGeometryMergeSubMeshes(String nativeFileGeometry);
    static native void setFileGeometryVisibilityFlag(String nativeFileGeometry, int flag);
    static native int getFileGeometryVisibilityFlag(String nativeFileGeometry);
    static native void setFileGeometryOwner(String nativeFileGeometry, String ownerID);
    static native String getFileGeometryOwner(String nativeFileGeometry);
    static native float getFileGeometryUnitScale(String nativeFileGeometry);
    static native void setFileGeometryUnitScale(String nativeFileGeometry, float unitScale);

    /* --- PlaneGeometry --- */
    static native void setPlaneGeometryParameters(String nativePlaneGeometry, float numSegX, float numSegY, float sizeX, float sizeY, float tileX, float tileY);
    static native @Size(2) float[] getPlaneGeometryNumSeg(String nativePlaneGeometry);
    static native @Size(2) float[] getPlaneGeometrySize(String nativePlaneGeometry);
    static native @Size(2) float[] getPlaneGeometryTile(String nativePlaneGeometry);
    static native @Size(6) float[] getPlaneGeometryParameters(String nativePlaneGeometry);
    static native void setPlaneGeometryParentNode(String nativePlaneGeometry, String parentNodeName);
    static native void setPlaneGeometryMaterial(String nativePlaneGeometry, String nativeMaterial);
    static native String getPlaneGeometryMaterial(String nativePlaneGeometry);
    static native void setPlaneGeometryOwner(String nativePlaneGeometry, String nativeOwner);
    static native String getPlaneGeometryOwner(String nativePlaneGeometry);

    /* --- ConeGeometry --- */
    static native void setConeGeometryParameters(String nativeCone, float radius, float height, float tile, float numSegX, float numSegY);
    static native @Size(5) float[] getConeGeometryParameters(String nativeCone);
    static native void setConeGeometryParentNode(String nativeCone, String parentNode);
    static native void setConeGeometryMaterial(String nativeCone, String material);
    static native String getConeGeometryMaterial(String nativeCone);
    static native void setConeGeometryOwner(String nativeCone, String ownerID);
    static native String getConeGeometryOwner(String nativeCone);

    /* --- Material --- */
    static native int getMaterialCullingMode(String nativeMaterial);
    static native int getMaterialManualCullingMode(String nativeMaterial);
    static native boolean getMaterialDepthWriteEnabled(String nativeMaterial);
    static native boolean getMaterialDepthCheckEnabled(String nativeMaterial);
    static native @Size(2) float[] getMaterialDepthBias(String nativeMaterial);
    static native boolean getMaterialLightingEnabled(String nativeMaterial);
    static native int getMaterialSceneBlendingType(String nativeMaterial);
    static native boolean isMaterialShowOnOverlay(String nativeMaterial);

    /* --- FileMaterial --- */
    static native void setFileMaterialFileName (String nativeFileMaterial, String fileName);
    static native String getFileMaterialfFileName (String nativeFileMaterial);
    static native void setFileMaterialAsSkyBox(String nativeFileMaterial);
    static native void setFileMaterialTexture(String nativeFileMaterial, String texture);
    static native String getFileMaterialTexture(String nativeFileMaterial);
    static native void setFileMaterialOwner(String nativeFileMaterial, String ownerID);
    static native String getFileMaterialOwner(String nativeFileMaterial);

    /* --- ManualMaterial --- */
    static native void setManualMaterialDiffuseColor(String nativeManualMaterial, float r, float g, float b, float a);
    static native void setManualMaterialSpecularColor(String nativeManualMaterial, float r, float g, float b, float a);
    static native @Size(4) float[] getManualMaterialDiffuseColor(String nativeManualMaterial);
    static native @Size(4) float[] getManualMaterialSpecularColor(String nativeManualMaterial);
    static native void setManualMaterialAmbientColor(String nativeManualMaterial, float r, float g, float b, float a);
    static native void setManualMaterialEmissiveColor(String nativeManualMaterial, float r, float g, float b, float a);
    static native @Size(4) float[] getManualMaterialAmbientColor(String nativeManualMaterial);
    static native @Size(4) float[] getManualMaterialEmissiveColor(String nativeManualMaterial);
    static native void setManualMaterialTexture(String nativeManualMaterial, String nativeTexture);
    static native String getManualMaterialTexture(String nativeManualMaterial);
    static native void setManualMaterialCullingMode(String nativeManualMaterial, int cullingMode);
    static native void setManualMaterialSceneBlending(String nativeManualMaterial, int sceneBlendingType);
    static native void setManualMaterialDepthWriteEnabled(String nativeManualMaterial, boolean enable);
    static native void setManualMaterialDepthCheckEnabled(String nativeManualMaterial, boolean enable);
    static native void setManualMaterialLightingEnabled(String nativeManualMaterial, boolean enable);
    static native void setManualMaterialManualCullingMode(String nativeManualMaterial, int manualcullingMode);
    static native void setManualMaterialDepthBias(String nativeManualMaterial, float constantBias, float slopeScaleBias);
    static native void showManualMaterialOnOverlay(String nativeManualMaterial, boolean enable, int zOrder);
    static native int getManualMaterialZOrder(String nativeManualMaterial);
    static native void setManualMaterialOwner(String nativeManualMaterial, String ownerID);
    static native String getManualMaterialOwner(String nativeManualMaterial);

    /* --- FileTexture --- */
    static native void setFileTextureFileName(String nativeFileTexture, String fileName);
    static native String getFileTextureFileName(String nativeFileTexture);
    static native void setFileTextureMapType(String nativeFileTexture, int mapType);
    static native int getFileTextureMapType(String nativeFileTexture);
    static native void setFileTextureOwner(String nativeFileTexture, String ownerID);
    static native String getFileTextureOwner(String nativeFileTexture);

    /* --- Event --- */
    public static native void processEventDoubleQueue();
}
