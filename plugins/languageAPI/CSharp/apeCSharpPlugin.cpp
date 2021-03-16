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

#include "../../core/eventManager/apeEventManagerImpl.h"
#include <map>
#include <apeColor.h>
#include "apeCSharpPlugin.h"

#include "apeIFileGeometry.h"

ape::CSharpPlugin::CSharpPlugin()
{
	APE_LOG_FUNC_ENTER()
    mpSceneManager = ape::ISceneManager::getSingletonPtr();
    mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
    mEventDoubleQueue = ape::DoubleQueue<Event>();
    mpEventManager = ape::IEventManager::getSingletonPtr();

    mpThisPlugin = this;

	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CLONE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&CSharpPlugin::eventCallBack, this, std::placeholders::_1));

	APE_LOG_FUNC_LEAVE()
}

void ape::CSharpPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	printf("CSharpPlugin init\n");
	APE_LOG_FUNC_LEAVE();
}

void ape::CSharpPlugin::eventCallBack(const ape::Event& event)
{
	mEventDoubleQueue.push(event);
}

void ape::CSharpPlugin::processEventDoubleQueue()
{

}

void ape::CSharpPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::CSharpPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::CSharpPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::CSharpPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::CSharpPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

ape::CSharpPlugin* ape::CSharpPlugin::mpThisPlugin;

ape::CSharpPlugin* ape::CSharpPlugin::getPluginPtr()
{
    return mpThisPlugin;
}

ape::ISceneManager *ape::CSharpPlugin::getSceneManager()
{
    return mpSceneManager;
}

ape::ICoreConfig *ape::CSharpPlugin::getCoreConfig()
{
    return mpCoreConfig;
}

ape::DoubleQueue<ape::Event> *ape::CSharpPlugin::getEventDoubleQueue()
{
    return &mEventDoubleQueue;
}

std::map<ape::Event::Type, int> ape::CSharpPlugin::getEventNumberMap()
{
    return mEventNumberMap;
}

//extern "C"
//JNIEXPORT void JNICALL
//Java_org_apertusvr_ApertusJNI_processEventDoubleQueue(JNIEnv *env, jclass clazz)
//{
//    ape::CSharpPlugin* CSharpPlugin = ape::CSharpPlugin::getPluginPtr();
//    jclass jEventManagerClass = env->FindClass("org/apertusvr/apeEventManager");
//
//    jmethodID jFireEventID = env->GetStaticMethodID(jEventManagerClass,"fireEvent","(IILjava/lang/String;)V");
//    ape::DoubleQueue<ape::Event>* eventDoubleQueue = CSharpPlugin->getEventDoubleQueue();
//    eventDoubleQueue->swap();
//
//    while(!eventDoubleQueue->emptyPop())
//    {
//        ape::Event frontEvent = eventDoubleQueue->front();
//        const char* sname = frontEvent.subjectName.c_str();
//        jstring jSubjectName =  env->NewStringUTF(sname);
//        jint typeAsJint = CSharpPlugin->getEventNumberMap().at(frontEvent.type);
//
//        env->CallStaticVoidMethod(
//                jEventManagerClass,
//                jFireEventID,
//                typeAsJint,
//                (jint)frontEvent.group,
//                jSubjectName
//                );
//        // env->ReleaseStringUTFChars(jSubjectName,sname); //?
//        eventDoubleQueue->pop();
//    }
//}