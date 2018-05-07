#include <iostream>
#include <math.h> 
#include "ApeTexasEEGPlugin.h"

Ape::ApeTexasEEGPlugin::ApeTexasEEGPlugin()
{
	LOG_FUNC_ENTER();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeTexasEEGPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mUserNode = Ape::NodeWeakPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mScore = 0;
	LOG_FUNC_LEAVE();
}

Ape::ApeTexasEEGPlugin::~ApeTexasEEGPlugin()
{
	LOG_FUNC_ENTER();
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

	/*if (auto bubblesNode = mpScene->createNode("bubblesNode").lock())
	{
		bubblesNode->setPosition(Ape::Vector3(0, 0, 0));
		bubblesNode->setScale(Ape::Vector3(0.1, 0.1, 0.1));
		if (auto bubblesMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("bubbles.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			bubblesMeshFile->setFileName("bubbles.mesh");
			bubblesMeshFile->setParentNode(bubblesNode);
		}
	}*/

	mScoreText = mpScene->createEntity("scoreText", Ape::Entity::GEOMETRY_TEXT);
	if (auto scoreText = std::static_pointer_cast<Ape::ITextGeometry>(mScoreText.lock()))
	{
		scoreText->setCaption(std::to_string(mScore));
		scoreText->setOffset(Ape::Vector3(20.0f, 10.0f, -30.0f));
		scoreText->setParentNode(mUserNode);
	}

	int newXDist = 0;
	int newYDist = 0;
	int newZDist = 0;
	for (int i = 0; i < 50; i++)
	{
		newXDist = (std::rand() % 100) - 10;
		newYDist = (std::rand() % 50) - 50;
		newZDist -= (std::rand() % 300) + 150;
		mBubbleQueue.push(new TexasEEG::Bubble(Ape::Vector3(newXDist, newYDist, newZDist)));
	}

	mBubbleQueue.front()->startThread();

	LOG_FUNC_LEAVE();
}

void Ape::ApeTexasEEGPlugin::Run()
{
	LOG_FUNC_ENTER();
	bool isTimedOut = false;
	while (true && !isTimedOut)
	{
		if (auto userNode = mUserNode.lock())
		{
			if (!mBubbleQueue.empty())
			{
				isTimedOut = mBubbleQueue.front()->isTimedOut();
				if (!isTimedOut && userNode->getPosition().distance(mBubbleQueue.front()->getPosition()) < 30)
				{
					mScore++;
					if (auto scoreText = std::static_pointer_cast<Ape::ITextGeometry>(mScoreText.lock()))
					{
						scoreText->setCaption(std::to_string(mScore));
					}

					if (mBubbleQueue.front())
					{
						delete mBubbleQueue.front();
						mBubbleQueue.pop();
					}
					if (!mBubbleQueue.empty() && !isTimedOut)
					{
						mBubbleQueue.front()->startThread();
					}
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	if (isTimedOut)
	{
		LOG(LOG_TYPE_DEBUG, "Game Over!");

		mStatusText = mpScene->createEntity("statusText", Ape::Entity::GEOMETRY_TEXT);
		if (auto statusText = std::static_pointer_cast<Ape::ITextGeometry>(mStatusText.lock()))
		{
			statusText->setCaption("Game Over");
			statusText->setOffset(Ape::Vector3(0.0f, 0.0f, -30.0f));
			statusText->setParentNode(mUserNode);
		}
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
