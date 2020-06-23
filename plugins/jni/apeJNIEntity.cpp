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
#include "apeEntity.h"

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getEntityType(JNIEnv *env, jclass clazz, jstring native_entity) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_entity,NULL);

    int type = ape::Entity::Type::INVALID;
    if(auto entityShared = jniPlugin->getSceneManager()->getEntity(std::string(name)).lock())
        type = entityShared->getType();

    env->ReleaseStringUTFChars(native_entity,name);

    return (jint) type;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isEntityValid(JNIEnv *env, jclass clazz, jstring native_entity) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_entity,NULL);

    bool valid = false;
    if(auto entityShared = jniPlugin->getSceneManager()->getEntity(std::string(name)).lock())
        valid = true;

    env->ReleaseStringUTFChars(native_entity,name);

    return (jboolean) valid;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isEntityValidWithType(JNIEnv *env, jclass clazz, jstring native_entity, jint type_as_int)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_entity,NULL);

    bool valid = false;
    if(auto entityShared = jniPlugin->getSceneManager()->getEntity(std::string(name)).lock())
        valid = ape::Entity::Type(type_as_int) == entityShared->getType();

    env->ReleaseStringUTFChars(native_entity,name);

    return (jboolean) valid;
}