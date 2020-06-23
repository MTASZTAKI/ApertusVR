#include "apeJNIPlugin.h"
#include "apeIFileGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileGeometryFileName(JNIEnv *env, jclass clazz,
                                                      jstring native_file_geometry,
                                                      jstring file_name) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry,NULL);
    const char* fileName = env->GetStringUTFChars(file_name,NULL);

    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->setFileName(std::string(fileName));

    env->ReleaseStringUTFChars(native_file_geometry,name);
    env->ReleaseStringUTFChars(file_name,fileName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileGeometryFileName(JNIEnv *env, jclass clazz,
                                                      jstring native_file_geometry) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry,NULL);

    const char* fileName = "";
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileName = fileGeometryShared->getFileName().c_str();

    return env->NewStringUTF(fileName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileGeometryParentNode(JNIEnv *env, jclass clazz,
                                                        jstring native_file_geometry,
                                                        jstring parent_node_name) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);
    const char* parentName = env->GetStringUTFChars(parent_node_name, NULL);

    ape::NodeWeakPtr parentWeak = jniPlugin->getSceneManager()->getNode(std::string(parentName));
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->setParentNode(parentWeak);

    env->ReleaseStringUTFChars(native_file_geometry,name);
    env->ReleaseStringUTFChars(parent_node_name, parentName);
}


extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_file_geometry, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);
    const char* material_name = env->GetStringUTFChars(native_material, NULL);

    ape::MaterialWeakPtr materialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(material_name)).lock());
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->setMaterial(materialWeak);

    env->ReleaseStringUTFChars(native_file_geometry, name);
    env->ReleaseStringUTFChars(native_material, material_name);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    ape::MaterialWeakPtr materialWeak;
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        materialWeak = fileGeometryShared->getMaterial();

    env->ReleaseStringUTFChars(native_file_geometry, name);

    const char* materialName = "";
    if(auto materialShared = materialWeak.lock())
        materialName = materialShared->getName().c_str();

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_exportFileGeometryMesh(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->exportMesh();

    env->ReleaseStringUTFChars(native_file_geometry, name);
}

extern "C"
JNIEXPORT jboolean JNICALL
        Java_org_apertusvr_ApertusJNI_isFileGeometryExportMesh(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    bool isExportMesh = false;
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        isExportMesh = fileGeometryShared->isExportMesh();

    env->ReleaseStringUTFChars(native_file_geometry, name);

    return (jboolean) isExportMesh;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_mergeFileGeometrySubMeshes(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->mergeSubMeshes();

    env->ReleaseStringUTFChars(native_file_geometry, name);
}

extern "C"
JNIEXPORT jboolean JNICALL
        Java_org_apertusvr_ApertusJNI_isFileGeometryMergeSubMeshes(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    bool isMergeSubMeshes = false;
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        isMergeSubMeshes = fileGeometryShared->isMergeSubMeshes();

    env->ReleaseStringUTFChars(native_file_geometry, name);

    return (jboolean) isMergeSubMeshes;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileGeometryVisibilityFlag(JNIEnv *env, jclass clazz, jstring native_file_geometry, jint flag)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->setVisibilityFlag((unsigned int)flag); // TODO: int vs long...

    env->ReleaseStringUTFChars(native_file_geometry, name);
}

extern "C"
JNIEXPORT jint JNICALL
        Java_org_apertusvr_ApertusJNI_getFileGeometryVisibilityFlag(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    int visibilityFlag = -1; // TODO: int vs long...
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        visibilityFlag = fileGeometryShared->getVisibilityFlag();

    env->ReleaseStringUTFChars(native_file_geometry, name);

    return (jint) visibilityFlag;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileGeometryOwner(JNIEnv *env, jclass clazz, jstring native_file_geometry, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);
    const char* ownerID = env->GetStringUTFChars(owner_id, NULL);

    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->setOwner(std::string(ownerID));

    env->ReleaseStringUTFChars(native_file_geometry, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileGeometryOwner(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    const char* ownerID = "";
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        ownerID = fileGeometryShared->getOwner().c_str();

    env->ReleaseStringUTFChars(native_file_geometry, name);

    return env->NewStringUTF(ownerID);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_org_apertusvr_ApertusJNI_getFileGeometryUnitScale(JNIEnv *env, jclass clazz, jstring native_file_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    float unitScale = 1.f;
    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        unitScale = fileGeometryShared->getUnitScale();

    env->ReleaseStringUTFChars(native_file_geometry, name);

    return (jfloat) unitScale;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileGeometryUnitScale(JNIEnv *env, jclass clazz, jstring native_file_geometry, jfloat unit_scale)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_geometry, NULL);

    if(auto fileGeometryShared = std::static_pointer_cast<ape::IFileGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileGeometryShared->setUnitScale(unit_scale);

    env->ReleaseStringUTFChars(native_file_geometry, name);
}