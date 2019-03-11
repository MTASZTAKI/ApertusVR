#include <iostream>
#include "ApeAudiPlugin.h"

Ape::ApeAudiPlugin::ApeAudiPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeAudiPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeAudiPlugin::~ApeAudiPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAudiPlugin::eventCallBack(const Ape::Event& event)
{
	
}

void Ape::ApeAudiPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAudiPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeAudiPlugin::Step()
{

}

void Ape::ApeAudiPlugin::Stop()
{

}

void Ape::ApeAudiPlugin::Suspend()
{

}

void Ape::ApeAudiPlugin::Restart()
{

}
