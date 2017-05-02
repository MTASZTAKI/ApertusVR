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
	ovr_Initialize();
	mpHMD = NULL;
}

void ApeOculusDK2Plugin::Run()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
