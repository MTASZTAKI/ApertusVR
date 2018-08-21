#include <iostream>
#include "ApeGyorPlugin.h"

Ape::ApeGyorPlugin::ApeGyorPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeGyorPlugin::~ApeGyorPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeGyorPlugin::Init()
{
	LOG_FUNC_ENTER();

	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(Ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.6f));
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, 1));
		light->setDiffuseColor(Ape::Color(0.6f, 0.6f, 0.6f));
		light->setSpecularColor(Ape::Color(0.6f, 0.6f, 0.6f));
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Run()
{
	LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeGyorPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeGyorPlugin::Step()
{

}

void Ape::ApeGyorPlugin::Stop()
{

}

void Ape::ApeGyorPlugin::Suspend()
{

}

void Ape::ApeGyorPlugin::Restart()
{

}
