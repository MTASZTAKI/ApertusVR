#include "apeAvatarPlugin.h"

ape::apeAvatarPlugin::apeAvatarPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeAvatarPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeAvatarPlugin::~apeAvatarPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeAvatarPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAvatarPlugin::eventCallBack(const ape::Event& event)
{
}

void ape::apeAvatarPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::shared_ptr<ape::IManualMaterial> handMaterial;
	if (handMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity("handMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
	{
		if (auto handMaterialManualPass = std::static_pointer_cast<ape::IManualPass>(mpSceneManager->createEntity("handMaterialManualPass", ape::Entity::PASS_MANUAL).lock()))
		{
			handMaterialManualPass->setShininess(15.0f);
			handMaterialManualPass->setDiffuseColor(ape::Color(0.0f, 1.0f, 0.0f));
			handMaterialManualPass->setSpecularColor(ape::Color(0.0f, 1.0f, 0.0f));
			handMaterial->setPass(handMaterialManualPass);
		}
	}
	if (auto leftHandGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("leftHandGeometry", ape::Entity::GEOMETRY_SPHERE).lock()))
	{
		leftHandGeometry->setParameters(1.0f, ape::Vector2(1, 1));
		leftHandGeometry->setParentNode(mLeftHandNode);
		leftHandGeometry->setMaterial(handMaterial);
	}
	if (auto rightHandGeometry = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->createEntity("rightHandGeometry", ape::Entity::GEOMETRY_SPHERE).lock()))
	{
		rightHandGeometry->setParameters(1.0f, ape::Vector2(1, 1));
		rightHandGeometry->setParentNode(mRightHandNode);
		rightHandGeometry->setMaterial(handMaterial);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAvatarPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	/*while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}*/
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAvatarPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAvatarPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAvatarPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAvatarPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
