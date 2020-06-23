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
#include "apeColor.h"
#include "apeILight.h"

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getLightType(JNIEnv *env, jclass clazz, jstring native_light)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    int typeAsInt = 0;
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        typeAsInt = lightShared->getLightType();

    env->ReleaseStringUTFChars(native_light,name);

    return (jint)typeAsInt;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getLightDiffuseColor(JNIEnv *env, jclass clazz, jstring native_light)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    ape::Color diffuse;
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        diffuse = lightShared->getDiffuseColor();

    env->ReleaseStringUTFChars(native_light,name);

    float outArrayBuf[] = {diffuse.r, diffuse.g, diffuse.b, diffuse.a};
    jfloatArray jOutArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jOutArray,0,4,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getLightSpecularColor(JNIEnv *env, jclass clazz, jstring native_light)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    ape::Color specular;
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        specular = lightShared->getSpecularColor();

    env->ReleaseStringUTFChars(native_light,name);

    float outArrayBuf[] = {specular.r,specular.g,specular.b,specular.a};
    jfloatArray jOutArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jOutArray,0,4,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getLightSpotRange(JNIEnv *env, jclass clazz, jstring native_light) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    ape::LightSpotRange spotRange;
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        spotRange = lightShared->getLightSpotRange();

    env->ReleaseStringUTFChars(native_light,name);

    float outArrayBuf[] = {spotRange.innerAngle.degree, spotRange.outerAngle.degree, spotRange.falloff};
    jfloatArray  jOutArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jOutArray,0,3,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getLightAttenuation(JNIEnv *env, jclass clazz, jstring native_light) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    ape::LightAttenuation lightAttenuation;
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightAttenuation = lightShared->getLightAttenuation();

    env->ReleaseStringUTFChars(native_light,name);

    float outArrayBuf[] = {lightAttenuation.range, lightAttenuation.constant, lightAttenuation.linear, lightAttenuation.quadratic};
    jfloatArray jOutArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jOutArray,0,4,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getLightDirection(JNIEnv *env, jclass clazz, jstring native_light)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    ape::Vector3 lightDirection;
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightDirection = lightShared->getLightDirection();

    env->ReleaseStringUTFChars(native_light,name);

    float outArrayBuf[] = {lightDirection.x,lightDirection.y,lightDirection.z};
    jfloatArray jOutArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jOutArray,0,3,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setLightType(JNIEnv *env, jclass clazz, jstring native_light, jint light_type)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightShared->setLightType(ape::Light::Type(light_type));

    env->ReleaseStringUTFChars(native_light,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setLightDiffuseColor(JNIEnv *env, jclass clazz, jstring native_light,
                                                   jfloat r, jfloat g, jfloat b, jfloat a)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightShared->setDiffuseColor(ape::Color(r,g,b,a));

    env->ReleaseStringUTFChars(native_light, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setLightSpecularColor(JNIEnv *env, jclass clazz, jstring native_light,
                                                    jfloat r, jfloat g, jfloat b, jfloat a) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightShared->setSpecularColor(ape::Color(r,g,b,a));

    env->ReleaseStringUTFChars(native_light,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setLightSpotRange(JNIEnv *env, jclass clazz, jstring native_light,
                                                jfloat inner_angle, jfloat outer_angle,
                                                jfloat falloff)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightShared->setLightSpotRange(ape::LightSpotRange(
                ape::Degree(inner_angle),
                ape::Degree(outer_angle),
                falloff));

    env->ReleaseStringUTFChars(native_light, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setLightAttenuation(JNIEnv *env, jclass clazz, jstring native_light,
                                                  jfloat range, jfloat constant, jfloat linear,
                                                  jfloat quadratic)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightShared->setLightAttenuation(ape::LightAttenuation(range, constant, linear, quadratic));

    env->ReleaseStringUTFChars(native_light,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setLightDirection(JNIEnv *env, jclass clazz, jstring native_light, jfloat x, jfloat y, jfloat z)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightShared->setLightDirection(ape::Vector3(x,y,z));

    env->ReleaseStringUTFChars(native_light,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setLightParentNode(JNIEnv *env, jclass clazz, jstring native_light, jstring parent_name)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);
    const char* parentName = env->GetStringUTFChars(parent_name,NULL);

    ape::NodeWeakPtr parentWeak = jniPlugin->getSceneManager()->getNode(std::string(parentName));
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        lightShared->setParentNode(parentWeak);

    env->ReleaseStringUTFChars(native_light, name);
    env->ReleaseStringUTFChars(parent_name, parentName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getLightParentNode(JNIEnv *env, jclass clazz, jstring native_light)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_light,NULL);

    ape::NodeWeakPtr parentWeak;
    if(auto lightShared = std::static_pointer_cast<ape::ILight>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        parentWeak = lightShared->getParentNode();

    env->ReleaseStringUTFChars(native_light,name);

    const char* parent_name = "";
    if(auto parentShared = parentWeak.lock())
        parent_name = parentShared->getName().c_str();

    return env->NewStringUTF(parent_name);
}