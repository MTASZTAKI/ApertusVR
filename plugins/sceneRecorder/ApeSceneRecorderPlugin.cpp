#include <iostream>
#include "ApeSceneRecorderPlugin.h"

Ape::ApeSceneRecorderPlugin::ApeSceneRecorderPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::LIGHT, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TEXT, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_PLANE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_BOX, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_CONE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TUBE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_SPHERE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TORUS, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::MATERIAL_FILE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::MATERIAL_MANUAL, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::PASS_PBS, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::PASS_MANUAL, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_MANUAL, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_FILE, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::SKY, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::WATER, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mIsRecorder = false;
	mIsPlayer = false;
	mIsLooping = false;
	mFileName = "";
	mLastEventTimeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());;
	LOG_FUNC_LEAVE();
}

Ape::ApeSceneRecorderPlugin::~ApeSceneRecorderPlugin()
{
	LOG_FUNC_ENTER();
	if (mFileStreamOut.is_open())
	{
		mFileStreamOut.close();
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeSceneRecorderPlugin::fireEvent(unsigned int milliseconds, Ape::Event event)
{
	if (milliseconds)
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	if (event.group == Ape::Event::Group::NODE)
	{
		if (event.type == Ape::Event::Type::NODE_CREATE)
		{
			mpScene->createNode(event.subjectName);
		}
		else if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			if (event.type == Ape::Event::Type::NODE_POSITION)
			{
				Ape::Vector3 position;
				//position.read(mFileStreamIn);
				//node->setPosition(position);
			}
			else if (event.type == Ape::Event::Type::NODE_ORIENTATION)
			{

			}
		}
	}
	else if (event.group == Ape::Event::Group::POINT_CLOUD)
	{

	}
}

void Ape::ApeSceneRecorderPlugin::readEvent()
{
	Ape::Event event;
	long timeToCallEventInMilliseconds;
	mFileStreamIn.read(reinterpret_cast<char*>(&timeToCallEventInMilliseconds), sizeof(long));
	long subjectNameSize;
	mFileStreamIn.read(reinterpret_cast<char*>(&subjectNameSize), sizeof(long));
	event.subjectName.resize(subjectNameSize);
	mFileStreamIn.read(&event.subjectName[0], subjectNameSize);
	mFileStreamIn.read(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamIn.read(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	LOG(LOG_TYPE_DEBUG, "subjectNameSize" << subjectNameSize << " timeToCallEventInMilliseconds: " << timeToCallEventInMilliseconds << " name:" << event.subjectName << " type:" << event.type);
	//auto eventCallbackThread = std::thread(&ApeSceneRecorderPlugin::fireEvent, this, timeToCallEventInMilliseconds, event);
	//eventCallbackThread.detach();
	fireEvent(timeToCallEventInMilliseconds, event);
}

void Ape::ApeSceneRecorderPlugin::writeEvent(Ape::Event event)
{
	auto timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	long timeToCallEventInMilliseconds = timeStamp.count() - mLastEventTimeStamp.count();
	mLastEventTimeStamp = timeStamp;
	mFileStreamOut.write(reinterpret_cast<char*>(&timeToCallEventInMilliseconds), sizeof(long));
	long subjectNameSize = event.subjectName.size();
	mFileStreamOut.write(reinterpret_cast<char*>(&subjectNameSize), sizeof(long));
	mFileStreamOut.write(event.subjectName.c_str(), subjectNameSize);
	mFileStreamOut.write(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamOut.write(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	LOG(LOG_TYPE_DEBUG, "subjectNameSize" << subjectNameSize << " timeToCallEventInMilliseconds: " << timeToCallEventInMilliseconds << " name:" << event.subjectName << " type:" << event.type);
	if (event.group == Ape::Event::Group::NODE)
	{
		if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			if (event.type == Ape::Event::Type::NODE_POSITION)
			{
				//Ape::Vector3 position = node->getPosition();
				//position.write(mFileStreamOut);
			}
			else if (event.type == Ape::Event::Type::NODE_ORIENTATION)
			{

			}
		}
	}
	else if (event.group == Ape::Event::Group::POINT_CLOUD)
	{

	}
}

void Ape::ApeSceneRecorderPlugin::eventCallBack(const Ape::Event& event)
{
	if (mIsRecorder)
	{
		//LOG(LOG_TYPE_DEBUG," name:" << event.subjectName << " type:" << event.type);
		writeEvent(event);
	}
}

void Ape::ApeSceneRecorderPlugin::Init()
{
	LOG_FUNC_ENTER();
	mIsRecorder = false;
	mIsPlayer = true;
	mIsLooping = true;
	mFileName = "scene.bin";
	if (mIsRecorder)
		mFileStreamOut.open(mFileName, std::ios::out | std::ios::binary);
	else if (mIsPlayer)
	{
		mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeSceneRecorderPlugin::Run()
{
	LOG_FUNC_ENTER();
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
			}
			else if (mFileStreamIn.eof())
			{
				mIsPlayer = false;
			}
		}
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&ApeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeSceneRecorderPlugin::Step()
{

}

void Ape::ApeSceneRecorderPlugin::Stop()
{

}

void Ape::ApeSceneRecorderPlugin::Suspend()
{

}

void Ape::ApeSceneRecorderPlugin::Restart()
{

}

