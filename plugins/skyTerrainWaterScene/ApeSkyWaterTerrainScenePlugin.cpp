#include <iostream>
#include "ApeSkyWaterTerrainScenePlugin.h"

ApeSkyWaterTerrainScenePlugin::ApeSkyWaterTerrainScenePlugin()
{
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeSkyWaterTerrainScenePlugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeSkyWaterTerrainScenePlugin::~ApeSkyWaterTerrainScenePlugin()
{
	std::cout << "ApeSkyWaterTerrainScenePlugin dtor" << std::endl;
}

void ApeSkyWaterTerrainScenePlugin::nodeEventCallBack(const Ape::Event& event)
{
	//std::cout  << "event called on the " << event.subjectName << " node" << std::endl;
}

void ApeSkyWaterTerrainScenePlugin::Init()
{
	std::cout << "ApeSkyWaterTerrainScenePlugin::init" << std::endl;
}

void ApeSkyWaterTerrainScenePlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeSkyWaterTerrainScenePlugin::nodeEventCallBack, this, std::placeholders::_1));
}

void ApeSkyWaterTerrainScenePlugin::Step()
{

}

void ApeSkyWaterTerrainScenePlugin::Stop()
{

}

void ApeSkyWaterTerrainScenePlugin::Suspend()
{

}

void ApeSkyWaterTerrainScenePlugin::Restart()
{

}
