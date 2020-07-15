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
#include "apeICloneGeometry.h"


extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCloneGeometrySourceGeometry(JNIEnv* env, jclass clazz, jstring native_clone_geometry, jstring source_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);
    const char* sourceGeometryName = env->GetStringUTFChars(source_geometry, nullptr);

    ape::GeometryWeakPtr sourceGeometry = std::static_pointer_cast<ape::Geometry>(jniPlugin->getSceneManager()->getEntity(std::string(sourceGeometryName)).lock());
    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cloneGeometryShared->setSourceGeometry(sourceGeometry);
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);
    env->ReleaseStringUTFChars(source_geometry, sourceGeometryName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCloneGeometrySourceGeometryGroupName(JNIEnv* env, jclass clazz, jstring native_clone_geometry, jstring group_geometry_name)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);
    const char* sourceGeometryGroupName = env->GetStringUTFChars(group_geometry_name, nullptr);

    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cloneGeometryShared->setSourceGeometryGroupName(std::string(sourceGeometryGroupName));
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCloneGeometryParentNode(JNIEnv* env, jclass clazz, jstring native_clone_geometry, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    ape::NodeWeakPtr parentNodeWeak = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName)).lock();
    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cloneGeometryShared->setParentNode(parentNodeWeak);
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCloneGeometrySourceGeometry(JNIEnv* env, jclass clazz, jstring native_clone_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);

    const char* sourceGeometryName = jniPlugin->NA_STR;
    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sourceGeometryName = cloneGeometryShared->getSourceGeometryName().c_str();
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);

    return env->NewStringUTF(sourceGeometryName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCloneGeometrySourceGeometryName(JNIEnv* env, jclass clazz, jstring native_clone_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);

    const char* sourceGeometryName = jniPlugin->NA_STR;
    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        sourceGeometryName = cloneGeometryShared->getSourceGeometryName().c_str();
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);

    return env->NewStringUTF(sourceGeometryName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCloneGeometrySourceGeometryGroupName(JNIEnv* env, jclass clazz, jstring native_clone_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);

    const char* cloneGeometryGroupName = jniPlugin->NA_STR;
    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cloneGeometryGroupName = cloneGeometryShared->getSourceGeometryGroupName().c_str();
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);

    return env->NewStringUTF(cloneGeometryGroupName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCloneGeometryOwner(JNIEnv* env, jclass clazz, jstring native_clone_geometry, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id, nullptr);

    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        cloneGeometryShared->setOwner(std::string(ownerID));
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCloneGeometryOwner(JNIEnv* env, jclass clazz, jstring native_clone_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_clone_geometry, nullptr);

    const char* ownerID = jniPlugin->NA_STR;
    if(auto cloneGeometryShared = std::static_pointer_cast<ape::ICloneGeometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
    {
        ownerID = cloneGeometryShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_clone_geometry, name);

    return env->NewStringUTF(ownerID);
}
