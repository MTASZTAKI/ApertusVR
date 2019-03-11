#include "ApePolimiPlugin.h"

Ape::ApePolimiPlugin::ApePolimiPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApePolimiPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&ApePolimiPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApePolimiPlugin::~ApePolimiPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}



void Ape::ApePolimiPlugin::eventCallBack(const Ape::Event& event)
{
	
}

void Ape::ApePolimiPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePolimiPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApePolimiPlugin::Step()
{

}

void Ape::ApePolimiPlugin::Stop()
{

}

void Ape::ApePolimiPlugin::Suspend()
{

}

void Ape::ApePolimiPlugin::Restart()
{

}
