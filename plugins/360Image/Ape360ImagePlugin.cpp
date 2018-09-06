#include <iostream>
#include "Ape360ImagePlugin.h"

Ape::Ape360ImagePlugin::Ape360ImagePlugin()
{
	LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape::Ape360ImagePlugin::~Ape360ImagePlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Init()
{
	LOG_FUNC_ENTER();
	if (auto sphereNode = mpSceneManager->createNode("sphereNode").lock())
	{
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
	LOG_FUNC_LEAVE();
}

void Ape::Ape360ImagePlugin::Run()
{

}

void Ape::Ape360ImagePlugin::Step()
{

}

void Ape::Ape360ImagePlugin::Stop()
{

}

void Ape::Ape360ImagePlugin::Suspend()
{

}

void Ape::Ape360ImagePlugin::Restart()
{

}
