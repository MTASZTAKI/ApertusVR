#include <iostream>
#include "ApeAudiPlugin.h"

ape::ApeAudiPlugin::ApeAudiPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApeAudiPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::ApeAudiPlugin::~ApeAudiPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeAudiPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::ApeAudiPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeAudiPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeAudiPlugin::Step()
{

}

void ape::ApeAudiPlugin::Stop()
{

}

void ape::ApeAudiPlugin::Suspend()
{

}

void ape::ApeAudiPlugin::Restart()
{

}
