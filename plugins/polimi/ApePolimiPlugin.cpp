#include "ApePolimiPlugin.h"

ape::ApePolimiPlugin::ApePolimiPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApePolimiPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&ApePolimiPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::ApePolimiPlugin::~ApePolimiPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}



void ape::ApePolimiPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::ApePolimiPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApePolimiPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApePolimiPlugin::Step()
{

}

void ape::ApePolimiPlugin::Stop()
{

}

void ape::ApePolimiPlugin::Suspend()
{

}

void ape::ApePolimiPlugin::Restart()
{

}
