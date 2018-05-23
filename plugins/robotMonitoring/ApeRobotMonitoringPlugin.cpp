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
	}
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
		glassWallNode->setPosition(Ape::Vector3(0, 0, 200));
		Ape::Radian angle(1.57f);
		Ape::Vector3 axis(1, 0, 0);
		Ape::Quaternion orientation;
		orientation.FromAngleAxis(angle, axis);
		Ape::Radian angle2(1.57f);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion orientation2;
		orientation2.FromAngleAxis(angle2, axis2);
		glassWallNode->setOrientation(orientation * orientation2);
		if (auto glassWallGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("glassWallGeometry", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			glassWallGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(500, 500), Ape::Vector2(1, 1));
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
		Ape::Radian angle2(1.57f);
		Ape::Vector3 axis2(0, 1, 0);
		Ape::Quaternion orientation2;
		orientation2.FromAngleAxis(angle2, axis2);
		glassWallNode->setOrientation(orientation * orientation2);
		if (auto glassWallGeometry = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("glassWallGeometry3", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			glassWallGeometry->setParameters(Ape::Vector2(1, 1), Ape::Vector2(500, 500), Ape::Vector2(1, 1));
			glassWallGeometry->setMaterial(glassMaterial);
			glassWallGeometry->setParentNode(glassWallNode);
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

