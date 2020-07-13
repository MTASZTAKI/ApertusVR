#include <jni.h>
#include <string>
#include "apeSystem.h"
#include <android/log.h>
#include <unistd.h>
#include "pthread.h"
#include <functional>
#include <apeIEventManager.h>
#include <apeICoreConfig.h>
#include <apeISceneManager.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <iostream>
#include "../fopen/android_fopen.h"

void* print_message_function( void *ptr);
int start_logger(int* pfd, pthread_t& thr);
const char*android_log_tag = "cpplog";

int pfd[2];
pthread_t thr;


int start_logger(int* pfd, pthread_t& thr)
{
    // make stdout line-buffered, stderr unbuffered
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    // create the pipe and redirect stdout and stderr
    pipe(pfd);
    dup2(pfd[1],1); //stdout
    dup2(pfd[1],2); //stderr

    // spawn the logging thread with the pipe
    if(pthread_create(&thr, 0, print_message_function,pfd) == -1)
        return -1;
    pthread_detach(thr);

    return 0;
}


void* print_message_function( void *ptr )
{
    int* pfd = (int*) ptr;
    ssize_t rdsz;
    char buf[4096];

    for(;;)
    {
        rdsz = read(pfd[0], buf, sizeof(buf) - 1);
        if (rdsz > 0) {
            if (buf[rdsz - 1] == '\n') --rdsz;

            buf[rdsz] = 0;
            __android_log_print(ANDROID_LOG_INFO, android_log_tag, "%s", buf);
        }
    }

    return  0;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_startApertusVR(JNIEnv *env, jobject thiz, jstring conf_path, jobject assetManager)
{
    int pfd[2];
    pthread_t thr;
    start_logger(pfd,thr);

    android_fopen_set_asset_manager(AAssetManager_fromJava(env,assetManager));

    const char* confPath = env->GetStringUTFChars(conf_path,nullptr);

    ape::System::Start(confPath,true);

    env->ReleaseStringUTFChars(conf_path, confPath);

    printf("ape system started\n");
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_stopApertusVR(JNIEnv *env, jobject thiz) {
    ape::System::Stop();
}