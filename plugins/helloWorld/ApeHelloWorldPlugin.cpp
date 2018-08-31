#include <iostream>
#include "ApeHelloWorldPlugin.h"

Ape::ApeHelloWorldPlugin::ApeHelloWorldPlugin()
{
	LOG_FUNC_ENTER();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeHelloWorldPlugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeHelloWorldPlugin::~ApeHelloWorldPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeHelloWorldPlugin::nodeEventCallBack(const Ape::Event& event)
{
}

void Ape::ApeHelloWorldPlugin::Init()
{
	LOG_FUNC_ENTER();
	if (auto universeSkyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("universe", Ape::Entity::MATERIAL_FILE).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe.material");
		universeSkyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, 0, 0));
		light->setDiffuseColor(Ape::Color(0.8f, 0.8f, 0.8f));
		light->setSpecularColor(Ape::Color(0.8f, 0.8f, 0.8f));
	}
	mPlanetNode = mpScene->createNode("planetNode");
	if (auto planetNode = mPlanetNode.lock())
	{
		planetNode->setPosition(Ape::Vector3(0, 0, -100));
		if (auto planetMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("planet.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			planetMeshFile->setFileName("planet.mesh");
			planetMeshFile->setParentNode(planetNode);
		}
		if (auto textNode = mpScene->createNode("helloWorldText_Node").lock())
		{
			textNode->setParentNode(planetNode);
			textNode->setPosition(Ape::Vector3(0.0f, 10.0f, 0.0f));
			if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("helloWorldText", Ape::Entity::GEOMETRY_TEXT).lock()))
			{
				userNameText->setCaption("helloWorld");
				userNameText->setParentNode(textNode);
			}
		}
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeHelloWorldPlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeHelloWorldPlugin::nodeEventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeHelloWorldPlugin::Step()
{
}

void Ape::ApeHelloWorldPlugin::Stop()
{
}

void Ape::ApeHelloWorldPlugin::Suspend()
{
}

void Ape::ApeHelloWorldPlugin::Restart()
{
}
