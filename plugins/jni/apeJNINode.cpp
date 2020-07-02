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
#include "apeINode.h"
#include <queue>

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isNodeValid(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    bool valid = false;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        valid = true;

    env->ReleaseStringUTFChars(native_node,name);

    return (jboolean) valid;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodePosition(JNIEnv *env, jclass clazz, jstring native_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Vector3 position;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        position = nodeShared->getPosition();

    env->ReleaseStringUTFChars(native_node,name);

    float positionArray[] = {position.x,position.y,position.z};
    jfloatArray jPositionArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jPositionArray,0,3,positionArray);

    return jPositionArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeDerivedPosition(JNIEnv *env, jclass clazz, jstring native_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Vector3 position;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        position = nodeShared->getDerivedPosition();

    env->ReleaseStringUTFChars(native_node,name);

    float positionArray[] = {position.x,position.y,position.z};
    jfloatArray jPositionArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jPositionArray,0,3,positionArray);

    return jPositionArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeOrientation(JNIEnv *env, jclass clazz, jstring native_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Quaternion orientation;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        orientation = nodeShared->getOrientation();

    env->ReleaseStringUTFChars(native_node,name);

    float orientationArray[] = {orientation.w, orientation.x, orientation.y, orientation.z};
    jfloatArray  jOrientationArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jOrientationArray,0,4,orientationArray);

    return jOrientationArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeDerivedOrientation(JNIEnv *env, jclass clazz,
                                                        jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Quaternion orientation;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        orientation = nodeShared->getDerivedOrientation();

    env->ReleaseStringUTFChars(native_node,name);

    float orientationArray[] = {orientation.w, orientation.x, orientation.y, orientation.z};
    jfloatArray  jOrientationArray = env->NewFloatArray(4);
    env->SetFloatArrayRegion(jOrientationArray,0,4,orientationArray);

    return jOrientationArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeScale(JNIEnv *env, jclass clazz, jstring native_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Vector3 scale;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        scale = nodeShared->getScale();

    env->ReleaseStringUTFChars(native_node,name);

    float outArrayBuf[] = {scale.x,scale.y,scale.z};
    jfloatArray jOutArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jOutArray,0,3,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeDerivedScale(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Vector3 derivedScale;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        derivedScale = nodeShared->getDerivedScale();

    env->ReleaseStringUTFChars(native_node,name);

    float outArrayBuf[] = {derivedScale.x,derivedScale.y,derivedScale.z};
    jfloatArray jOutArray = env->NewFloatArray(3);
    env->SetFloatArrayRegion(jOutArray,0,3,outArrayBuf);

    return jOutArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeModelMatrix(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Matrix4 modelMx;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        modelMx = nodeShared->getModelMatrix();

    env->ReleaseStringUTFChars(native_node, name);

    float outArrayBuf[] =
            {
                modelMx.m[0][0], modelMx.m[0][1], modelMx.m[0][2], modelMx.m[0][3],
                modelMx.m[1][0], modelMx.m[1][1], modelMx.m[1][2], modelMx.m[1][3],
                modelMx.m[2][0], modelMx.m[2][1], modelMx.m[2][2], modelMx.m[2][3],
                modelMx.m[3][0], modelMx.m[3][1], modelMx.m[3][2], modelMx.m[3][3],
            };

    jfloatArray jOutArray = env->NewFloatArray(16);
    env->SetFloatArrayRegion(jOutArray,0,16,outArrayBuf);
    return jOutArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeDerivedModelMatrix(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::Matrix4 derivedModelMx;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        derivedModelMx = nodeShared->getDerivedModelMatrix();

    env->ReleaseStringUTFChars(native_node, name);

    float outArrayBuf[] =
            {
                derivedModelMx.m[0][0], derivedModelMx.m[0][1], derivedModelMx.m[0][2], derivedModelMx.m[0][3],
                derivedModelMx.m[1][0], derivedModelMx.m[1][1], derivedModelMx.m[1][2], derivedModelMx.m[1][3],
                derivedModelMx.m[2][0], derivedModelMx.m[2][1], derivedModelMx.m[2][2], derivedModelMx.m[2][3],
                derivedModelMx.m[3][0], derivedModelMx.m[3][1], derivedModelMx.m[3][2], derivedModelMx.m[3][3],
            };

    jfloatArray jOutArray = env->NewFloatArray(16);
    env->SetFloatArrayRegion(jOutArray,0,16,outArrayBuf);
    return jOutArray;
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_getNodeChildrenVisibility(JNIEnv *env, jclass clazz,
                                                        jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    bool visibility = false;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        visibility = nodeShared->getChildrenVisibility();

    env->ReleaseStringUTFChars(native_node,name);

    return (jboolean) visibility;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isNodeVisible(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    bool visibility = false;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        visibility = nodeShared->isVisible();

    env->ReleaseStringUTFChars(native_node,name);

    return (jboolean) visibility;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isNodeFixedYaw(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    bool fixedYaw = false;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        fixedYaw = nodeShared->isFixedYaw();

    env->ReleaseStringUTFChars(native_node,name);

    return (jboolean) fixedYaw;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeParentNode(JNIEnv *env, jclass clazz, jstring native_node,
                                                jstring native_parent_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);
    const char* parentName = env->GetStringUTFChars(native_parent_node,NULL);

    ape::NodeWeakPtr parentWeak = jniPlugin->getSceneManager()->getNode(parentName);
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setParentNode(parentWeak);

    env->ReleaseStringUTFChars(native_node,name);
    env->ReleaseStringUTFChars(native_parent_node,parentName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_detachNodeFromParentNode(JNIEnv *env, jclass clazz,
                                                       jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->detachFromParentNode();

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getNodeParentNode(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    ape::NodeWeakPtr parentWeak;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        parentWeak = nodeShared->getParentNode();

    env->ReleaseStringUTFChars(native_node,name);

    const char *parent_name = "";
    if(auto parentShared = parentWeak.lock())
        parent_name = parentShared->getName().c_str();

    return env->NewStringUTF(parent_name);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_apertusvr_ApertusJNI_getNodeChildNodes(JNIEnv *env, jclass clazz, jstring native_node)
{
    // TODO: implement getNodeChildNodes()
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    jclass jEntityCls = env->FindClass("org/apertusvr/apeEntity");
    jmethodID jEntityConstrMID = env->GetMethodID(jEntityCls, "<init>","(java/lang/String)V");

    std::vector<ape::NodeWeakPtr> childNodes;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        childNodes = nodeShared->getChildNodes();

    int childNodeCount = childNodes.size();
    jobjectArray jOutArray = env->NewObjectArray(childNodeCount,jEntityCls,NULL);

    for(int i = 0; i < childNodeCount; ++i)
    {
        const char* childName = "";
        if(auto childNodeShared = childNodes[i].lock())
            childName = childNodeShared->getName().c_str();

        env->SetObjectArrayElement(jOutArray,i,env->NewObject(jEntityCls,jEntityConstrMID,env->NewStringUTF(childName)));
    }

    return jOutArray;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_hasNodeChildNodes(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    bool hasChildNodes = false;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        hasChildNodes = nodeShared->hasChildNode();

    env->ReleaseStringUTFChars(native_node,name);

    return (jboolean) hasChildNodes;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isNodeChildNode(JNIEnv *env, jclass clazz, jstring native_node, jstring native_child_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);
    const char* child_name = env->GetStringUTFChars(native_child_node,NULL);

    bool isChildNode = false;
    ape::NodeWeakPtr childWeak = jniPlugin->getSceneManager()->getNode(child_name);
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        isChildNode = nodeShared->isChildNode(childWeak);

    env->ReleaseStringUTFChars(native_node,name);
    env->ReleaseStringUTFChars(native_child_node,child_name);

    return (jboolean) isChildNode;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodePosition(JNIEnv *env, jclass clazz, jstring native_node,jfloat x, jfloat y, jfloat z)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setPosition(ape::Vector3(x, y, z));

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeOrientation(JNIEnv *env, jclass clazz, jstring native_node, jfloat w, jfloat x, jfloat y, jfloat z)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setOrientation(ape::Quaternion(w,x,y,z));

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeScale(JNIEnv *env, jclass clazz, jstring native_node, jfloat x,
                                           jfloat y, jfloat z) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setScale(ape::Vector3(x,y,z));

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_translateNode(JNIEnv *env, jclass clazz, jstring native_node,
                                            jfloat x, jfloat y, jfloat z, jint transform_space) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->translate(ape::Vector3(x,y,z),ape::Node::TransformationSpace(transform_space));

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_rotateNode(JNIEnv *env, jclass clazz, jstring native_node,
                                         jfloat angle, jfloat x, jfloat y, jfloat z,
                                         jint transform_space) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->rotate(angle,ape::Vector3(x,y,z),(ape::Node::TransformationSpace)transform_space);

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeChildrenVisibility(JNIEnv *env, jclass clazz,
                                                        jstring native_node, jboolean visible) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setChildrenVisibility(visible);

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeFixedYaw(JNIEnv *env, jclass clazz, jstring native_node,
                                              jboolean fix) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setFixedYaw(fix);

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeInheritOrientation(JNIEnv *env, jclass clazz,
                                                        jstring native_node, jboolean enable) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setInheritOrientation(enable);

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isNodeInheritOrientation(JNIEnv *env, jclass clazz,
                                                       jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    bool isInheritOrientation = false;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->isInheritOrientation();

    env->ReleaseStringUTFChars(native_node,name);

    return (jboolean) isInheritOrientation;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeInitialState(JNIEnv *env, jclass clazz, jstring native_node) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setInitalState();

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_showNodeBoundingBox(JNIEnv *env, jclass clazz, jstring native_node,
                                                  jboolean show) {
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->showBoundingBox(show);

    env->ReleaseStringUTFChars(native_node,name);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isNodeReplicated(JNIEnv* env, jclass clazz, jstring native_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    bool isReplicated = false;
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        isReplicated = nodeShared->isReplicated();

    env->ReleaseStringUTFChars(native_node,name);

    return (jboolean) isReplicated;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setNodeOwner(JNIEnv* env, jclass clazz, jstring native_node, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);
    const char* ownerID = env->GetStringUTFChars(owner_id,NULL);

    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeShared->setOwner(std::string(ownerID));

    env->ReleaseStringUTFChars(native_node, name);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getNodeOwner(JNIEnv* env, jclass clazz, jstring native_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    const char* nodeID = "";
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeID = nodeShared->getOwner().c_str();

    env->ReleaseStringUTFChars(native_node,name);

    return env->NewStringUTF(nodeID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getNodeCreator(JNIEnv* env, jclass clazz, jstring native_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* name = env->GetStringUTFChars(native_node,NULL);

    const char* nodeCreator = "";
    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
        nodeCreator = nodeShared->getCreator().c_str();

    env->ReleaseStringUTFChars(native_node,name);

    return env->NewStringUTF(nodeCreator);
}

//extern "C"
//JNIEXPORT jlongArray JNICALL
//Java_org_apertusvr_ApertusJNI_getNodeRelatedGeometries(JNIEnv *env, jclass clazz, jstring native_node)
//{
//
//
//    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
//    auto* nodeGeometryMap = jniPlugin->getNodeGeometryMap();
//    const char* name = env->GetStringUTFChars(native_node,NULL);
//
//    std::vector<jlong> relatedGeometries;
//    std::queue<ape::NodeWeakPtr> nodeQueue;
//
//    if(auto nodeShared = jniPlugin->getSceneManager()->getNode(std::string(name)).lock())
//        nodeQueue.push(nodeShared);
//
//    env->ReleaseStringUTFChars(native_node,name);
//
//    // TODO: recursive traversal without queue
//
//    while(!nodeQueue.empty())
//    {
//        ape::NodeWeakPtr frontWeak = nodeQueue.front();
//        if(auto frontShared = frontWeak.lock())
//        {
//            auto childGeomIt = nodeGeometryMap->find(frontShared->getName());
//            if (childGeomIt != nodeGeometryMap->end())
//            {
//                ape::GeometryWeakPtr* pChildGeomWeak = &(childGeomIt->second);
//                relatedGeometries.push_back((jlong) pChildGeomWeak);
//            }
//
//            if(frontShared->hasChildNode())
//            {
//                for(const auto& childNodeWeak : frontShared->getChildNodes())
//                    nodeQueue.push(childNodeWeak);
//            }
//        }
//
//        nodeQueue.pop();
//    }
//
//    jlongArray jOutArray = env->NewLongArray(relatedGeometries.size());
//    env->SetLongArrayRegion(jOutArray,0,relatedGeometries.size(),&relatedGeometries[0]);
//
//    return jOutArray;
//}
