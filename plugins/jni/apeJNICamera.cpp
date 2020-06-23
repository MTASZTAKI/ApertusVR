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
#include "apeICamera.h"

extern "C"
JNIEXPORT jfloat JNICALL
Java_org_apertusvr_ApertusJNI_getCameraFocalLength(JNIEnv *env, jclass clazz,
                                                   jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    float focalLength = 0.0f;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        focalLength = cameraShared->getFocalLength();

    env->ReleaseStringUTFChars(native_camera,name);

    return (jfloat)focalLength;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraFocalLength(JNIEnv *env, jclass clazz, jstring native_camera,
                                                   jfloat focal_length) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setFocalLength(focal_length);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getCameraFrustumOffset(JNIEnv *env, jclass clazz,
                                                     jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    ape::Vector2 frustumOffset;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        frustumOffset = cameraShared->getFrustumOffset();

    env->ReleaseStringUTFChars(native_camera,name);

    float outArrayBuf[] = {frustumOffset.x, frustumOffset.y};
    jfloatArray jOutArray = env->NewFloatArray(2);
    env->SetFloatArrayRegion(jOutArray,0,2,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraFrustumOffset(JNIEnv *env, jclass clazz,
                                                     jstring native_camera, jfloat x, jfloat y) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setFrustumOffset(ape::Vector2(x,y));

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_org_apertusvr_ApertusJNI_getCameraFOVy(JNIEnv *env, jclass clazz, jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    float fovY = 0.f;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        fovY = cameraShared->getFOVy().radian;

    env->ReleaseStringUTFChars(native_camera,name);

    return (jfloat)fovY;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraFOVy(JNIEnv *env, jclass clazz, jstring native_camera,
                                            jfloat fov_y) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setFOVy(ape::Radian(fov_y));

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_org_apertusvr_ApertusJNI_getCameraNearClipDistance(JNIEnv *env, jclass clazz,
                                                        jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    float nearClipDistance = 0.f;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        nearClipDistance = cameraShared->getNearClipDistance();

    env->ReleaseStringUTFChars(native_camera,name);

    return (jfloat)nearClipDistance;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraNearClipDistance(JNIEnv *env, jclass clazz,
                                                        jstring native_camera,
                                                        jfloat near_clip_distance) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setNearClipDistance(near_clip_distance);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_org_apertusvr_ApertusJNI_getCameraFarClipDistance(JNIEnv *env, jclass clazz,
                                                       jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    float farClipDistance = 0.f;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        farClipDistance = cameraShared->getFarClipDistance();

    env->ReleaseStringUTFChars(native_camera,name);

    return (jfloat)farClipDistance;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraFarClipDistance(JNIEnv *env, jclass clazz,
                                                       jstring native_camera,
                                                       jfloat far_clip_distance) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setFarClipDistance(far_clip_distance);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_org_apertusvr_ApertusJNI_getCameraAspectRatio(JNIEnv *env, jclass clazz,
                                                   jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    float aspectRatio = 0.f;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        aspectRatio = cameraShared->getAspectRatio();

    env->ReleaseStringUTFChars(native_camera,name);

    return (jfloat)aspectRatio;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraAspectRatio(JNIEnv *env, jclass clazz, jstring native_camera,
                                                   jfloat aspect_ratio) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setAspectRatio(aspect_ratio);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraAutoAspectRatio(JNIEnv *env, jclass clazz,
                                                       jstring native_camera, jboolean enable) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setAutoAspectRatio(enable);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isCameraAutoAspectRatio(JNIEnv *env, jclass clazz,
                                                      jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    bool isAutoAspectRatio = false;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        isAutoAspectRatio = cameraShared->isAutoAspectRatio();

    env->ReleaseStringUTFChars(native_camera,name);

    return (jboolean)isAutoAspectRatio;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getCameraProjection(JNIEnv *env, jclass clazz,
                                                  jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    ape::Matrix4 projMx;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        projMx = cameraShared->getProjection();

    env->ReleaseStringUTFChars(native_camera,name);

    float outArrayBuf[] =
            {
                projMx.m[0][0], projMx.m[0][1], projMx.m[0][2], projMx.m[0][3],
                projMx.m[1][0], projMx.m[1][1], projMx.m[1][2], projMx.m[1][3],
                projMx.m[2][0], projMx.m[2][1], projMx.m[2][2], projMx.m[2][3],
                projMx.m[3][0], projMx.m[3][1], projMx.m[3][2], projMx.m[3][3],
            };
    jfloatArray jOutArray = env->NewFloatArray(16);
    env->SetFloatArrayRegion(jOutArray,0,16,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraProjection(JNIEnv *env, jclass clazz, jstring native_camera,
                                                  jfloatArray projection) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    jfloat* projMxCArray = env->GetFloatArrayElements(projection,NULL);
    if(projMxCArray == NULL) return;

    ape::Matrix4 projMx = ape::Matrix4(
            projMxCArray[0],  projMxCArray[1],  projMxCArray[2],  projMxCArray[3],
            projMxCArray[4],  projMxCArray[5],  projMxCArray[6],  projMxCArray[7],
            projMxCArray[8],  projMxCArray[9],  projMxCArray[10], projMxCArray[11],
            projMxCArray[12], projMxCArray[13], projMxCArray[14], projMxCArray[15]
    );

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setProjection(projMx);

    env->ReleaseFloatArrayElements(projection,projMxCArray,0);
    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraParentNode(JNIEnv *env, jclass clazz, jstring native_camera,
                                                  jstring native_parent_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);
    const char* parentName = env->GetStringUTFChars(native_parent_node,NULL);

    ape::NodeWeakPtr parentWeak = jniPlugin->getSceneManager()->getNode(parentName);
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setParentNode(parentWeak);

    env->ReleaseStringUTFChars(native_camera, name);
    env->ReleaseStringUTFChars(native_parent_node, parentName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCameraParentNode(JNIEnv *env, jclass clazz,
                                                  jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    ape::NodeWeakPtr parentWeak;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        parentWeak = cameraShared->getParentNode();

    env->ReleaseStringUTFChars(native_camera,name);

    const char* parent_name = "";
    if(auto parentShared = parentWeak.lock())
        parent_name = parentShared->getName().c_str();

    return env->NewStringUTF(parent_name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraProjectionType(JNIEnv *env, jclass clazz,
                                                      jstring native_camera, jint projection_type) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setProjectionType((ape::Camera::ProjectionType)projection_type);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getCameraProjectionType(JNIEnv *env, jclass clazz,
                                                      jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    int projection_type = -1;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        projection_type = cameraShared->getProjectionType();

    env->ReleaseStringUTFChars(native_camera,name);

    return (jint) projection_type;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraOrthoWindowSize(JNIEnv *env, jclass clazz,
                                                       jstring native_camera, jfloat width,
                                                       jfloat height) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setOrthoWindowSize(width,height);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getCameraOrthoWindowSize(JNIEnv *env, jclass clazz,
                                                       jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    ape::Vector2 orthoWindowSize;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        orthoWindowSize = cameraShared->getOrthoWindowSize();

    env->ReleaseStringUTFChars(native_camera,name);

    float outArrayBuf[] = {orthoWindowSize.x, orthoWindowSize.y};
    jfloatArray jOutArray = env->NewFloatArray(2);
    env->SetFloatArrayRegion(jOutArray,0,2,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraWindow(JNIEnv *env, jclass clazz, jstring native_camera,
                                              jstring window) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);
    const char* window_c_str = env->GetStringUTFChars(window,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setWindow(std::string(window_c_str));

    env->ReleaseStringUTFChars(native_camera, name);
    env->ReleaseStringUTFChars(window, window_c_str);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setCameraVisibilityMask(JNIEnv *env, jclass clazz,
                                                      jstring native_camera, jint mask) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cameraShared->setVisibilityMask((unsigned int) mask);

    env->ReleaseStringUTFChars(native_camera,name);
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getCameraVisibilityMask(JNIEnv *env, jclass clazz,
                                                      jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    int mask = -1;
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        mask = cameraShared->getVisibilityMask();

    env->ReleaseStringUTFChars(native_camera,name);

    return (jint) mask;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getCameraWindow(JNIEnv *env, jclass clazz, jstring native_camera) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_camera,NULL);

    const char* cam_window = "";
    if(auto cameraShared = std::static_pointer_cast<ape::ICamera>(jniPlugin->getSceneManager()->getEntity(std::string(name)).lock()))
        cam_window = cameraShared->getWindow().c_str();

    env->ReleaseStringUTFChars(native_camera,name);

    return env->NewStringUTF(cam_window);
}
