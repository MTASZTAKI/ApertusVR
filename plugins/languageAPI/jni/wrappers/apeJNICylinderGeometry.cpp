//
// Created by Erik on 2020. 08. 08..
//
#include "apeJNIPlugin.h"
#include "apeICylinderGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCylinderGeometryParameters(JNIEnv* env, jclass clazz, jstring native_sphere, float radius, float height, float tile)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    if(auto cylinderGeometryShared = std::static_pointer_cast<ape::ICylinderGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cylinderGeometryShared->setParameters(radius, height, tile);
    }

    env->ReleaseStringUTFChars(native_sphere, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getCylinderGeometryParameters(JNIEnv *env, jclass clazz, jstring native_sphere)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    ape::GeometryCylinderParameters sphereParameters;
    if(auto cylinderGeometryShared = std::static_pointer_cast<ape::ICylinderGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sphereParameters = cylinderGeometryShared->getParameters();
    }

    env->ReleaseStringUTFChars(native_sphere, name);

    float outArrayBuf[] = {sphereParameters.radius, sphereParameters.height,
                           sphereParameters.tile,};
    jfloatArray jOutArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jOutArray,0,3,outArrayBuf);

    return jOutArray;
}
extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCylinderGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_sphere, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    auto parentNode = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName));

    if(auto cylinderGeometryShared = std::static_pointer_cast<ape::ICylinderGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cylinderGeometryShared->setParentNode(parentNode);
    }

    env->ReleaseStringUTFChars(native_sphere, name);
    env->ReleaseStringUTFChars(parent_node, parentNodeName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCylinderGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_sphere, jstring material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);
    const char* materialName = env->GetStringUTFChars(material, nullptr);

    ape::MaterialWeakPtr sphereMaterialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(materialName)).lock());
    if(auto cylinderGeometryShared = std::static_pointer_cast<ape::ICylinderGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cylinderGeometryShared->setMaterial(sphereMaterialWeak);
    }

    env->ReleaseStringUTFChars(native_sphere, name);
    env->ReleaseStringUTFChars(material, materialName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCylinderGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_sphere)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    const char* materialName = jniPlugin->NA_STR;
    if(auto cylinderGeometryShared = std::static_pointer_cast<ape::ICylinderGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        if (auto materialShared = cylinderGeometryShared->getMaterial().lock())
            materialName = materialShared->getName().c_str();
    }

    env->ReleaseStringUTFChars(native_sphere, name);

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCylinderGeometryOwner(JNIEnv *env, jclass clazz, jstring native_sphere, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);
    if(auto cylinderGeometryShared = std::static_pointer_cast<ape::ICylinderGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cylinderGeometryShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_sphere, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCylinderGeometryOwner(JNIEnv *env, jclass clazz, jstring native_sphere)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_sphere, nullptr);

    const char* ownerID = jniPlugin->NA_STR;
    if(auto cylinderGeometryShared = std::static_pointer_cast<ape::ICylinderGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ownerID = cylinderGeometryShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_sphere, name);

    return env->NewStringUTF(ownerID);
}