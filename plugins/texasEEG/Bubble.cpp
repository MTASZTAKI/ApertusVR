#include <thread>
#include "Bubble.h"

int TexasEEG::Bubble::geometryCount = 0;

TexasEEG::Bubble::Bubble(Ape::Vector3 pos, int maxCount)
{
	LOG(LOG_TYPE_DEBUG, "pos: " << pos.toString() << " maxCount: " << maxCount);
	mPosition = pos;
	mTimerCount = maxCount;
	mIsTimedOut = false;
}

TexasEEG::Bubble::~Bubble()
{
	LOG(LOG_TYPE_DEBUG, "");
	hide();
	if (mTimerThread)
	{
		delete mTimerThread;
		mTimerThread = NULL;
	}
}

void TexasEEG::Bubble::startThread()
{
	mTimerThread = new std::thread(std::bind(&Bubble::init, this));
	mTimerThread->detach();
}

void TexasEEG::Bubble::init()
{
	geometryCount++;
	std::string indexStr = std::to_string(geometryCount);
	LOG(LOG_TYPE_DEBUG, "geometryCount: " << indexStr);
	mTimerCount = 10;
	mpScene = Ape::IScene::getSingletonPtr();
	
	mBubbleNode = mpScene->createNode("bubbleNode" + indexStr);
	if (auto bubbleNode = mBubbleNode.lock())
	{
		bubbleNode->setPosition(mPosition);
		bubbleNode->setScale(Ape::Vector3(0.1, 0.1, 0.1));

		mGeometry = mpScene->createEntity("bubbleGeometry" + indexStr, Ape::Entity::GEOMETRY_SPHERE);
		if (auto bubbleGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mGeometry.lock()))
		{
			bubbleGeometry->setParameters(200.0f, Ape::Vector2(1, 1));
			bubbleGeometry->setParentNode(bubbleNode);
		}

		mCounterText = mpScene->createEntity("bubbleText" + indexStr, Ape::Entity::GEOMETRY_TEXT);
		if (auto counterText = std::static_pointer_cast<Ape::ITextGeometry>(mCounterText.lock()))
		{
			counterText->setCaption(std::to_string(mTimerCount));
			counterText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
			counterText->setParentNode(bubbleNode);
		}
	}

	startCounter();
}

void TexasEEG::Bubble::startCounter()
{
	LOG(LOG_TYPE_DEBUG, "Start[" << geometryCount << "]");
	while (mTimerCount > 1)
	{
		mTimerCount--;
		LOG(LOG_TYPE_DEBUG, "mTimerCount[" << geometryCount << "]: " << mTimerCount);
		setText(std::to_string(mTimerCount));
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	mIsTimedOut = true;
	hide();
}

void TexasEEG::Bubble::hide()
{
	if (auto bubbleNode = mBubbleNode.lock())
	{
		bubbleNode->setChildrenVisibility(false);
	}
}

Ape::Vector3 TexasEEG::Bubble::getPosition()
{
	return mPosition;
}

bool TexasEEG::Bubble::isTimedOut()
{
	return mIsTimedOut;
}

void TexasEEG::Bubble::setText(std::string caption)
{
	if (auto counterText = std::static_pointer_cast<Ape::ITextGeometry>(mCounterText.lock()))
	{
		counterText->setCaption(caption);
	}
}
