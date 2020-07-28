#include "apePhotoRealisticScenePlugin.h"

ape::apePhotoRealisticScenePlugin::apePhotoRealisticScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apePhotoRealisticScenePlugin::~apePhotoRealisticScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::eventCallBack(const ape::Event& event)
{

}

void ape::apePhotoRealisticScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1, 0, 0));
		light->setDiffuseColor(ape::Color(0.8f, 0.8f, 0.8f));
		light->setSpecularColor(ape::Color(0.8f, 0.8f, 0.8f));
	}
	mNode = mpSceneManager->createNode("gltfMovementNode", true, mpCoreConfig->getNetworkGUID());
	if (auto node = mNode.lock())
	{
		node->setPosition(ape::Vector3(0, 150, 0));
		if (auto gltfMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("damagedHelmet.gltf", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			gltfMeshFile->setFileName("damagedHelmet.gltf");
			gltfMeshFile->setParentNode(node);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		if (auto node = mNode.lock())
			node->rotate(0.0017f, ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::LOCAL);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
