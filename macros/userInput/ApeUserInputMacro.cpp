#include <iostream>
#include "ApeUserInputMacro.h"

Ape::UserInputMacro::UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&UserInputMacro::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::ISceneManager::getSingletonPtr();
	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		mUserNode = userNode;
		mDummyNode = mpScene->createNode(userNode->getName() + "_DummyNode");
		if (auto headNode = mpScene->getNode(userNode->getName() + "_HeadNode").lock())
		{
			mHeadNode = headNode;
			headNode->setParentNode(Ape::NodeWeakPtr());
		}
		//userNode->setParentNode(mDummyNode);
	}
	APE_LOG_FUNC_LEAVE();
}

Ape::UserInputMacro::~UserInputMacro()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::UserInputMacro::translateUserNode(Ape::Vector3 axis, Ape::Node::TransformationSpace transformationSpace)
{
	if (auto userNode = mUserNode.lock())
	{
		if (auto dummyNode = mDummyNode.lock())
		{
			if (auto headNode = mHeadNode.lock())
			{
				if (!dummyNode->getOrientation().equals(headNode->getDerivedOrientation(), Ape::Radian(0.0f)))
				{
					dummyNode->setOrientation(headNode->getDerivedOrientation());
				}
			}
			dummyNode->translate(axis, transformationSpace);
			userNode->setPosition(dummyNode->getPosition());
		}
	}
}

void Ape::UserInputMacro::rotateUserNode(Ape::Degree angle, Ape::Vector3 axis, Ape::Node::TransformationSpace transformationSpace)
{
	if (auto userNode = mUserNode.lock())
	{
		userNode->rotate(angle.toRadian(), axis, Ape::Node::TransformationSpace::WORLD);
	}
}

void Ape::UserInputMacro::eventCallBack(const Ape::Event& event)
{
}