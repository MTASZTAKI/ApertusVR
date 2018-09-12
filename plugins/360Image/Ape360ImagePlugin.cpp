#include "Ape360ImagePlugin.h"

Ape::Ape360ImagePlugin::Ape360ImagePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::Ape360ImagePlugin::~Ape360ImagePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (auto sphereNode = mpSceneManager->createNode("sphereNode").lock())
	{
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
