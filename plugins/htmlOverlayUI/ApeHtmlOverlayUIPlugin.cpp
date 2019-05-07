#include <fstream>
#include "apeHtmlOverlayUIPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::apeHtmlOverlayUIPlugin::apeHtmlOverlayUIPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeHtmlOverlayUIPlugin::~apeHtmlOverlayUIPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtmlOverlayUIPlugin::eventCallBack(const ape::Event& event)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtmlOverlayUIPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpapeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mpSceneMakerMacro->makeOverlayBrowser();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtmlOverlayUIPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	double duration = 0;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeHtmlOverlayUIPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtmlOverlayUIPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtmlOverlayUIPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtmlOverlayUIPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeHtmlOverlayUIPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
