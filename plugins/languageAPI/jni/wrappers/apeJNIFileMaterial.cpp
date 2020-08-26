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
#include "apeIFileMaterial.h"
#include "apeTexture.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileMaterialFileName (JNIEnv *env, jclass clazz, jstring native_file_material, jstring file_name)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_material, NULL);
    const char* fileName = env->GetStringUTFChars(file_name, NULL);

    if(auto fileMaterialShared = std::static_pointer_cast<ape::IFileMaterial>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileMaterialShared->setFileName(std::string(fileName));

    env->ReleaseStringUTFChars(native_file_material, name);
    env->ReleaseStringUTFChars(file_name, fileName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileMaterialfFileName (JNIEnv *env, jclass clazz, jstring native_file_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_material, NULL);

    const char* fileName = "";
    if(auto fileMaterialShared = std::static_pointer_cast<ape::IFileMaterial>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileName = fileMaterialShared->getfFileName().c_str();

    env->ReleaseStringUTFChars(native_file_material, name);

    return env->NewStringUTF(fileName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileMaterialAsSkyBox(JNIEnv *env, jclass clazz, jstring native_file_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_material, NULL);

    if(auto fileMaterialShared = std::static_pointer_cast<ape::IFileMaterial>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileMaterialShared->setAsSkyBox();

    env->ReleaseStringUTFChars(native_file_material, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileMaterialTexture(JNIEnv *env, jclass clazz, jstring native_file_material, jstring texture)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_material, NULL);
    const char* textureName = env->GetStringUTFChars(texture, NULL);

    ape::TextureWeakPtr textureWeak = std::static_pointer_cast<ape::Texture>(jniPlugin->getSceneManager()->getEntity(std::string(textureName)).lock());
    if(auto fileMaterialShared = std::static_pointer_cast<ape::IFileMaterial>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileMaterialShared->setTexture(textureWeak);

    env->ReleaseStringUTFChars(native_file_material, name);
    env->ReleaseStringUTFChars(texture, textureName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileMaterialTexture(JNIEnv *env, jclass clazz, jstring native_file_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_material, NULL);

    ape::TextureWeakPtr textureWeak;
    if(auto fileMaterialShared = std::static_pointer_cast<ape::IFileMaterial>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        textureWeak = fileMaterialShared->getTexture();

    const char* textureName = "";
    if(auto textureShared = textureWeak.lock())
        textureName = textureShared->getName().c_str();

    env->ReleaseStringUTFChars(native_file_material, name);

    return env->NewStringUTF(textureName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setFileMaterialOwner(JNIEnv *env, jclass clazz, jstring native_file_material, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_material, NULL);
    const char* ownerID = env->GetStringUTFChars(owner_id, NULL);

    if(auto fileMaterialShared = std::static_pointer_cast<ape::IFileMaterial>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fileMaterialShared->setOwner(std::string(ownerID));

    env->ReleaseStringUTFChars(native_file_material, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getFileMaterialOwner(JNIEnv *env, jclass clazz, jstring native_file_material)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_file_material, NULL);

    const char* ownerID = "";
    if(auto fileMaterialShared = std::static_pointer_cast<ape::IFileMaterial>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        ownerID = fileMaterialShared->getOwner().c_str();

    env->ReleaseStringUTFChars(native_file_material, name);

    return env->NewStringUTF(ownerID);
}

