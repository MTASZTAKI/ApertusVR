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
#include "apeIPlaneGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryParameters(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, float num_seg_x, float num_seg_y,
        float size_x, float size_y, float tile_x, float tile_y)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);

    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        planeGeometryShared->setParameters(
                ape::Vector2(num_seg_x, num_seg_y),
                ape::Vector2(size_x, size_y),
                ape::Vector2(tile_x, tile_y)
                );
    }

    env->ReleaseStringUTFChars(native_plane_geometry, name);

}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryNumSeg(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);

    ape::GeometryPlaneParameters parameters;
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        parameters = planeGeometryShared->getParameters();

    env->ReleaseStringUTFChars(native_plane_geometry, name);

    float numSegArrayBuf[] = {parameters.numSeg.x, parameters.numSeg.y};
    jfloatArray jNumSegArray = env->NewFloatArray(2);
    env->SetFloatArrayRegion(jNumSegArray,0,2,numSegArrayBuf);

    return jNumSegArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometrySize(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);

    ape::GeometryPlaneParameters parameters;
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        parameters = planeGeometryShared->getParameters();

    env->ReleaseStringUTFChars(native_plane_geometry, name);

    float sizeArrayBuf[] = {parameters.size.x, parameters.size.y};
    jfloatArray jSizeArray = env->NewFloatArray(2);
    env->SetFloatArrayRegion(jSizeArray,0,2,sizeArrayBuf);

    return jSizeArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryTile(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);

    ape::GeometryPlaneParameters parameters;
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        parameters = planeGeometryShared->getParameters();

    env->ReleaseStringUTFChars(native_plane_geometry, name);

    float tileArrayBuf[] = {parameters.tile.x, parameters.tile.y};
    jfloatArray jTileArray = env->NewFloatArray(2);
    env->SetFloatArrayRegion(jTileArray,0,2,tileArrayBuf);

    return jTileArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryParameters(JNIEnv *env, jclass clazz, jstring native_plane_geometry) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);

    ape::GeometryPlaneParameters parameters;
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        parameters = planeGeometryShared->getParameters();

    env->ReleaseStringUTFChars(native_plane_geometry, name);

    float parametersArrayBuf[] =
            {
                parameters.numSeg.x, parameters.numSeg.y,
                parameters.size.x, parameters.size.y,
                parameters.tile.x, parameters.tile.y
            };
    jfloatArray jParametersArray = env->NewFloatArray(6);
    env->SetFloatArrayRegion(jParametersArray,0,2, parametersArrayBuf);

    return jParametersArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryParentNode(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, jstring parent_node_name)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);
    const char* parentName = env->GetStringUTFChars(parent_node_name, NULL);

    ape::NodeWeakPtr parentWeak = jniPlugin->getSceneManager()->getNode(std::string(name));
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        planeGeometryShared->setParentNode(parentWeak);

    env->ReleaseStringUTFChars(native_plane_geometry, name);
    env->ReleaseStringUTFChars(parent_node_name, parentName);

}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryMaterial(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, jstring native_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);
    const char* materialName = env->GetStringUTFChars(native_material, NULL);

    ape::MaterialWeakPtr materialWeak = std::static_pointer_cast<ape::Material>(jniPlugin->getSceneManager()->getEntity(std::string(materialName)).lock());
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        planeGeometryShared->setMaterial(materialWeak);

    env->ReleaseStringUTFChars(native_plane_geometry, name);
    env->ReleaseStringUTFChars(native_material, materialName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);

    ape::MaterialWeakPtr materialWeak;
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        materialWeak = planeGeometryShared->getMaterial();

    env->ReleaseStringUTFChars(native_plane_geometry, name);

    const char* materialName = "";
    if(auto materialShared = materialWeak.lock())
        materialName = materialShared->getName().c_str();

    return env->NewStringUTF(materialName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryOwner(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, jstring native_owner)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);
    const char* ownerName = env->GetStringUTFChars(native_owner, NULL);

    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        planeGeometryShared->setOwner(std::string(ownerName));

    env->ReleaseStringUTFChars(native_plane_geometry, name);
    env->ReleaseStringUTFChars(native_owner, ownerName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryOwner(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_plane_geometry,NULL);

    const char* ownerName = "";
    if(auto planeGeometryShared = std::static_pointer_cast<ape::IPlaneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        ownerName = planeGeometryShared->getOwner().c_str();

    env->ReleaseStringUTFChars(native_plane_geometry, name);

    return env->NewStringUTF(ownerName);
}