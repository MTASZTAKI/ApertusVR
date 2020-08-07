/*MIT License

Copyright (c) 2020 MTA SZTAKI

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
#include "apeISceneManager.h"

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_createSceneManagerNode(JNIEnv *env, jclass clazz, jstring name, jboolean replicate, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* nodeName = env->GetStringUTFChars(name,nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);

    bool success = false;
    if(auto node = jniPlugin->getSceneManager()->createNode(nodeName, replicate, std::string(ownerID)).lock())
        success = true;

    env->ReleaseStringUTFChars(name,nodeName);
    env->ReleaseStringUTFChars(owner_id, ownerID);

    return (jboolean) success;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_createSceneManagerEntity(JNIEnv *env, jclass clazz, jstring name, jint type, jboolean replicate, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* entityName = env->GetStringUTFChars(name,nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id,nullptr);
    auto entityType = (ape::Entity::Type) type;

    bool success = false;
    if(auto entity = jniPlugin->getSceneManager()->createEntity(entityName,entityType,replicate,ownerID).lock())
        success = true;

    env->ReleaseStringUTFChars(name, entityName);
    env->ReleaseStringUTFChars(owner_id, ownerID);

    return (jboolean) success;
}

//
//extern "C"
//JNIEXPORT void JNICALL
//Java_org_apertusvr_ApertusJNI_deleteSceneManagerNode(JNIEnv *env, jclass clazz, jstring name)
//{
//    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
//    const char* nodeName = env->GetStringUTFChars(name,nullptr);
//
//    jniPlugin->getSceneManager()->deleteEntity(nodeName);
//
//    env->ReleaseStringUTFChars(name,nodeName);
//}
//
//extern "C"
//JNIEXPORT void JNICALL
//Java_org_apertusvr_ApertusJNI_deleteSceneManagerEntity(JNIEnv *env, jclass clazz, jstring name)
//{
//
//}