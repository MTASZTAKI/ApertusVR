#include "ApeAvatarPlugin.h"

Ape::ApeAvatarPlugin::ApeAvatarPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeAvatarPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeAvatarPlugin::~ApeAvatarPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeAvatarPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::eventCallBack(const Ape::Event& event)
{
}

void Ape::ApeAvatarPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
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
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	/*while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}*/
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAvatarPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
