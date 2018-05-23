#include <thread>
#include <ctime>
#include "Bubble.h"

int TexasEEG::Bubble::geometryCount = 0;

TexasEEG::Bubble::Bubble(Ape::Vector3 pos, int maxCount)
{
	mPosition = pos;
	mTimerCount = maxCount;
	mIsTimedOut = false;
	geometryCount++;
	id = geometryCount;
	LOG(LOG_TYPE_DEBUG, "Bubbl" << " id: " << id << " pos: " << pos.toString() << " maxCount: " << maxCount);
}

TexasEEG::Bubble::~Bubble()
{
	LOG(LOG_TYPE_DEBUG, "");
	finish();
}

void TexasEEG::Bubble::init()
{
	std::string indexStr = std::to_string(id);
	LOG(LOG_TYPE_DEBUG, "geometryCount: " << indexStr);
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

			mMaterial = mpScene->createEntity("bubbleMaterial" + indexStr, Ape::Entity::MATERIAL_MANUAL);
			if (auto material = std::static_pointer_cast<Ape::IManualMaterial>(mMaterial.lock()))
			{
				float opacity = 0.12f;
				material->setDiffuseColor(Ape::Color(0.3f, 0.3f, 0.3f, opacity));
				material->setSpecularColor(Ape::Color(0.3f, 0.3f, 0.3f, opacity));
				material->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
				bubbleGeometry->setMaterial(material);
			}
		}

		mCounterText = mpScene->createEntity("bubbleText" + indexStr, Ape::Entity::GEOMETRY_TEXT);
		if (auto counterText = std::static_pointer_cast<Ape::ITextGeometry>(mCounterText.lock()))
		{
			counterText->setCaption(std::to_string(mTimerCount));
			counterText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
			counterText->setParentNode(bubbleNode);
		}
	}
}

void TexasEEG::Bubble::start(int counter)
{
	mValue = counter;
	setCounter(counter);
	init();
}

void TexasEEG::Bubble::finish()
{
	setText("-");
	mIsTimedOut = true;
	mTimerCount = 0;
	hide();
}

void TexasEEG::Bubble::hide()
{
	if (auto bubbleNode = mBubbleNode.lock())
	{
		bubbleNode->setChildrenVisibility(false);
	}
}

std::string TexasEEG::Bubble::getName()
{
	std::string indexStr = std::to_string(id);
	return "Bubble[" + indexStr + "]";
}

int TexasEEG::Bubble::getId()
{
	return id;
}

Ape::Vector3 TexasEEG::Bubble::getPosition()
{
	return mPosition;
}

int TexasEEG::Bubble::getValue()
{
	return mValue;
}

int TexasEEG::Bubble::getCounter()
{
	return mTimerCount;
}

void TexasEEG::Bubble::setCounter(int num)
{
	mTimerCount = num;
	setText(std::to_string(mTimerCount));
}

void TexasEEG::Bubble::decCounter()
{
	setCounter(mTimerCount - 1);
	if (mTimerCount == 3)
		setColor(Ape::Color(1.0f, 0.0f, 0.0f, 0.10f));
	else if (mTimerCount == 2)
		setColor(Ape::Color(1.0f, 0.0f, 0.0f, 0.30f));
	else if (mTimerCount == 1)
		setColor(Ape::Color(1.0f, 0.0f, 0.0f, 0.60f));
}

void TexasEEG::Bubble::setColor(Ape::Color color)
{
	if (auto bubbleGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mGeometry.lock()))
	{
		if (auto material = std::static_pointer_cast<Ape::IManualMaterial>(mMaterial.lock()))
		{
			material->setDiffuseColor(color);
			material->setSpecularColor(color);
			material->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
			bubbleGeometry->setMaterial(material);
		}
	}
}

void TexasEEG::Bubble::setText(std::string caption)
{
	if (auto counterText = std::static_pointer_cast<Ape::ITextGeometry>(mCounterText.lock()))
	{
		counterText->setCaption(caption);
	}
}

bool TexasEEG::Bubble::isTimedOut()
{
	return mIsTimedOut;
}
