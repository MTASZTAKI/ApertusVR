#include <iostream>
#include "ApeSceneRecorderPlugin.h"

Ape::ApeSceneRecorderPlugin::ApeSceneRecorderPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
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

void Ape::ApeSceneRecorderPlugin::readFrame()
{
	/*mFileStreamIn.read(reinterpret_cast<char*>(&mCurrentPoints[0]), mPointCloudSize * sizeof(float));
	mFileStreamIn.read(reinterpret_cast<char*>(&mCurrentColors[0]), mPointCloudSize * sizeof(float));*/
}

void Ape::ApeSceneRecorderPlugin::writeFrame()
{
	//TODO maybe write timestamp for timing?
	//mFileStreamOut.write(reinterpret_cast<char*>(&mCurrentPoints[0]), mPointCloudSize * sizeof(float));
	//mFileStreamOut.write(reinterpret_cast<char*>(&mCurrentColors[0]), mPointCloudSize * sizeof(float));
}

void Ape::ApeSceneRecorderPlugin::eventCallBack(const Ape::Event& event)
{
	if (mIsRecorder)
	{
		
	}
}

void Ape::ApeSceneRecorderPlugin::Init()
{
	LOG_FUNC_ENTER();
	mIsRecorder = false;
	mIsPlayer = true;
	mIsLooping = true;
	mFileName = "pointCloud.bin";
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
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else if (mIsPlayer)
		{
			readFrame();
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
