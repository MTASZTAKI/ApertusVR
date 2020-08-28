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
#include "apeGeometry.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_geometry,NULL);

    ape::NodeWeakPtr parentWeak;
    if(auto geometryShared = std::static_pointer_cast<ape::Geometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        parentWeak = geometryShared->getParentNode();

    env->ReleaseStringUTFChars(native_geometry,name);

    const char* parent_name = "";
    if(auto parentShared = parentWeak.lock())
        parent_name = parentShared->getName().c_str();

    return env->NewStringUTF(parent_name);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isGeometryIntersectionEnabled(JNIEnv *env, jclass clazz, jstring native_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_geometry,NULL);

    bool isIntersectionEnabled = false;
    if(auto geometryShared = std::static_pointer_cast<ape::Geometry>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        isIntersectionEnabled = geometryShared->isIntersectingEnabled();

    env->ReleaseStringUTFChars(native_geometry,name);

    return (jboolean) isIntersectionEnabled;
}

