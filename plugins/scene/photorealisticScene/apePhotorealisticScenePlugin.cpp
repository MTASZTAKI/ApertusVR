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
	if (auto universeSkyBoxMaterial = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->createEntity("universe", ape::Entity::MATERIAL_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe");
		universeSkyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light", ape::Entity::LIGHT, true, mpCoreConfig->getNetworkGUID()).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1, 0, 0));
		light->setDiffuseColor(ape::Color(0.8f, 0.8f, 0.8f));
		light->setSpecularColor(ape::Color(0.8f, 0.8f, 0.8f));
	}
	mPlanetNode = mpSceneManager->createNode("planetNode", true, mpCoreConfig->getNetworkGUID());
	if (auto planetNode = mPlanetNode.lock())
	{
		//planetNode->setScale(ape::Vector3(10, 10, 10));
		planetNode->setPosition(ape::Vector3(0, 150, 0));
		if (auto planetMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("planet.mesh", ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			planetMeshFile->setFileName("planet.mesh");
			planetMeshFile->setParentNode(planetNode);
		}
		if (auto textNode = mpSceneManager->createNode("helloWorldText_Node", true, mpCoreConfig->getNetworkGUID()).lock())
		{
			textNode->setParentNode(planetNode);
			textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
			if (auto userNameText = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("helloWorldText", ape::Entity::GEOMETRY_TEXT, true, mpCoreConfig->getNetworkGUID()).lock()))
			{
				userNameText->setCaption("helloWorld");
				userNameText->setParentNode(textNode);
			}
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
		if (auto planetNode = mPlanetNode.lock())
			planetNode->rotate(0.0017f, ape::Vector3(0, 1, 0), ape::Node::TransformationSpace::LOCAL);
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
