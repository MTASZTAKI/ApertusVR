#include <iostream>
#include "ApeAvatarPlugin.h"

ApeAvatarPlugin::ApeAvatarPlugin()
{
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeAvatarPlugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeAvatarPlugin::~ApeAvatarPlugin()
{
	std::cout << "ApeAvatarPlugin dtor" << std::endl;
}

void ApeAvatarPlugin::nodeEventCallBack(const Ape::Event& event)
{
	
}

void ApeAvatarPlugin::Init()
{
	std::cout << "ApeAvatarPlugin::init" << std::endl;
}

void ApeAvatarPlugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeAvatarPlugin::nodeEventCallBack, this, std::placeholders::_1));
}

void ApeAvatarPlugin::Step()
{

}

void ApeAvatarPlugin::Stop()
{

}

void ApeAvatarPlugin::Suspend()
{

}

void ApeAvatarPlugin::Restart()
{

}
