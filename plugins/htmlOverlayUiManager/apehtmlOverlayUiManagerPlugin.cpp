#include <fstream>
#include "apeHtmlOverlayUiManagerPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::HtmlOverlayUiManagerPlugin::HtmlOverlayUiManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&HtmlOverlayUiManagerPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::HtmlOverlayUiManagerPlugin::~HtmlOverlayUiManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&HtmlOverlayUiManagerPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::eventCallBack(const ape::Event& event)
{
}

void ape::HtmlOverlayUiManagerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	mOverlayBrowserCursor = ape::UserInputMacro::OverlayBrowserCursor();
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();

	while (true)
	{
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
