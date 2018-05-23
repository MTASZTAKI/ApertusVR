#include <iostream>
#include "ApeRobotMonitoringPlugin.h"

Ape::ApeRobotMonitoringPlugin::ApeRobotMonitoringPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeRobotMonitoringPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeRobotMonitoringPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeRobotMonitoringPlugin::~ApeRobotMonitoringPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeRobotMonitoringPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeRobotMonitoringPlugin::Init()
{
	LOG_FUNC_ENTER();
	/*if (auto node = mpScene->createNode("standNode").lock())
	{
		node->setPosition(Ape::Vector3(0, 0, 0));
		if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("stand.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			meshFile->setFileName("stand.mesh");
			meshFile->setParentNode(node);
		}
	}*/
	if (auto environmentNode = mpScene->createNode("environmentNode").lock())
	{
		environmentNode->setScale(Ape::Vector3(100, 100, 100));
		if (auto environmentMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			environmentMeshFile->setFileName("sphere.mesh");
			environmentMeshFile->setParentNode(environmentNode);
		}
	}
	if (auto skyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("skyBox", Ape::Entity::MATERIAL_FILE).lock()))
	{
		skyBoxMaterial->setFileName("skyBox.material");
		skyBoxMaterial->setAsSkyBox();
	}
	Ape::Color lightColor(0.4f, 0.4f, 0.44f);
	/*if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(-1, -1, 0));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, -1));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light3", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light4", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, 1));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}*/
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, 1, -1));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, 1, 1));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light3", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(1, -1, 0));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}

	LOG_FUNC_LEAVE();
}




void Ape::ApeRobotMonitoringPlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeRobotMonitoringPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeRobotMonitoringPlugin::eventCallBack, this, std::placeholders::_1));
}

void Ape::ApeRobotMonitoringPlugin::Step()
{

}

void Ape::ApeRobotMonitoringPlugin::Stop()
{

}

void Ape::ApeRobotMonitoringPlugin::Suspend()
{

}

void Ape::ApeRobotMonitoringPlugin::Restart()
{

}

