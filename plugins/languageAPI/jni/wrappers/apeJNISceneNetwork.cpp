#include "apeISceneNetwork.h"
#include "apeJNIPlugin.h"

extern "C"
JNIEXPORT jint JNICALL
Java_org_apertusvr_ApertusJNI_getSceneNetworkParticipantType(JNIEnv *env, jclass clazz)
{
    auto mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
    return (jint) mpSceneNetwork->getParticipantType();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_org_apertusvr_ApertusJNI_isSceneNetworkRoomRunning(JNIEnv *env, jclass clazz, jstring room_name)
{
    auto mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
    const char* roomName = env->GetStringUTFChars(room_name,nullptr);

    jboolean isRoomRunning = (jboolean) mpSceneNetwork->isRoomRunning(std::string(roomName));

    env->ReleaseStringUTFChars(room_name,roomName);

    return isRoomRunning;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_connectSceneNetworkToRoom(JNIEnv *env, jclass clazz, jstring room_name)
{
    auto mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
    const char* roomName = env->GetStringUTFChars(room_name,nullptr);

    mpSceneNetwork->connectToRoom(roomName,std::vector<std::string>(),std::vector<std::string>());

    env->ReleaseStringUTFChars(room_name, roomName);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getSceneNetworkCurrentRoomName(JNIEnv *env, jclass clazz)
{
    auto mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();

    const char* currentRoomName = mpSceneNetwork->getCurrentRoomName().c_str();

    return env->NewStringUTF(currentRoomName);
}
