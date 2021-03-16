#include <string>
#include <functional>
#include <iostream>
#include "apeSystem.h"
#include "apeIEventManager.h"
#include "apeICoreConfig.h"
#include "apeISceneManager.h"

//extern "C"
//JNIEXPORT void JNICALL
//Java_org_apertusvr_ApertusJNI_startApertusVR(JNIEnv *env, jclass clazz, jstring conf_path, jobject assetManager)
//{
//
//#if ANDROID_PIPE_LOG
//    int pfd[2];
//    pthread_t thr;
//    start_logger(pfd,thr);
//#endif
//
//    ape::AndroidAssetOpen::setAssetManager(AAssetManager_fromJava(env, assetManager));
//
//    const char* confPath = env->GetStringUTFChars(conf_path,nullptr);
//
//    ape::System::Start(confPath,true);
//
//    env->ReleaseStringUTFChars(conf_path, confPath);
//}
//
//extern "C"
//JNIEXPORT void JNICALL
//Java_org_apertusvr_ApertusJNI_stopApertusVR(JNIEnv *env, jclass clazz) {
//    ape::System::Stop();
//}