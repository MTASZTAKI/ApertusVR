#include "ApeTesterPlugin.h"

ape::ApeTesterPlugin::ApeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::ApeTesterPlugin::~ApeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ApeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTesterPlugin::eventCallBack(const ape::Event& event)
{

}

void ape::ApeTesterPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeGround();
	mpSceneMakerMacro->makeCoordinateSystem();
	mpSceneMakerMacro->makeBox("demoBox");
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTesterPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{	
		mpSceneMakerMacro->interpolate(mpSceneManager->getNode("demoBoxNode"), ape::Vector3(10, 10, 100), ape::Quaternion(0.7071, 0, 0.7071, 0), 10000);
		mpSceneMakerMacro->interpolate(mpSceneManager->getNode("demoBoxNode"), ape::Vector3(10, 10, 0), ape::Quaternion(1, 0, 0, 0), 10000);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTesterPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTesterPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTesterPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeTesterPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
