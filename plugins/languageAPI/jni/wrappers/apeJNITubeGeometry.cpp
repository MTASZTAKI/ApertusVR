#include "apeJNIPlugin.h"
#include "apeITubeGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTubeGeometryParameters(JNIEnv *env, jclass clazz, jstring native_tube, float height, float tile)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_tube, nullptr);

    if(auto tubeGeometryShared = std::static_pointer_cast<ape::ITubeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        tubeGeometryShared->setParameters(height, tile);
    }

    env->ReleaseStringUTFChars(native_tube, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getTubeGeometryParameters(JNIEnv *env, jclass clazz, jstring native_tube)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_tube, nullptr);

    ape::GeometryTubeParameters tubeParameters;
    if(auto tubeGeometryShared = std::static_pointer_cast<ape::ITubeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        tubeParameters = tubeGeometryShared->getParameters();
    }

    env->ReleaseStringUTFChars(native_tube, name);

    float outArrayBuf[] = {tubeParameters.height, tubeParameters.tile};
    jfloatArray jOutArray = env->NewFloatArray(2);
    env->SetFloatArrayRegion(jOutArray,0,2,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTubeGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_tube, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_tube, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    auto parentNode = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName));

    if(auto tubeGeometryShared = std::static_pointer_cast<ape::ITubeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        tubeGeometryShared->setParentNode(parentNode);
    }

    env->ReleaseStringUTFChars(native_tube, name);
    env->ReleaseStringUTFChars(parent_node, parentNodeName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTubeGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_tube, jstring material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_tube, nullptr);
    const char* materialName = env->GetStringUTFChars(material, nullptr);

    ape::MaterialWeakPtr tubeMaterialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(materialName)).lock());
    if(auto tubeGeometryShared = std::static_pointer_cast<ape::ITubeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        tubeGeometryShared->setMaterial(tubeMaterialWeak);
    }

    env->ReleaseStringUTFChars(native_tube, name);
    env->ReleaseStringUTFChars(material, materialName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getTubeGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_tube)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_tube, nullptr);

    const char* materialName = jniPlugin->NA_STR;
    if(auto tubeGeometryShared = std::static_pointer_cast<ape::ITubeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        if (auto materialShared = tubeGeometryShared->getMaterial().lock())
            materialName = materialShared->getName().c_str();
    }

    env->ReleaseStringUTFChars(native_tube, name);

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTubeGeometryOwner(JNIEnv *env, jclass clazz, jstring native_tube, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_tube, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);
    if(auto tubeGeometryShared = std::static_pointer_cast<ape::ITubeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        tubeGeometryShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_tube, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getTubeGeometryOwner(JNIEnv *env, jclass clazz, jstring native_tube)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_tube, nullptr);

    const char* ownerID = jniPlugin->NA_STR;
    if(auto tubeGeometryShared = std::static_pointer_cast<ape::ITubeGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ownerID = tubeGeometryShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_tube, name);

    return env->NewStringUTF(ownerID);
}