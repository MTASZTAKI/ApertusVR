#include <iostream>
#include "ApeOculusDK2Plugin.h"

ApeOculusDK2Plugin::ApeOculusDK2Plugin()
{
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeOculusDK2Plugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeOculusDK2Plugin::~ApeOculusDK2Plugin()
{
	std::cout << "ApeOculusDK2Plugin dtor" << std::endl;
}

void ApeOculusDK2Plugin::nodeEventCallBack(const Ape::Event& event)
{
	//std::cout  << "event called on the " << event.subjectName << " node" << std::endl;
}

void ApeOculusDK2Plugin::Init()
{
	std::cout << "ApeOculusDK2Plugin::init" << std::endl;
	mPlanetNode = mpScene->createNode("planetNode");
	if (auto planetNode = mPlanetNode.lock())
	{
		if (auto universeSkyBoxMaterial = std::static_pointer_cast<Ape::IFileMaterial>(mpScene->createEntity("universe", Ape::Entity::MATERIAL_FILE).lock()))
		{
			universeSkyBoxMaterial->setFileName("universe.material");
			universeSkyBoxMaterial->setAsSkyBox();
		}
		if (auto planetMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("planet.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			planetMeshFile->setFileName("planet.mesh");
			planetMeshFile->setParentNode(planetNode);
		}
		if (auto helloWorldText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity("helloWorldText", Ape::Entity::GEOMETRY_TEXT).lock()))
		{
			helloWorldText->setOffset(Ape::Vector3(0.0f, 10.0f, 0.0f));
			helloWorldText->setCaption("helloWorld");
			helloWorldText->setParentNode(planetNode);
		}
	}
}

void ApeOculusDK2Plugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		if (auto planetNode = mPlanetNode.lock())
			planetNode->rotate(0.0017f, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::LOCAL);
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeOculusDK2Plugin::nodeEventCallBack, this, std::placeholders::_1));
}

void ApeOculusDK2Plugin::Step()
{

}

void ApeOculusDK2Plugin::Stop()
{

}

void ApeOculusDK2Plugin::Suspend()
{

}

void ApeOculusDK2Plugin::Restart()
{

}
