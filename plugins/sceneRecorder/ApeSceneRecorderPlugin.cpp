#include <iostream>
#include "apeSceneRecorderPlugin.h"

ape::apeSceneRecorderPlugin::apeSceneRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::PASS_PBS, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::PASS_MANUAL, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = ape::ISceneManager::getSingletonPtr();
	mLastEventTimeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	mIsRecorder = false;
	mIsPlayer = true;
	mIsLooping = false;
	mFileName = "scene.bin";
	if (mIsRecorder) 
		mFileStreamOut.open(mFileName, std::ios::out | std::ios::binary);
	else if (mIsPlayer)
	{
		mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
	}
	mDelayToNextEvent = 0;
	APE_LOG_FUNC_LEAVE();
}

ape::apeSceneRecorderPlugin::~apeSceneRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	if (mFileStreamOut.is_open())
	{
		mFileStreamOut.close();
		APE_LOG_DEBUG("fileStreamOut closed ");
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSceneRecorderPlugin::readEventAndFire()
{
	ape::Event event;
	mFileStreamIn.read(reinterpret_cast<char*>(&mDelayToNextEvent), sizeof(long));
	mFileStreamIn.read(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamIn.read(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	unsigned int subjectNameSize = 0;
	mFileStreamIn.read(reinterpret_cast<char *>(&subjectNameSize), sizeof(subjectNameSize));
	event.subjectName.resize(subjectNameSize);
	mFileStreamIn.read(&event.subjectName[0], subjectNameSize);
	APE_LOG_DEBUG("mDelayToNextEvent: " << mDelayToNextEvent << " event.subjectName: " << event.subjectName << " event.type:" << event.type);
	if (mDelayToNextEvent)
		std::this_thread::sleep_for(std::chrono::milliseconds(mDelayToNextEvent));
	if (event.group == ape::Event::Group::NODE)
	{
		if (event.type == ape::Event::Type::NODE_CREATE)
		{
			mpScene->createNode(event.subjectName);
		}
		/*else if (auto node = mpScene->getNode(event.subjectName).lock())
		{*/
			if (event.type == ape::Event::Type::NODE_POSITION)
			{
				ape::Vector3 position;
				position.read(mFileStreamIn);
				//node->setPosition(position);
			}
		//}
	}
}

void ape::apeSceneRecorderPlugin::writeEventHeader(ape::Event& event)
{
	auto timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	long timeToCallEventInMilliseconds = timeStamp.count() - mLastEventTimeStamp.count();
	mLastEventTimeStamp = timeStamp;
	mFileStreamOut.write(reinterpret_cast<char*>(&timeToCallEventInMilliseconds), sizeof(long));
	mFileStreamOut.write(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamOut.write(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	unsigned int subjectNameSize = event.subjectName.size();
	mFileStreamOut.write(reinterpret_cast<char *>(&subjectNameSize), sizeof(subjectNameSize));
	mFileStreamOut.write(event.subjectName.c_str(), subjectNameSize);
}

void ape::apeSceneRecorderPlugin::writeEvent(ape::Event event)
{
	if (event.group == ape::Event::Group::NODE)
	{
		if (event.type == ape::Event::Type::NODE_CREATE)
		{
			writeEventHeader(event);
		}
		else if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			if (event.type == ape::Event::Type::NODE_POSITION)
			{
				writeEventHeader(event);
				ape::Vector3 position = node->getPosition();
				position.write(mFileStreamOut);
			}
		}
	}
}

void ape::apeSceneRecorderPlugin::eventCallBack(const ape::Event& event)
{
	if (mIsRecorder)
	{
		writeEvent(event);
	}
}

void ape::apeSceneRecorderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSceneRecorderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		if (mIsPlayer)
		{
			readEventAndFire();
			if (!mFileStreamIn.good() && mIsLooping)
			{
				mFileStreamIn.close();
				mFileStreamIn.clear();
				mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
				APE_LOG_DEBUG("looping the file");
			}
			else if (mFileStreamIn.eof())
			{
				mIsPlayer = false;
				APE_LOG_DEBUG("end of the file");
			}
		}
	}
	mpEventManager->disconnectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSceneRecorderPlugin::Step()
{

}

void ape::apeSceneRecorderPlugin::Stop()
{

}

void ape::apeSceneRecorderPlugin::Suspend()
{

}

void ape::apeSceneRecorderPlugin::Restart()
{

}

