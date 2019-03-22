#include "apeEngineeringScenePlugin.h"

ape::apeEngineeringScenePlugin::apeEngineeringScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeEngineeringScenePlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apeEngineeringScenePlugin::~apeEngineeringScenePlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeEngineeringScenePlugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeEngineeringScenePlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeGround();
	mpSceneMakerMacro->makeCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeEngineeringScenePlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeEngineeringScenePlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeEngineeringScenePlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeEngineeringScenePlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeEngineeringScenePlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeEngineeringScenePlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
