#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "apeHtmlOverlayUiManagerPlugin.h"

ape::HtmlOverlayUiManagerPlugin::HtmlOverlayUiManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&HtmlOverlayUiManagerPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mOverlayBrowserCursor = ape::UserInputMacro::OverlayBrowserCursor();
	mServerPort = 0;
	mMouseMovedValueAbs = ape::Vector2();
	mMouseScrolledValue = 0;
	APE_LOG_FUNC_LEAVE();
}

ape::HtmlOverlayUiManagerPlugin::~HtmlOverlayUiManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&HtmlOverlayUiManagerPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::parseNodeJsPluginConfig()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeNodeJsPlugin.json";
	FILE* configFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (configFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(configFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject() && jsonDocument.HasMember("httpServer"))
		{
			rapidjson::Value& httpServer = jsonDocument["httpServer"];
			if (httpServer.IsObject() && httpServer.HasMember("port"))
			{
				rapidjson::Value& port = httpServer["port"];
				if (port.IsNumber())
				{
					mServerPort = port.GetInt();
				}
			}
		}
		fclose(configFile);
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::mousePressedStringEventCallback(const std::string & keyValue)
{
	if (keyValue == "left")
	{
		if (mpUserInputMacro->isOverlayBrowserShowed())
		{
			mOverlayBrowserCursor.cursorClick = true;
			mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
			mpUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
		}
		else
		{
			mpUserInputMacro->rayQuery(ape::Vector3(mMouseMovedValueAbs.x, mMouseMovedValueAbs.y, 0));
		}
	}
}

void ape::HtmlOverlayUiManagerPlugin::mouseReleasedStringEventCallback(const std::string & keyValue)
{
	if (mpUserInputMacro->isOverlayBrowserShowed())
	{
		mOverlayBrowserCursor.cursorClick = false;
		mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
		mpUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
	}
}

void ape::HtmlOverlayUiManagerPlugin::mouseMovedCallback(const ape::Vector2 & mouseMovedValueRel, const ape::Vector2 & mouseMovedValueAbs)
{
	mMouseMovedValueAbs = mouseMovedValueAbs;
	if (mpUserInputMacro->isOverlayBrowserShowed())
	{
		ape::Vector2 cursorTexturePosition;
		cursorTexturePosition.x = (float)-mMouseMovedValueAbs.x / (float)mpCoreConfig->getWindowConfig().width;
		cursorTexturePosition.y = (float)-mMouseMovedValueAbs.y / (float)mpCoreConfig->getWindowConfig().height;
		ape::Vector2 cursorBrowserPosition;
		cursorBrowserPosition.x = (float)mMouseMovedValueAbs.x / (float)mpCoreConfig->getWindowConfig().width;
		cursorBrowserPosition.y = (float)mMouseMovedValueAbs.y / (float)mpCoreConfig->getWindowConfig().height;
		mOverlayBrowserCursor.cursorBrowserPosition = cursorBrowserPosition;
		mOverlayBrowserCursor.cursorTexturePosition = cursorTexturePosition;
		mOverlayBrowserCursor.cursorScrollPosition = ape::Vector2(0, mMouseScrolledValue);
		mOverlayBrowserCursor.cursorClick = false;
		mpUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
	}
}

void ape::HtmlOverlayUiManagerPlugin::eventCallBack(const ape::Event& event)
{
}

void ape::HtmlOverlayUiManagerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	parseNodeJsPluginConfig();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mpUserInputMacro->registerCallbackForMousePressedStringValue(std::bind(&HtmlOverlayUiManagerPlugin::mousePressedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMouseReleasedStringValue(std::bind(&HtmlOverlayUiManagerPlugin::mouseReleasedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMouseMovedValue(std::bind(&HtmlOverlayUiManagerPlugin::mouseMovedCallback, this, std::placeholders::_1, std::placeholders::_2));
	APE_LOG_FUNC_LEAVE();
}

void ape::HtmlOverlayUiManagerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream url;
	url << "http://localhost:" << mServerPort << "/robotCalibration/public/";
	mpSceneMakerMacro->makeOverlayBrowser(url.str());
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
