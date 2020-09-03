//
// Created by Erik on 2020. 08. 08..
//
#include "apeJNIPlugin.h"
#include "apeISphereGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setSphereGeometryParameters(JNIEnv* env, jclass clazz, jstring native_sphere, float radius, float tileX, float tileY)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    if(auto sphereGeometryShared = std::static_pointer_cast<ape::ISphereGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sphereGeometryShared->setParameters(radius, ape::Vector2(tileX,tileY));
    }

    env->ReleaseStringUTFChars(native_sphere, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getSphereGeometryParameters(JNIEnv *env, jclass clazz, jstring native_sphere)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    ape::GeometrySphereParameters sphereParameters;
    if(auto sphereGeometryShared = std::static_pointer_cast<ape::ISphereGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sphereParameters = sphereGeometryShared->getParameters();
    }

    env->ReleaseStringUTFChars(native_sphere, name);

    float outArrayBuf[] = {sphereParameters.radius, sphereParameters.tile.x,
                           sphereParameters.tile.y,};
    jfloatArray jOutArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jOutArray,0,3,outArrayBuf);

    return jOutArray;
}
extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setSphereGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_sphere, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    auto parentNode = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName));

    if(auto sphereGeometryShared = std::static_pointer_cast<ape::ISphereGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sphereGeometryShared->setParentNode(parentNode);
    }

    env->ReleaseStringUTFChars(native_sphere, name);
    env->ReleaseStringUTFChars(parent_node, parentNodeName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setSphereGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_sphere, jstring material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);
    const char* materialName = env->GetStringUTFChars(material, nullptr);

    ape::MaterialWeakPtr sphereMaterialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(materialName)).lock());
    if(auto sphereGeometryShared = std::static_pointer_cast<ape::ISphereGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sphereGeometryShared->setMaterial(sphereMaterialWeak);
    }

    env->ReleaseStringUTFChars(native_sphere, name);
    env->ReleaseStringUTFChars(material, materialName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getSphereGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_sphere)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    const char* materialName = jniPlugin->NA_STR;
    if(auto sphereGeometryShared = std::static_pointer_cast<ape::ISphereGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        if (auto materialShared = sphereGeometryShared->getMaterial().lock())
            materialName = materialShared->getName().c_str();
    }

    env->ReleaseStringUTFChars(native_sphere, name);

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setSphereGeometryOwner(JNIEnv *env, jclass clazz, jstring native_sphere, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);
    if(auto sphereGeometryShared = std::static_pointer_cast<ape::ISphereGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sphereGeometryShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_sphere, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getSphereGeometryOwner(JNIEnv *env, jclass clazz, jstring native_sphere)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    const char* ownerID = jniPlugin->NA_STR;
    if(auto sphereGeometryShared = std::static_pointer_cast<ape::ISphereGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ownerID = sphereGeometryShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_sphere, name);

    return env->NewStringUTF(ownerID);
}