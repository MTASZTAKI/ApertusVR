#include <iostream>
#include "ApeHtcVivePlugin.h"

Ape::ApeHtcVivePlugin::ApeHtcVivePlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpHMD = NULL;
	mCameraLeft = Ape::CameraWeakPtr();
	mCameraRight = Ape::CameraWeakPtr();
	mHeadNode = Ape::NodeWeakPtr();
	mUserMaterial = Ape::ManualMaterialWeakPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeHtcVivePlugin::~ApeHtcVivePlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

Ape::CameraWeakPtr Ape::ApeHtcVivePlugin::createCamera(std::string name)
{
	if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->createEntity(name, Ape::Entity::Type::CAMERA).lock()))
	{
		if (auto cameraNode = mpScene->createNode(name + "_Node").lock())
		{
			cameraNode->setParentNode(mHeadNode);
			if (auto cameraConeNode = mpScene->createNode(name + "_ConeNode").lock())
			{
				cameraConeNode->setParentNode(cameraNode);
				cameraConeNode->rotate(Ape::Degree(90.0f).toRadian(), Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::WORLD);
				if (auto cameraCone = std::static_pointer_cast<Ape::IConeGeometry>(mpScene->createEntity(name + "_ConeGeometry", Ape::Entity::GEOMETRY_CONE).lock()))
				{
					cameraCone->setParameters(10.0f, 30.0f, 1.0f, Ape::Vector2(1, 1));
					cameraCone->setParentNode(cameraConeNode);
					cameraCone->setMaterial(mUserMaterial);
				}
			}
			if (auto userNameTextNode = mpScene->createNode(name + "_TextNode").lock())
			{
				userNameTextNode->setParentNode(cameraNode);
				userNameTextNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
				if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity(name + "_TextGeometry", Ape::Entity::GEOMETRY_TEXT).lock()))
				{
					userNameText->setCaption(name);
					userNameText->setParentNode(userNameTextNode);
				}
			}
			camera->setParentNode(cameraNode);
		}
		return camera;
	}
}


void Ape::ApeHtcVivePlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeHtcVivePlugin::Init()
{
	LOG_FUNC_ENTER();

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		userNode->setFixedYaw(true);
		mUserNode = userNode;
		if (auto headNode = mpScene->getNode(userNode->getName() + "_HeadNode").lock())
		{
			mHeadNode = headNode;
		}
		if (auto userMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->getEntity(userNode->getName() + "_Material").lock()))
		{
			mUserMaterial = userMaterial;
		}
	}

	LOG(LOG_TYPE_DEBUG, "waiting for main window");
	while (Ape::IMainWindow::getSingletonPtr()->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "main window was found");
}

void Ape::ApeHtcVivePlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtcVivePlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeHtcVivePlugin::Step()
{

}

void Ape::ApeHtcVivePlugin::Stop()
{

}

void Ape::ApeHtcVivePlugin::Suspend()
{

}

void Ape::ApeHtcVivePlugin::Restart()
{

}
