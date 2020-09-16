#include "apeJNIPlugin.h"
#include "apeITorusGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTorusGeometryParameters(JNIEnv *env, jclass clazz, jstring native_torus, float radius, float sectionRadius, float tileX, float tileY)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_torus, nullptr);

    if(auto torusGeometryShared = std::static_pointer_cast<ape::ITorusGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        torusGeometryShared->setParameters(radius, sectionRadius, ape::Vector2(tileX,tileY));
    }

    env->ReleaseStringUTFChars(native_torus, name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getTorusGeometryParameters(JNIEnv *env, jclass clazz, jstring native_torus)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_torus, nullptr);

    ape::GeometryTorusParameters torusParameters;
    if(auto torusGeometryShared = std::static_pointer_cast<ape::ITorusGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        torusParameters = torusGeometryShared->getParameters();
    }

    env->ReleaseStringUTFChars(native_torus, name);

    float outArrayBuf[] = {torusParameters.radius, torusParameters.sectionRadius,
                           torusParameters.tile.x, torusParameters.tile.y};
    jfloatArray jOutArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jOutArray,0,4,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTorusGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_torus, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_torus, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    auto parentNode = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName));

    if(auto torusGeometryShared = std::static_pointer_cast<ape::ITorusGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        torusGeometryShared->setParentNode(parentNode);
    }

    env->ReleaseStringUTFChars(native_torus, name);
    env->ReleaseStringUTFChars(parent_node, parentNodeName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTorusGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_torus, jstring material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_torus, nullptr);
    const char* materialName = env->GetStringUTFChars(material, nullptr);

    ape::MaterialWeakPtr torusMaterialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(materialName)).lock());
    if(auto torusGeometryShared = std::static_pointer_cast<ape::ITorusGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        torusGeometryShared->setMaterial(torusMaterialWeak);
    }

    env->ReleaseStringUTFChars(native_torus, name);
    env->ReleaseStringUTFChars(material, materialName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getTorusGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_torus)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_torus, nullptr);

    const char* materialName = jniPlugin->NA_STR;
    if(auto torusGeometryShared = std::static_pointer_cast<ape::ITorusGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        if (auto materialShared = torusGeometryShared->getMaterial().lock())
            materialName = materialShared->getName().c_str();
    }

    env->ReleaseStringUTFChars(native_torus, name);

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTorusGeometryOwner(JNIEnv *env, jclass clazz, jstring native_torus, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_torus, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);
    if(auto torusGeometryShared = std::static_pointer_cast<ape::ITorusGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        torusGeometryShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_torus, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getTorusGeometryOwner(JNIEnv *env, jclass clazz, jstring native_torus)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_torus, nullptr);

    const char* ownerID = jniPlugin->NA_STR;
    if(auto torusGeometryShared = std::static_pointer_cast<ape::ITorusGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ownerID = torusGeometryShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_torus, name);

    return env->NewStringUTF(ownerID);
}