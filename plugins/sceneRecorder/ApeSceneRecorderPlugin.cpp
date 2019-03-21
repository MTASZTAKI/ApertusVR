#include <iostream>
#include "ApeSceneRecorderPlugin.h"

ape::ApeSceneRecorderPlugin::ApeSceneRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::PASS_PBS, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::PASS_MANUAL, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
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
	APE_LOG_FUNC_LEAVE();
}

ape::ApeSceneRecorderPlugin::~ApeSceneRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	if (mFileStreamOut.is_open())
	{
		mFileStreamOut.close();
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeSceneRecorderPlugin::fireEvent(unsigned int milliseconds, ape::Event event)
{
	if (milliseconds)
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	if (event.group == ape::Event::Group::NODE)
	{
		if (event.type == ape::Event::Type::NODE_CREATE)
		{
			mpScene->createNode(event.subjectName);
		}
		else if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			if (event.type == ape::Event::Type::NODE_POSITION)
			{
				ape::Vector3 position;
				position.read(mFileStreamIn);
				node->setPosition(position);
			}
			else if (event.type == ape::Event::Type::NODE_ORIENTATION)
			{

			}
		}
	}
	else if (event.group == ape::Event::Group::POINT_CLOUD)
	{

	}
}

void ape::ApeSceneRecorderPlugin::readEvent()
{
	ape::Event event;
	long timeToCallEventInMilliseconds;
	mFileStreamIn.read(reinterpret_cast<char*>(&timeToCallEventInMilliseconds), sizeof(long));
	unsigned int subjectNameSize = 0;
	mFileStreamIn.read(reinterpret_cast<char *>(&subjectNameSize), sizeof(subjectNameSize));
	event.subjectName.resize(subjectNameSize);
	mFileStreamIn.read(&event.subjectName[0], subjectNameSize);
	mFileStreamIn.read(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamIn.read(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	APE_LOG_DEBUG("subjectNameSize: " << subjectNameSize << " timeToCallEventInMilliseconds: " << timeToCallEventInMilliseconds << " name:" << event.subjectName << " type:" << event.type);
	//auto eventCallbackThread = std::thread(&ApeSceneRecorderPlugin::fireEvent, this, timeToCallEventInMilliseconds, event);
	//eventCallbackThread.detach();
	fireEvent(timeToCallEventInMilliseconds, event);
}

void ape::ApeSceneRecorderPlugin::writeEvent(ape::Event event)
{
	auto timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	long timeToCallEventInMilliseconds = timeStamp.count() - mLastEventTimeStamp.count();
	mLastEventTimeStamp = timeStamp;
	mFileStreamOut.write(reinterpret_cast<char*>(&timeToCallEventInMilliseconds), sizeof(long));
	unsigned int subjectNameSize = event.subjectName.size();                                 
	mFileStreamOut.write(reinterpret_cast<char *>(&subjectNameSize), sizeof(subjectNameSize));  
	mFileStreamOut.write(event.subjectName.c_str(), subjectNameSize);
	mFileStreamOut.write(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamOut.write(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	//APE_LOG_DEBUG("subjectNameSize" << subjectNameSize << " timeToCallEventInMilliseconds: " << timeToCallEventInMilliseconds << " name:" << event.subjectName << " type:" << event.type);
	if (event.group == ape::Event::Group::NODE)
	{
		if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			if (event.type == ape::Event::Type::NODE_POSITION)
			{
				ape::Vector3 position = node->getPosition();
				position.write(mFileStreamOut);
			}
			else if (event.type == ape::Event::Type::NODE_ORIENTATION)
			{

			}
		}
	}
	else if (event.group == ape::Event::Group::POINT_CLOUD)
	{

	}
}

void ape::ApeSceneRecorderPlugin::eventCallBack(const ape::Event& event)
{
	if (mIsRecorder)
	{
		//LOG(LOG_TYPE_DEBUG," name:" << event.subjectName << " type:" << event.type);
		//writeEvent(event);
		auto writeEventThread = std::thread(&ApeSceneRecorderPlugin::writeEvent, this, event);
		writeEventThread.detach();
	}
}

void ape::ApeSceneRecorderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeSceneRecorderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		if (mIsPlayer)
		{
			readEvent();
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
	mpEventManager->disconnectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeSceneRecorderPlugin::Step()
{

}

void ape::ApeSceneRecorderPlugin::Stop()
{

}

void ape::ApeSceneRecorderPlugin::Suspend()
{

}

void ape::ApeSceneRecorderPlugin::Restart()
{

}

