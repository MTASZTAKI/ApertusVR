#include "apeJNIPlugin.h"
#include "apeIBoxGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setBoxGeometryParameters(JNIEnv *env, jclass clazz, jstring native_box, float dimensionX, float dimensionY, float dimensionZ)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_box, nullptr);

    if(auto boxGeometryShared = std::static_pointer_cast<ape::IBoxGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        boxGeometryShared->setParameters(ape::Vector3(dimensionX,dimensionY, dimensionZ));
    }

    env->ReleaseStringUTFChars(native_box, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getBoxGeometryParameters(JNIEnv *env, jclass clazz, jstring native_box)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_box, nullptr);

    ape::GeometryBoxParameters boxParameters;
    if(auto boxGeometryShared = std::static_pointer_cast<ape::IBoxGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        boxParameters = boxGeometryShared->getParameters();
    }

    env->ReleaseStringUTFChars(native_box, name);

    float outArrayBuf[] = {boxParameters.dimensions.x, boxParameters.dimensions.y,
                           boxParameters.dimensions.z};
    jfloatArray jOutArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jOutArray,0,3,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setBoxGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_box, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_box, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    auto parentNode = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName));

    if(auto boxGeometryShared = std::static_pointer_cast<ape::IBoxGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        boxGeometryShared->setParentNode(parentNode);
    }

    env->ReleaseStringUTFChars(native_box, name);
    env->ReleaseStringUTFChars(parent_node, parentNodeName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setBoxGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_box, jstring material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_box, nullptr);
    const char* materialName = env->GetStringUTFChars(material, nullptr);

    ape::MaterialWeakPtr boxMaterialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(materialName)).lock());
    if(auto boxGeometryShared = std::static_pointer_cast<ape::IBoxGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        boxGeometryShared->setMaterial(boxMaterialWeak);
    }

    env->ReleaseStringUTFChars(native_box, name);
    env->ReleaseStringUTFChars(material, materialName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getBoxGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_box)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_box, nullptr);

    const char* materialName = jniPlugin->NA_STR;
    if(auto boxGeometryShared = std::static_pointer_cast<ape::IBoxGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        if (auto materialShared = boxGeometryShared->getMaterial().lock())
            materialName = materialShared->getName().c_str();
    }

    env->ReleaseStringUTFChars(native_box, name);

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setBoxGeometryOwner(JNIEnv *env, jclass clazz, jstring native_box, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_box, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);
    if(auto boxGeometryShared = std::static_pointer_cast<ape::IBoxGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        boxGeometryShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_box, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
        Java_org_apertusvr_ApertusJNI_getBoxGeometryOwner(JNIEnv *env, jclass clazz, jstring native_box)
{
ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
const char* name = env->GetStringUTFChars(native_box, nullptr);

const char* ownerID = jniPlugin->NA_STR;
if(auto boxGeometryShared = std::static_pointer_cast<ape::IBoxGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
{
ownerID = boxGeometryShared->getOwner().c_str();
}

env->ReleaseStringUTFChars(native_box, name);

return env->NewStringUTF(ownerID);
}