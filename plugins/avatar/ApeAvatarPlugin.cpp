#include <iostream>
#include "ApeAvatarPlugin.h"

Ape::ApeAvatarPlugin::ApeAvatarPlugin()
{
	LOG_FUNC_ENTER();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeAvatarPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeAvatarPlugin::~ApeAvatarPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == (mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName + "_rightHandNode"))
		mRightHandNode = mpSceneManager->getNode(event.subjectName);
	if (event.type == Ape::Event::Type::NODE_CREATE && event.subjectName == (mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName + "_leftHandNode"))
		mLeftHandNode = mpSceneManager->getNode(event.subjectName);
}

void Ape::ApeAvatarPlugin::Init()
{
	LOG_FUNC_ENTER();
	std::shared_ptr<Ape::IManualMaterial> handMaterial;
	if (handMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity("handMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto handMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpSceneManager->createEntity("handMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
		{
			handMaterialManualPass->setShininess(15.0f);
			handMaterialManualPass->setDiffuseColor(Ape::Color(0.0f, 1.0f, 0.0f));
			handMaterialManualPass->setSpecularColor(Ape::Color(0.0f, 1.0f, 0.0f));
			handMaterial->setPass(handMaterialManualPass);
		}
	}
	if (auto leftHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("leftHandGeometry", Ape::Entity::GEOMETRY_SPHERE).lock()))
	{
		leftHandGeometry->setParameters(1.0f, Ape::Vector2(1, 1));
		leftHandGeometry->setParentNode(mLeftHandNode);
		leftHandGeometry->setMaterial(handMaterial);
	}
	if (auto rightHandGeometry = std::static_pointer_cast<Ape::ISphereGeometry>(mpSceneManager->createEntity("rightHandGeometry", Ape::Entity::GEOMETRY_SPHERE).lock()))
	{
		rightHandGeometry->setParameters(1.0f, Ape::Vector2(1, 1));
		rightHandGeometry->setParentNode(mRightHandNode);
		rightHandGeometry->setMaterial(handMaterial);
	}
	LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::Run()
{
	LOG_FUNC_ENTER();
	/*while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}*/
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeAvatarPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::Step()
{

}

void Ape::ApeAvatarPlugin::Stop()
{

}

void Ape::ApeAvatarPlugin::Suspend()
{

}

void Ape::ApeAvatarPlugin::Restart()
{

}
