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

#include "apeJNIPlugin.h"
#include "apeIManualMaterial.h"
#include "apeTexture.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialDiffuseColor(JNIEnv *env, jclass clazz, jstring native_manual_material, jfloat r, jfloat g, jfloat b, jfloat a)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setDiffuseColor(ape::Color(r,g,b,a));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialSpecularColor(JNIEnv *env, jclass clazz, jstring native_manual_material, jfloat r, jfloat g, jfloat b, jfloat a)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setDiffuseColor(ape::Color(r,g,b,a));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getManualMaterialDiffuseColor(JNIEnv *env, jclass clazz, jstring native_manual_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    ape::Color diffuse;
    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        diffuse = manualMaterialShared->getDiffuseColor();
    }

    env->ReleaseStringUTFChars(native_manual_material, name);

    float diffuseArrayBuf[] = {diffuse.r, diffuse.g, diffuse.b, diffuse.a};
    jfloatArray jDiffuseArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jDiffuseArray,0,4,diffuseArrayBuf);

    return jDiffuseArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getManualMaterialSpecularColor(JNIEnv *env, jclass clazz, jstring native_manual_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    ape::Color specular;
    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        specular = manualMaterialShared->getSpecularColor();
    }

    env->ReleaseStringUTFChars(native_manual_material, name);

    float specularArrayBuf[] = {specular.r, specular.g, specular.b, specular.a};
    jfloatArray jSpecularArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jSpecularArray,0,4,specularArrayBuf);

    return jSpecularArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialAmbientColor(JNIEnv *env, jclass clazz, jstring native_manual_material, jfloat r, jfloat g, jfloat b, jfloat a)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setAmbientColor(ape::Color(r,g,b,a));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialEmissiveColor(JNIEnv *env, jclass clazz, jstring native_manual_material, jfloat r, jfloat g, jfloat b, jfloat a)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setEmissiveColor(ape::Color(r,g,b,a));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getManualMaterialAmbientColor(JNIEnv *env, jclass clazz, jstring native_manual_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    ape::Color ambient;
    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ambient = manualMaterialShared->getAmbientColor();
    }

    env->ReleaseStringUTFChars(native_manual_material, name);

    float ambientArrayBuf[] = {ambient.r, ambient.g, ambient.b, ambient.a};
    jfloatArray jAmbientArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jAmbientArray,0,4,ambientArrayBuf);

    return jAmbientArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getManualMaterialEmissiveColor(JNIEnv *env, jclass clazz, jstring native_manual_material)
{
    ape::JNIPlugin *jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char *name = env->GetStringUTFChars(native_manual_material, NULL);

    ape::Color emissive;
    if (auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock())) {
        emissive = manualMaterialShared->getEmissiveColor();
    }

    env->ReleaseStringUTFChars(native_manual_material, name);

    float emissiveArrayBuf[] = {emissive.r, emissive.g, emissive.b, emissive.a};
    jfloatArray jEmissiveArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jEmissiveArray, 0, 4, emissiveArrayBuf);

    return jEmissiveArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialTexture(JNIEnv *env, jclass clazz, jstring native_manual_material, jstring native_texture)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);
    const char* textureName = env->GetStringUTFChars(native_manual_material, NULL);

    ape::TextureWeakPtr textureWeak = std::static_pointer_cast<ape::Texture>(jniPlugin->getSceneManager()->getEntity(std::string(textureName)).lock());

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setTexture(textureWeak);
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
    env->ReleaseStringUTFChars(native_texture, textureName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getManualMaterialTexture(JNIEnv *env, jclass clazz, jstring native_manual_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    ape::TextureWeakPtr textureWeak;
    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        textureWeak = manualMaterialShared->getTexture();
    }

    env->ReleaseStringUTFChars(native_manual_material, name);

    const char* textureName = "";
    if(auto textureShared = textureWeak.lock())
        textureName = textureShared->getName().c_str();


    return env->NewStringUTF(textureName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialCullingMode(JNIEnv *env, jclass clazz, jstring native_manual_material, jint culling_mode)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setCullingMode(ape::Material::CullingMode(culling_mode));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialSceneBlending(JNIEnv *env, jclass clazz, jstring native_manual_material, jint scene_blending_type)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setSceneBlending(ape::Material::SceneBlendingType(scene_blending_type));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialDepthWriteEnabled(JNIEnv *env, jclass clazz, jstring native_manual_material, jboolean enable)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setDepthWriteEnabled(enable);
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialDepthCheckEnabled(JNIEnv *env, jclass clazz, jstring native_manual_material, jboolean enable)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setDepthCheckEnabled(enable);
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialLightingEnabled(JNIEnv *env, jclass clazz, jstring native_manual_material, jboolean enable)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setLightingEnabled(enable);
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialManualCullingMode(JNIEnv *env, jclass clazz, jstring native_manual_material, jint manualculling_mode)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setManualCullingMode(ape::Material::ManualCullingMode(manualculling_mode));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialDepthBias(JNIEnv *env, jclass clazz, jstring native_manual_material, jfloat constant_bias, jfloat slope_scale_bias)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setDepthBias(constant_bias, slope_scale_bias);
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT void Java_org_apertusvr_ApertusJNI_showManualMaterialOnOverlay(JNIEnv *env, jclass clazz, jstring native_manual_material, jboolean enable, jint z_order)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->showOnOverlay(enable, z_order);
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getManualMaterialZOrder(JNIEnv *env, jclass clazz, jstring native_manual_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    int zOrder = 0;
    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        zOrder = manualMaterialShared->getZOrder();
    }

    env->ReleaseStringUTFChars(native_manual_material, name);

    return (jint)zOrder;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setManualMaterialOwner(JNIEnv *env, jclass clazz, jstring native_manual_material, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);
    const char* ownerID = env->GetStringUTFChars(native_manual_material, NULL);

    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        manualMaterialShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_manual_material, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getManualMaterialOwner(JNIEnv *env, jclass clazz, jstring native_manual_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_manual_material, NULL);

    const char* ownerID = "";
    if(auto manualMaterialShared = std::static_pointer_cast<ape::IManualMaterial>(
            jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ownerID = manualMaterialShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_manual_material, name);

    return env->NewStringUTF(ownerID);
}