#include "apePolimiPlugin.h"

ape::apePolimiPlugin::apePolimiPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apePolimiPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&apePolimiPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apePolimiPlugin::~apePolimiPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}



void ape::apePolimiPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::apePolimiPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeCoordinateSystem();
	APE_LOG_FUNC_LEAVE();
}

void ape::apePolimiPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apePolimiPlugin::Step()
{

}

void ape::apePolimiPlugin::Stop()
{

}

void ape::apePolimiPlugin::Suspend()
{

}

void ape::apePolimiPlugin::Restart()
{

}
