#include <fstream>
#include "ApeHtmlOverlayUIPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::ApeHtmlOverlayUIPlugin::ApeHtmlOverlayUIPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&ApeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	APE_LOG_FUNC_LEAVE();
}

ape::ApeHtmlOverlayUIPlugin::~ApeHtmlOverlayUIPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeHtmlOverlayUIPlugin::eventCallBack(const ape::Event& event)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeHtmlOverlayUIPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeOverlayBrowser();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeHtmlOverlayUIPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	double duration = 0;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&ApeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeHtmlOverlayUIPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeHtmlOverlayUIPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeHtmlOverlayUIPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::ApeHtmlOverlayUIPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
