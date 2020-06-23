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

public class apeMaterial extends apeEntity {

    public enum CullingMode {
        NONE_CM,
        CLOCKWISE,
        ANTICLOCKWISE,
        INVALID_CM
    }

    public enum ManualCullingMode {
        NONE_MCM,
        BACK,
        FRONT,
        INVALID_MCM
    }

    public enum SceneBlendingType {
        NONE_SBT,
        ADD,
        TRANSPARENT_ALPHA,
        REPLACE,
        INVALID_SBT
    }

    apeMaterial(String name, Type type) {
        super(name, type);
    }

    public CullingMode getCullingMode() {
        return CullingMode.values()[ApertusJNI.getMaterialCullingMode(mName)];
    }

    public ManualCullingMode getManualCullingMode() {
        return ManualCullingMode.values()[ApertusJNI.getMaterialManualCullingMode(mName)];
    }

    public boolean getDepthWriteEnabled() {
        return ApertusJNI.getMaterialDepthWriteEnabled(mName);
    }

    public boolean getDepthCheckEnabled() {
        return ApertusJNI.getMaterialDepthCheckEnabled(mName);
    }

    public apeVector2 getDepthBias() {
        return new apeVector2(ApertusJNI.getMaterialDepthBias(mName));
    }

    public boolean getLightingEnabled() {
        return ApertusJNI.getMaterialLightingEnabled(mName);
    }

    public SceneBlendingType getSceneBlendingType() {
        return SceneBlendingType.values()[ApertusJNI.getMaterialSceneBlendingType(mName)];
    }

    public boolean isShowOnOverlay() {
        return ApertusJNI.isMaterialShowOnOverlay(mName);
    }
}
