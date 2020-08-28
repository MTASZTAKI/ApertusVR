#include "apeJNIPlugin.h"
#include "apeIConeGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setConeGeometryParameters(JNIEnv *env, jclass clazz, jstring native_cone, float radius, float height, float tile, float numSegX, float numSegY)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_cone, nullptr);

    if(auto coneGeometryShared = std::static_pointer_cast<ape::IConeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        coneGeometryShared->setParameters(radius, height, tile, ape::Vector2(numSegX,numSegY));
    }

    env->ReleaseStringUTFChars(native_cone, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getConeGeometryParameters(JNIEnv *env, jclass clazz, jstring native_cone)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_cone, nullptr);

    ape::GeometryConeParameters coneParameters;
    if(auto coneGeometryShared = std::static_pointer_cast<ape::IConeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        coneParameters = coneGeometryShared->getParameters();
    }

    env->ReleaseStringUTFChars(native_cone, name);

    float outArrayBuf[] = {coneParameters.radius, coneParameters.height,
                           coneParameters.tile, coneParameters.numSeg.x,
                           coneParameters.numSeg.y};
    jfloatArray jOutArray = env->NewFloatArray(5);
    env->SetFloatArrayRegion(jOutArray,0,5,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setConeGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_cone, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_cone, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    auto parentNode = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName));

    if(auto coneGeometryShared = std::static_pointer_cast<ape::IConeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        coneGeometryShared->setParentNode(parentNode);
    }

    env->ReleaseStringUTFChars(native_cone, name);
    env->ReleaseStringUTFChars(parent_node, parentNodeName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setConeGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_cone, jstring material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_cone, nullptr);
    const char* materialName = env->GetStringUTFChars(material, nullptr);

    ape::MaterialWeakPtr coneMaterialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(materialName)).lock());
    if(auto coneGeometryShared = std::static_pointer_cast<ape::IConeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        coneGeometryShared->setMaterial(coneMaterialWeak);
    }

    env->ReleaseStringUTFChars(native_cone, name);
    env->ReleaseStringUTFChars(material, materialName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getConeGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_cone)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_cone, nullptr);

    const char* materialName = jniPlugin->NA_STR;
    if(auto coneGeometryShared = std::static_pointer_cast<ape::IConeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        if (auto materialShared = coneGeometryShared->getMaterial().lock())
            materialName = materialShared->getName().c_str();
    }

    env->ReleaseStringUTFChars(native_cone, name);

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setConeGeometryOwner(JNIEnv *env, jclass clazz, jstring native_cone, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_cone, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);
    if(auto coneGeometryShared = std::static_pointer_cast<ape::IConeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        coneGeometryShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_cone, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getConeGeometryOwner(JNIEnv *env, jclass clazz, jstring native_cone)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_cone, nullptr);

    const char* ownerID = jniPlugin->NA_STR;
    if(auto coneGeometryShared = std::static_pointer_cast<ape::IConeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ownerID = coneGeometryShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_cone, name);

    return env->NewStringUTF(ownerID);
}