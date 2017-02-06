#include <iostream>
#include "ApePresentationScenePlugin.h"

ApePresentationScenePlugin::ApePresentationScenePlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApePresentationScenePlugin::~ApePresentationScenePlugin()
{
	std::cout << "ApePresentationScenePlugin dtor" << std::endl;
}

void ApePresentationScenePlugin::eventCallBack(const Ape::Event& event)
{

}

void ApePresentationScenePlugin::Init()
{
	std::cout << "ApePresentationScenePlugin::init" << std::endl;
	if (auto presentationNode = mpScene->createNode("presentationNode").lock())
	{
		if (auto presentationMeshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity("metalroom.mesh", Ape::Entity::GEOMETRY_FILE).lock()))
		{
			presentationMeshFile->setFileName("metalroom.mesh");
			presentationMeshFile->setParentNode(presentationNode);
		}
	}
}

void ApePresentationScenePlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApePresentationScenePlugin::eventCallBack, this, std::placeholders::_1));
}

void ApePresentationScenePlugin::Step()
{

}

void ApePresentationScenePlugin::Stop()
{

}

void ApePresentationScenePlugin::Suspend()
{

}

void ApePresentationScenePlugin::Restart()
{

}
