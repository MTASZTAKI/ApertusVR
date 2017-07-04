#include <iostream>
#include "Ape360ImagePlugin.h"

Ape360ImagePlugin::Ape360ImagePlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
}

Ape360ImagePlugin::~Ape360ImagePlugin()
{
	std::cout << "Ape360ImagePlugin dtor" << std::endl;
}

void Ape360ImagePlugin::Init()
{
	std::cout << "Ape360ImagePlugin::init" << std::endl;
	if (auto sphereNode = mpScene->createNode("sphereNode").lock())
	{
		if (auto sphereMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("sphere.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			sphereMeshFile->setFileName("sphere.mesh");
			sphereMeshFile->setParentNode(sphereNode);
		}
	}
}

void Ape360ImagePlugin::Run()
{

}

void Ape360ImagePlugin::Step()
{

}

void Ape360ImagePlugin::Stop()
{

}

void Ape360ImagePlugin::Suspend()
{

}

void Ape360ImagePlugin::Restart()
{

}
