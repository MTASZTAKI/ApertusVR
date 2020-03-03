#include <thread>
#include <ctime>
#include "Bubble.h"

int WaterGame::Bubble::geometryCount = 0;

WaterGame::Bubble::Bubble(ape::Vector3 pos, int maxCount)
{
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mPosition = pos;
	mTimerCount = maxCount;
	mIsTimedOut = false;
	geometryCount++;
	id = geometryCount;
	APE_LOG_TRACE("Bubbl" << " id: " << id << " pos: " << pos.toString() << " maxCount: " << maxCount);
}

WaterGame::Bubble::~Bubble()
{
	APE_LOG_TRACE("");
	finish();
}

void WaterGame::Bubble::init()
{
	std::string indexStr = std::to_string(id);
	APE_LOG_TRACE("geometryCount: " << indexStr);
	mpSceneManager = ape::ISceneManager::getSingletonPtr();

	mBubbleNode = mpSceneManager->createNode("bubbleNode" + indexStr, true, mpCoreConfig->getNetworkGUID());
	if (auto bubbleNode = mBubbleNode.lock())
	{
		bubbleNode->setPosition(mPosition);
		bubbleNode->setScale(ape::Vector3(0.1, 0.1, 0.1));

		mGeometry = mpSceneManager->createEntity("bubbleGeometry" + indexStr, ape::Entity::GEOMETRY_SPHERE, true, mpCoreConfig->getNetworkGUID());
		if (auto bubbleGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mGeometry.lock()))
		{
			bubbleGeometry->setParameters(200.0f, ape::Vector2(1, 1));
			bubbleGeometry->setParentNode(bubbleNode);

			mMaterial = mpSceneManager->createEntity("bubbleMaterial" + indexStr, ape::Entity::MATERIAL_MANUAL, true, mpCoreConfig->getNetworkGUID());
			if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mMaterial.lock()))
			{
				float opacity = 0.12f;
				material->setDiffuseColor(ape::Color(0.3f, 0.3f, 0.3f, opacity));
				material->setSpecularColor(ape::Color(0.3f, 0.3f, 0.3f, opacity));
				material->setSceneBlending(ape::Material::SceneBlendingType::TRANSPARENT_ALPHA);
				bubbleGeometry->setMaterial(material);
			}
		}

		mCounterText = mpSceneManager->createEntity("bubbleText" + indexStr, ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID());
		if (auto counterText = std::static_pointer_cast<ape::ITextGeometry>(mCounterText.lock()))
		{
			counterText->setCaption(std::to_string(mTimerCount));
			counterText->setParentNode(bubbleNode);
		}
	}
}

void WaterGame::Bubble::start(int counter)
{
	mValue = counter;
	setCounter(counter);
	init();
}

void WaterGame::Bubble::finish()
{
	setText("-");
	mIsTimedOut = true;
	mTimerCount = 0;
	hide();
}

void WaterGame::Bubble::hide()
{
	if (auto bubbleNode = mBubbleNode.lock())
	{
		bubbleNode->setChildrenVisibility(false);
	}
}

std::string WaterGame::Bubble::getName()
{
	std::string indexStr = std::to_string(id);
	return "Bubble[" + indexStr + "]";
}

int WaterGame::Bubble::getId()
{
	return id;
}

ape::Vector3 WaterGame::Bubble::getPosition()
{
	return mPosition;
}

int WaterGame::Bubble::getValue()
{
	return mValue;
}

int WaterGame::Bubble::getCounter()
{
	return mTimerCount;
}

void WaterGame::Bubble::setCounter(int num)
{
	mTimerCount = num;
	setText(std::to_string(mTimerCount));
}

void WaterGame::Bubble::decCounter()
{
	setCounter(mTimerCount - 1);
	if (mTimerCount == 3)
		setColor(ape::Color(1.0f, 0.0f, 0.0f, 0.10f));
	else if (mTimerCount == 2)
		setColor(ape::Color(1.0f, 0.0f, 0.0f, 0.30f));
	else if (mTimerCount == 1)
		setColor(ape::Color(1.0f, 0.0f, 0.0f, 0.60f));
}

void WaterGame::Bubble::setColor(ape::Color color)
{
	if (auto bubbleGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mGeometry.lock()))
	{
		if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mMaterial.lock()))
		{
			material->setDiffuseColor(color);
			material->setSpecularColor(color);
			material->setSceneBlending(ape::Material::SceneBlendingType::TRANSPARENT_ALPHA);
			bubbleGeometry->setMaterial(material);
		}
	}
}

void WaterGame::Bubble::setText(std::string caption)
{
	if (auto counterText = std::static_pointer_cast<ape::ITextGeometry>(mCounterText.lock()))
	{
		counterText->setCaption(caption);
	}
}

bool WaterGame::Bubble::isTimedOut()
{
	return mIsTimedOut;
}
