package org.apertusvr;

public class apeManualMaterial extends apeMaterial {

    public apeManualMaterial(String name) {
        super(name, Type.MATERIAL_MANUAL);
    }

    public void setDiffuseColor(apeColor diffuse) {
        ApertusJNI.setManualMaterialDiffuseColor(mName,diffuse.r,diffuse.g,diffuse.b,diffuse.a);
    }

    public void setSpecularColor(apeColor specular) {
        ApertusJNI.setManualMaterialSpecularColor(mName,specular.r,specular.g,specular.b,specular.a);
    }

    public apeColor getDiffuseColor() {
        return new apeColor(ApertusJNI.getManualMaterialDiffuseColor(mName));
    }

    public apeColor getSpecularColor() {
        return new apeColor(ApertusJNI.getManualMaterialSpecularColor(mName));
    }

    public void setAmbientColor(apeColor ambient) {
        ApertusJNI.setManualMaterialAmbientColor(mName,ambient.r,ambient.g,ambient.b,ambient.a);
    }

    public void setEmissiveColor(apeColor emissive) {
        ApertusJNI.setManualMaterialEmissiveColor(mName,emissive.r,emissive.g,emissive.b,emissive.a);
    }

    public apeColor getAmbientColor() {
        return new apeColor(ApertusJNI.getManualMaterialAmbientColor(mName));
    }

    public apeColor getEmissiveColor() {
        return new apeColor(ApertusJNI.getManualMaterialEmissiveColor(mName));
    }

    public void setTexture(apeTexture texture) {
        ApertusJNI.setManualMaterialTexture(mName,texture.getName());
    }

    public apeTexture getTexture() {
        String textureName = ApertusJNI.getManualMaterialTexture(mName);
        apeEntity.Type textureType = apeEntity.Type.values()[ApertusJNI.getEntityType(textureName)];

        return new apeTexture(textureName, textureType);
    }

    public void setCullingMode(apeMaterial.CullingMode cullingMode) {
        ApertusJNI.setManualMaterialCullingMode(mName, cullingMode.ordinal());
    }

    public void setSceneBlending(apeMaterial.SceneBlendingType sceneBlendingType) {
        ApertusJNI.setManualMaterialSceneBlending(mName, sceneBlendingType.ordinal());
    }

    public void setDepthWriteEnabled(boolean enable) {
        ApertusJNI.setManualMaterialDepthWriteEnabled(mName, enable);
    }

    public void setDepthCheckEnabled(boolean enable) {
        ApertusJNI.setManualMaterialDepthCheckEnabled(mName, enable);
    }

    public void setLightingEnabled(boolean enable) {
        ApertusJNI.setManualMaterialLightingEnabled(mName, enable);
    }

    public void setManualCullingMode(apeMaterial.ManualCullingMode manualcullingMode) {
        ApertusJNI.setManualMaterialManualCullingMode(mName, manualcullingMode.ordinal());
    }

    public void setDepthBias(float constantBias, float slopeScaleBias) {
        ApertusJNI.setManualMaterialDepthBias(mName, constantBias, slopeScaleBias);
    }

    public void showOnOverlay(boolean enable, int zOrder) {
        ApertusJNI.showManualMaterialOnOverlay(mName,enable,zOrder);
    }

    public int getZOrder() {
        return ApertusJNI.getManualMaterialZOrder(mName);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setManualMaterialOwner(mName, ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getManualMaterialOwner(mName);
    }
}
