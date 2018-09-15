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

Ape::Event Ape::ApeSceneRecorderPlugin::readEvent()
{
	Ape::Event event;
	long subjectNameSize;
	char* subjectName;
	mFileStreamIn.read(reinterpret_cast<char*>(&subjectNameSize), sizeof(long));
	mFileStreamIn.read(subjectName, subjectNameSize);
	event.subjectName = subjectName;
	mFileStreamIn.read(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamIn.read(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	return event;
}

void Ape::ApeSceneRecorderPlugin::writeEvent(Ape::Event event)
{
	//TODO maybe write timestamp for timing?
	long subjectNameSize = event.subjectName.size();
	mFileStreamOut.write(reinterpret_cast<char*>(&subjectNameSize), sizeof(long));
	mFileStreamOut.write(event.subjectName.c_str(), subjectNameSize);
	mFileStreamOut.write(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamOut.write(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	if (event.dataCallback != nullptr)
	{
		auto data = event.dataCallback();
		data.write(mFileStreamOut);
	}
}

void Ape::ApeSceneRecorderPlugin::eventCallBack(const Ape::Event& event)
{
	if (mIsRecorder)
	{
		writeEvent(event);
	}
}

void Ape::ApeSceneRecorderPlugin::Init()
{
	LOG_FUNC_ENTER();
	mIsRecorder = true;
	mIsPlayer = false;
	mIsLooping = false;
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
		if (mIsRecorder)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else if (mIsPlayer)
		{
			auto event = readEvent();
			if (!mFileStreamIn.good() && mIsLooping) 
			{
				mFileStreamIn.close();
				mFileStreamIn.clear();
				mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
			}
			//TODO maybe timig by reading timestamps from the file?
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
