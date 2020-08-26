#include "apeJNIPlugin.h"
#include "apeITextGeometry.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getTextGeometryCaption(JNIEnv *env, jclass clazz, jstring native_text_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);

    const char* caption = jniPlugin->NA_STR;
    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        caption = textGeometryShared->getCaption().c_str();
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);

    return env->NewStringUTF(caption);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTextGeometryCaption(JNIEnv *env, jclass clazz, jstring native_text_geometry, jstring caption)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);
    const char* textCaption = env->GetStringUTFChars(caption,nullptr);

    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        textGeometryShared->setCaption(std::string(textCaption));
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);
    env->ReleaseStringUTFChars(caption, textCaption);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_clearTextGeometryCaption(JNIEnv *env, jclass clazz, jstring native_text_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);

    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        textGeometryShared->clearCaption();
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isTextGeometryVisible(JNIEnv *env, jclass clazz, jstring native_text_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);

    bool isVisible = false;
    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        isVisible = textGeometryShared->isVisible();
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);

    return (jboolean) isVisible;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTextGeometryVisible(JNIEnv *env, jclass clazz, jstring native_text_geometry, jboolean enabled)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);

    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        textGeometryShared->setVisible((bool) enabled);
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTextGeometryParentNode(JNIEnv *env, jclass clazz, jstring native_text_geometry, jstring parent_node)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);
    const char* parentNodeName = env->GetStringUTFChars(parent_node, nullptr);

    ape::NodeWeakPtr parentNodeWeak = jniPlugin->getSceneManager()->getNode(std::string(parentNodeName)).lock();
    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        textGeometryShared->setParentNode(parentNodeWeak);
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);
    env->ReleaseStringUTFChars(parent_node, parentNodeName);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_showTextGeometryOnTop(JNIEnv *env, jclass clazz, jstring native_text_geometry, jboolean show)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);

    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        textGeometryShared->showOnTop((bool) show);
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isTextGeometryShownOnTop(JNIEnv *env, jclass clazz, jstring native_text_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);

    bool shown = false;
    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        shown = textGeometryShared->isShownOnTop();
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);

    return (jboolean) shown;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setTextGeometryOwner(JNIEnv *env, jclass clazz, jstring native_text_geometry, jstring owner_id)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);
    const char* ownerID = env->GetStringUTFChars(owner_id, nullptr);

    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        textGeometryShared->setOwner(ownerID);
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);
    env->ReleaseStringUTFChars(owner_id, ownerID);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getTextGeometryOwner(JNIEnv *env, jclass clazz, jstring native_text_geometry)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    const char* textGeometryName = env->GetStringUTFChars(native_text_geometry, nullptr);

    const char* owner_id = jniPlugin->NA_STR;
    if(auto textGeometryShared = std::static_pointer_cast<ape::ITextGeometry>(
            jniPlugin->getSceneManager()->getEntity(std::string(textGeometryName)).lock()))
    {
        owner_id = textGeometryShared->getOwner().c_str();
    }

    env->ReleaseStringUTFChars(native_text_geometry, textGeometryName);

    return env->NewStringUTF(owner_id);
}

