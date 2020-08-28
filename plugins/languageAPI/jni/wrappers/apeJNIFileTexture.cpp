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
#include "apeIFileTexture.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileTextureFileName(JNIEnv* env, jclass clazz, jstring native_file_texture, jstring file_name)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_texture, NULL);
    const char* fileName = env->GetStringUTFChars(file_name, NULL);

    if(auto fileTextureShared = std::static_pointer_cast<ape::IFileTexture>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileTextureShared->setFileName(std::string(fileName));

    env->ReleaseStringUTFChars(native_file_texture, name);
    env->ReleaseStringUTFChars(file_name, fileName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileTextureFileName(JNIEnv* env, jclass clazz, jstring native_file_texture)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_texture,NULL);

    const char* fileName = "";
    if(auto fileTextureShared = std::static_pointer_cast<ape::IFileTexture>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileName = fileTextureShared->getFileName().c_str();

    env->ReleaseStringUTFChars(native_file_texture, name);

    return env->NewStringUTF(fileName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileTextureMapType(JNIEnv* env, jclass clazz, jstring native_file_texture, jint map_type)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_texture,NULL);

    if(auto fileTextureShared = std::static_pointer_cast<ape::IFileTexture>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileTextureShared->setMapType(ape::IFileTexture::MapType(map_type));

    env->ReleaseStringUTFChars(native_file_texture, name);
}

extern "C"
JNIEXPORT int JNICALL
Java_org_apertusvr_ApertusJNI_getFileTextureMapType(JNIEnv* env, jclass clazz, jstring native_file_texture)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_texture,NULL);

    int mapType = -1;
    if(auto fileTextureShared = std::static_pointer_cast<ape::IFileTexture>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        mapType = fileTextureShared->getMapType();

    env->ReleaseStringUTFChars(native_file_texture, name);

    return (jint) mapType;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileTextureOwner(JNIEnv* env, jclass clazz, jstring native_file_texture, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_texture,NULL);
    const char* ownerID = env->GetStringUTFChars(owner_id, NULL);

    if(auto fileTextureShared = std::static_pointer_cast<ape::IFileTexture>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileTextureShared->setOwner(std::string(ownerID));

    env->ReleaseStringUTFChars(native_file_texture, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileTextureOwner(JNIEnv* env, jclass clazz, jstring native_file_texture)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_texture,NULL);

    const char* ownerID = "";
    if(auto fileTextureShared = std::static_pointer_cast<ape::IFileTexture>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        ownerID = fileTextureShared->getOwner().c_str();

    env->ReleaseStringUTFChars(native_file_texture, name);

    return env->NewStringUTF(ownerID);
}
