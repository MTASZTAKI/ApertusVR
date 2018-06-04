#include <iostream>
#include <math.h> 
#include "ApeTexasEEGPlugin.h"
#include <Windows.h>

Ape::ApeTexasEEGPlugin::ApeTexasEEGPlugin()
{
	LOG_FUNC_ENTER();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeTexasEEGPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mUserNode = Ape::NodeWeakPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mScore = 0;
	while (ShowCursor(0) >= 0);
	LOG_FUNC_LEAVE();
}

Ape::ApeTexasEEGPlugin::~ApeTexasEEGPlugin()
{
	LOG_FUNC_ENTER();
	if (mBubbleManager)
	{
		delete mBubbleManager;
		mBubbleManager = NULL;
	}
	ShowCursor(true);
	LOG_FUNC_LEAVE();
}

void Ape::ApeTexasEEGPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeTexasEEGPlugin::Init()
{
	LOG_FUNC_ENTER();

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

	mGameManager = new TexasEEG::GameManager(mUserNode);
	mGameManager->Start();

	LOG_FUNC_LEAVE();
}

void Ape::ApeTexasEEGPlugin::Run()
{
	LOG_FUNC_ENTER();
	bool isTimedOut = false;
	bool removedBubbles = false;

	while (true)
	{
		if (auto userNode = mUserNode.lock())
		{
			Ape::Vector3 pos = userNode->getPosition();
			if (pos.y < -100)
			{
				pos.y = -100;
				userNode->setPosition(pos);
			}
		}
			
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeTexasEEGPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeTexasEEGPlugin::Step()
{

}

void Ape::ApeTexasEEGPlugin::Stop()
{

}

void Ape::ApeTexasEEGPlugin::Suspend()
{

}

void Ape::ApeTexasEEGPlugin::Restart()
{

}
