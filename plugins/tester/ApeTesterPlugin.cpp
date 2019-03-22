#include "apeTesterPlugin.h"

ape::apeTesterPlugin::apeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::apeTesterPlugin::~apeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTesterPlugin::eventCallBack(const ape::Event& event)
{

}

void ape::apeTesterPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeGround();
	mpSceneMakerMacro->makeCoordinateSystem();
	mpSceneMakerMacro->makeBox("demoBox");
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTesterPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{	
		mpSceneMakerMacro->interpolate(mpSceneManager->getNode("demoBoxNode"), ape::Vector3(10, 10, 100), ape::Quaternion(0.7071, 0, 0.7071, 0), 10000);
		mpSceneMakerMacro->interpolate(mpSceneManager->getNode("demoBoxNode"), ape::Vector3(10, 10, 0), ape::Quaternion(1, 0, 0, 0), 10000);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTesterPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTesterPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTesterPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeTesterPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
