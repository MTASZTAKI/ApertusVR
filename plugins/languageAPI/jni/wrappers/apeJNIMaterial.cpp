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
#include "apeMaterial.h"

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getMaterialCullingMode(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    int cullingMode = 0;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cullingMode = materialShared->getCullingMode();

    env->ReleaseStringUTFChars(native_material,name);

    return (jint) cullingMode;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getMaterialManualCullingMode(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    int manualCullingMode = 0;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        manualCullingMode = materialShared->getManualCullingMode();

    env->ReleaseStringUTFChars(native_material,name);

    return manualCullingMode;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_getMaterialDepthWriteEnabled(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    bool depthWriteEnabled = false;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        depthWriteEnabled = materialShared->getDepthWriteEnabled();

    env->ReleaseStringUTFChars(native_material,name);

    return (jboolean) depthWriteEnabled;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_getMaterialDepthCheckEnabled(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    bool depthCheckEnabled = false;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        depthCheckEnabled = materialShared->getDepthCheckEnabled();

    env->ReleaseStringUTFChars(native_material,name);

    return (jboolean) depthCheckEnabled;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getMaterialDepthBias(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    ape::Vector2 depthBias;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        depthBias = materialShared->getDepthBias();

    env->ReleaseStringUTFChars(native_material,name);

    float outArrayBuf[] = {depthBias.x, depthBias.y};
    jfloatArray jOutArray = env->NewFloatArray(2);
    env->SetFloatArrayRegion(jOutArray,0,2,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_getMaterialLightingEnabled(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    bool materialLightingEnabled = false;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        materialLightingEnabled = materialShared->getLightingEnabled();

    env->ReleaseStringUTFChars(native_material,name);

    return (jboolean) materialLightingEnabled;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getMaterialSceneBlendingType(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    int sceneBlendingType = 0;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        sceneBlendingType = materialShared->getSceneBlendingType();

    env->ReleaseStringUTFChars(native_material,name);

    return (jint) sceneBlendingType;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isMaterialShowOnOverlay(JNIEnv *env, jclass clazz, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_material,NULL);

    bool showOnOverlay = false;
    if(auto materialShared = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        showOnOverlay = materialShared->isShowOnOverlay();

    env->ReleaseStringUTFChars(native_material,name);

    return (jboolean) showOnOverlay;
}
