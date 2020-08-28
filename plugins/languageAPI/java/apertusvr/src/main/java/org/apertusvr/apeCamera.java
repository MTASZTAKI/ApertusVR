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

public final class apeCamera extends apeEntity {

    public enum ProjectionType
    {
        ORTHOGRAPHIC,
        PERSPECTIVE,
        INVALID
    }

    public apeCamera(String name) {
        super(name, Type.CAMERA);
    }

    float getFocalLength() {
        return ApertusJNI.getCameraFocalLength(mName);
    }

    void setFocalLength(float focalLength) {
        ApertusJNI.setCameraFocalLength(mName,focalLength);
    }

    apeVector2 getFrustumOffset() {
        return new apeVector2(ApertusJNI.getCameraFrustumOffset(mName));
    }

    void setFrustumOffset(apeVector2 frustumOffset) {
        ApertusJNI.setCameraFrustumOffset(mName,frustumOffset.x, frustumOffset.y);
    }

    apeRadian getFOVy() {
        return new apeRadian(ApertusJNI.getCameraFOVy(mName));
    }

    void setFOVy(apeRadian fovY) {
        ApertusJNI.setCameraFOVy(mName,fovY.radian);
    }

    float getNearClipDistance() {
        return ApertusJNI.getCameraNearClipDistance(mName);
    }

    void setNearClipDistance(float nearClipDistance) {
        ApertusJNI.setCameraNearClipDistance(mName, nearClipDistance);
    }

    float getFarClipDistance() {
        return ApertusJNI.getCameraFarClipDistance(mName);
    }

    void setFarClipDistance(float farClipDistance) {
        ApertusJNI.setCameraFarClipDistance(mName, farClipDistance);
    }

    float getAspectRatio() {
        return ApertusJNI.getCameraAspectRatio(mName);
    }

    void setAspectRatio(float aspectRatio) {
        ApertusJNI.setCameraAspectRatio(mName, aspectRatio);
    }

    void setAutoAspectRatio(boolean enable) {
        ApertusJNI.setCameraAutoAspectRatio(mName, enable);
    }

    boolean isAutoAspectRatio() {
        return ApertusJNI.isCameraAutoAspectRatio(mName);
    }

    apeMatrix4 getProjection() {
        return new apeMatrix4(ApertusJNI.getCameraProjection(mName));
    }

    void setProjection(apeMatrix4 projection) {
        ApertusJNI.setCameraProjection(mName, projection.m);
    }

    void setParentNode(apeNode parentNode) {
        ApertusJNI.setCameraParentNode(mName, parentNode.getName());
    }

    apeNode getParentNode() {
        return new apeNode(ApertusJNI.getCameraParentNode(mName));
    }

    void setProjectionType(apeCamera.ProjectionType type) {
        ApertusJNI.setCameraProjectionType(mName, type.ordinal());
    }

    apeCamera.ProjectionType getProjectionType() {
        return ProjectionType.values()[ApertusJNI.getCameraProjectionType(mName)];
    }

    void setOrthoWindowSize(float width, float height) {
        ApertusJNI.setCameraOrthoWindowSize(mName,width,height);
    }

    apeVector2 getOrthoWindowSize() {
        return new apeVector2(ApertusJNI.getCameraOrthoWindowSize(mName));
    }

    void setWindow(String window) {
        ApertusJNI.setCameraWindow(mName, window);
    }

    String getWindow() {
        return ApertusJNI.getCameraWindow(mName);
    }

    void setVisibilityMask(int mask) {
        ApertusJNI.setCameraVisibilityMask(mName,mask);
    }

    int getVisibilityMask() {
        return ApertusJNI.getCameraVisibilityMask(mName);
    }

}
