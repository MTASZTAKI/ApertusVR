#include "apePhotorealisticScenePlugin.h"

ape::apePhotoRealisticScenePlugin::apePhotoRealisticScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mIsRenderReady = false;
	APE_LOG_FUNC_LEAVE();
}

ape::apePhotoRealisticScenePlugin::~apePhotoRealisticScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apePhotoRealisticScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::CAMERA_WINDOW)
	{
		mIsRenderReady = true;
	}
}

void ape::apePhotoRealisticScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePhotoRealisticScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	if (auto node = mpSceneManager->createNode("lightNode1", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light1", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			light->setParentNode(node);
			light->setLightType(ape::Light::Type::DIRECTIONAL);
			light->setLightDirection(ape::Vector3(-1, -1, -0.5f));
			light->setPowerScale(5);
		}
	}
	if (auto node = mpSceneManager->createNode("lightNode2", true, mpCoreConfig->getNetworkGUID()).lock())
	{
		if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light2", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			light->setParentNode(node);
			light->setLightType(ape::Light::Type::DIRECTIONAL);
			light->setLightDirection(ape::Vector3(1, 1, 0.5f));
			light->setPowerScale(5);
		}
	}
	mNode = mpSceneManager->createNode("gltfNode", true, mpCoreConfig->getNetworkGUID());
	if (auto node = mNode.lock())
	{
		//node->setScale(ape::Vector3(10, 10, 10));
		//node->setOrientation(ape::Quaternion(0, 0, 1, 0));
		//node->setPosition(ape::Vector3(0, 0, -50));
		if (auto gltfMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("VLTF_3Dlogo", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			gltfMeshFile->setFileName("/plugins/scene/photorealisticScene/resources/VLTF_3Dlogo.gltf");
			gltfMeshFile->setParentNode(node);
            node->rotate(1.5708f, ape::Vector3(1, 0, 0), ape::Node::TransformationSpace::LOCAL);
           
            node->setPosition(ape::Vector3(0.05,0.005, 0.86));
            node->rotate(3.5f, ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::LOCAL);
            node->setPosition(ape::Vector3(0,0.005, 0.86));
		}
		/*if (auto glbMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("CesiumMan", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			glbMeshFile->setFileName("CesiumMan.glb");
			glbMeshFile->setParentNode(node);
		}*/
	}
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
    //		if (auto node = mNode.lock())
    //			node->rotate(-0.0034f, ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::LOCAL);
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
