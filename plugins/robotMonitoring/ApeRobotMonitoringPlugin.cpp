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
	Ape::Color lightColor(0.4f, 0.4f, 0.44f);
	Ape::ManualMaterialSharedPtr glassMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("glassMaterial", Ape::Entity::MATERIAL_MANUAL).lock());
	if (glassMaterial)
	{
		float opacity = 0.6f;
		glassMaterial->setDiffuseColor(Ape::Color(0.2, 0.2, 0.2, opacity));
		glassMaterial->setAmbientColor(Ape::Color(0.2, 0.2, 0.2));
		glassMaterial->setSpecularColor(Ape::Color(0.2, 0.2, 0.2, opacity));
		glassMaterial->setEmissiveColor(Ape::Color(0.2, 0.2, 0.2));
		glassMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
		glassMaterial->setCullingMode(Ape::Material::CullingMode::NONE_CM);
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(-1, 0, 0));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}
	/*if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, -1, -1));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}
	if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light3", Ape::Entity::LIGHT).lock()))
	{
		light->setLightType(Ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(Ape::Vector3(0, 1, 1));
		light->setDiffuseColor(lightColor);
		light->setSpecularColor(lightColor);
	}*/
	if (auto lightNode = mpScene->createNode("lightNode2").lock())
	{
		lightNode->setPosition(Ape::Vector3(0, 1500, -500));
		if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light2", Ape::Entity::LIGHT).lock()))
		{
			light->setLightType(Ape::Light::Type::SPOT);
			light->setLightDirection(Ape::Vector3(0, -1, 0));
			light->setLightSpotRange(Ape::LightSpotRange(Ape::Degree(10), Ape::Degree(70), 2));
			light->setLightAttenuation(Ape::LightAttenuation(3000, 1, 0, 0));
			light->setParentNode(lightNode);
		}
	}
	if (auto lightNode = mpScene->createNode("lightNode3").lock())
	{
		lightNode->setPosition(Ape::Vector3(-1000, 1500, -500));
		if (auto light = std::static_pointer_cast<Ape::ILight>(mpScene->createEntity("light3", Ape::Entity::LIGHT).lock()))
		{
			light->setLightType(Ape::Light::Type::SPOT);
			light->setLightDirection(Ape::Vector3(0, -1, 0));
			light->setLightSpotRange(Ape::LightSpotRange(Ape::Degree(10), Ape::Degree(70), 2));
			light->setLightAttenuation(Ape::LightAttenuation(3000, 1, 0, 0));
			light->setParentNode(lightNode);
		}
	}

	if (auto environmentNode = mpScene->createNode("environmentNode").lock())
	{
		environmentNode->setPosition(Ape::Vector3(0, 0, -100));
		environmentNode->setScale(Ape::Vector3(-100, 100, 100));
		Ape::Quaternion rot1(0, 0, 1, 0);
		Ape::Degree angle(8);
		Ape::Vector3 axis(0, 0, 1);
		Ape::Quaternion rot2;
		rot2.FromAngleAxis(angle, axis);
		Ape::Quaternion rot3;
		rot3 = rot1 * rot2;
		environmentNode->setOrientation(rot3);
		if (auto environmentMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			environmentMeshFile->setFileName("sphere.mesh");
			environmentMeshFile->setParentNode(environmentNode);
		}
	}
	if (auto glassWallNode = mpScene->createNode("glassWallNode").lock())
	{
		glassWallNode->setPosition(Ape::Vector3(0, 0, 195));
		Ape::Degree angle(90);
		Ape::Vector3 axis(1, 0, 0);
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		glassWallNode->setOrientation(orientation);
		if (auto glassWallGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("glassWallGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			glassWallGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(5000, 5000), Ape::Vector2(1, 1));
			glassWallGeometry->setMaterial(glassMaterial);
			glassWallGeometry->setParentNode(glassWallNode);
		}
	}
	if (auto environmentNode = mpScene->createNode("environmentNode_3").lock())
	{
		environmentNode->setPosition(Ape::Vector3(0, 0, 100));
		environmentNode->setScale(Ape::Vector3(-100, 100, 100));
		Ape::Quaternion rot1(0, 0, 1, 0);
		environmentNode->setOrientation(rot1);
		if (auto environmentMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere_3.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			environmentMeshFile->setFileName("sphere_3.mesh");
			environmentMeshFile->setParentNode(environmentNode);
		}
	}
	if (auto glassWallNode = mpScene->createNode("glassWallNode3").lock())
	{
		glassWallNode->setPosition(Ape::Vector3(0, 0, -1320));
		Ape::Radian angle(1.57f);
		Ape::Vector3 axis(1, 0, 0);
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		glassWallNode->setOrientation(orientation);
		if (auto glassWallGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("glassWallGeometry3", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			glassWallGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(5000, 5000), Ape::Vector2(1, 1));
			glassWallGeometry->setMaterial(glassMaterial);
			glassWallGeometry->setParentNode(glassWallNode);
		}
	}
	if (auto entranceWallNode = mpScene->createNode("entranceWallNode").lock())
	{
		entranceWallNode->setPosition(Ape::Vector3(410, 0, -500));
		Ape::Quaternion orientation(0.707, 0.707, 0, 0);
		Ape::Quaternion orientation2(0.707, 0, 0, 0.707);
		Ape::Quaternion orientation3;
		orientation3 = orientation * orientation2;
		entranceWallNode->setOrientation(orientation3);
		if (auto entranceWallGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("entranceWallGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			entranceWallGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(1000, 1000), Ape::Vector2(1, 1));
			entranceWallGeometry->setParentNode(entranceWallNode);
		}
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

