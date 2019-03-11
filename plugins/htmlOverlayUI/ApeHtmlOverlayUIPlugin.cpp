#include <fstream>
#include "ApeHtmlOverlayUIPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

Ape::ApeHtmlOverlayUIPlugin::ApeHtmlOverlayUIPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpApeUserInputMacro = Ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = Ape::UserInputMacro::ViewPose();
	mpSceneMakerMacro = new Ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

Ape::ApeHtmlOverlayUIPlugin::~ApeHtmlOverlayUIPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::eventCallBack(const Ape::Event& event)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeOverlayBrowser();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	double duration = 0;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::ApeHtmlOverlayUIPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
