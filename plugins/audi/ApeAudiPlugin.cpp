#include <iostream>
#include "apeAudiPlugin.h"

ape::apeAudiPlugin::apeAudiPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeAudiPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apeAudiPlugin::~apeAudiPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAudiPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::apeAudiPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAudiPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeAudiPlugin::Step()
{

}

void ape::apeAudiPlugin::Stop()
{

}

void ape::apeAudiPlugin::Suspend()
{

}

void ape::apeAudiPlugin::Restart()
{

}
