#include "ApeTesterPlugin.h"

Ape::ApeTesterPlugin::ApeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeTesterPlugin::~ApeTesterPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeTesterPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::eventCallBack(const Ape::Event& event)
{

}

void Ape::ApeTesterPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeBackground();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeGround();
	mpSceneMakerMacro->makeCoordinateSystem();
	mpSceneMakerMacro->makeBox("demoBox");
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{	
		mpSceneMakerMacro->interpolate(mpSceneManager->getNode("demoBoxNode"), Ape::Vector3(10, 10, 100), Ape::Quaternion(0.7071, 0, 0.7071, 0), 10000);
		mpSceneMakerMacro->interpolate(mpSceneManager->getNode("demoBoxNode"), Ape::Vector3(10, 10, 0), Ape::Quaternion(1, 0, 0, 0), 10000);
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeTesterPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
