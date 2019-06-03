#include "apeHelloWorldPlugin.h"

ape::apeHelloWorldPlugin::apeHelloWorldPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeHelloWorldPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeHelloWorldPlugin::~apeHelloWorldPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeHelloWorldPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHelloWorldPlugin::eventCallBack(const ape::Event& event)
{
}

void ape::apeHelloWorldPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto universeSkyBoxMaterial = std::static_pointer_cast<ape::IFileMaterial>(mpSceneManager->createEntity("universe", ape::Entity::MATERIAL_FILE).lock()))
	{
		universeSkyBoxMaterial->setFileName("universe.material");
		universeSkyBoxMaterial->setAsSkyBox();
	}
	if (auto light = std::static_pointer_cast<ape::ILight>(mpSceneManager->createEntity("light", ape::Entity::LIGHT).lock()))
	{
		light->setLightType(ape::Light::Type::DIRECTIONAL);
		light->setLightDirection(ape::Vector3(1, 0, 0));
		light->setDiffuseColor(ape::Color(0.8f, 0.8f, 0.8f));
		light->setSpecularColor(ape::Color(0.8f, 0.8f, 0.8f));
	}
	mPlanetNode = mpSceneManager->createNode("planetNode");
	if (auto planetNode = mPlanetNode.lock())
	{
		planetNode->setPosition(ape::Vector3(0, 0, -100));
		if (auto planetMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity("planet.mesh", ape::Entity::GEOMETRY_FILE).lock()))
		{
			planetMeshFile->setFileName("planet.mesh");
			planetMeshFile->setParentNode(planetNode);
		}
		if (auto textNode = mpSceneManager->createNode("helloWorldText_Node").lock())
		{
			textNode->setParentNode(planetNode);
			textNode->setPosition(ape::Vector3(0.0f, 10.0f, 0.0f));
			if (auto userNameText = std::static_pointer_cast<ape::ITextGeometry>(mpSceneManager->createEntity("helloWorldText", ape::Entity::GEOMETRY_TEXT).lock()))
			{
				userNameText->setCaption("helloWorld");
				userNameText->setParentNode(textNode);
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHelloWorldPlugin::Run()
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

void ape::apeHelloWorldPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHelloWorldPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHelloWorldPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHelloWorldPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
