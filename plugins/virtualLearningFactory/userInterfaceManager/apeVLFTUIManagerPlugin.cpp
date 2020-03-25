#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "apeVLFTUIManagerPlugin.h"

ape::VLFTUIManagerPlugin::VLFTUIManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&VLFTUIManagerPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::BROWSER, std::bind(&VLFTUIManagerPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&VLFTUIManagerPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mOverlayBrowserCursor = ape::UserInputMacro::OverlayBrowserCursor();
	mServerPort = 0;
	mMouseMovedValueAbs = ape::Vector2();
	mMouseScrolledValue = 0;
	mClickedNodeNames = std::vector<std::string>();
	mClickedNode = ape::NodeWeakPtr();
	mIsBrowserClicked = false;
	APE_LOG_FUNC_LEAVE();
}

ape::VLFTUIManagerPlugin::~VLFTUIManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&VLFTUIManagerPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTUIManagerPlugin::parseNodeJsPluginConfig()
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

void ape::VLFTUIManagerPlugin::keyPressedStringEventCallback(const std::string & keyValue)
{
	mpUserInputMacro->setOverlayBrowserKeyValue(keyValue);
}

void ape::VLFTUIManagerPlugin::rayQueryIfNotBrowserClick()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(150));
	if (!mIsBrowserClicked)
	{
		mpUserInputMacro->rayQuery(ape::Vector3(mMouseMovedValueAbs.x, mMouseMovedValueAbs.y, 0));
	}
	else
	{
		mIsBrowserClicked = false;
	}
}

void ape::VLFTUIManagerPlugin::mousePressedStringEventCallback(const std::string & keyValue)
{
	if (keyValue == "left")
	{
		mOverlayBrowserCursor.cursorClick = true;
		mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
		mpUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
		auto rayQueryIfNotBrowserClickThread = std::thread(&VLFTUIManagerPlugin::rayQueryIfNotBrowserClick, this);
		rayQueryIfNotBrowserClickThread.detach();
	}
}

void ape::VLFTUIManagerPlugin::mouseReleasedStringEventCallback(const std::string & keyValue)
{
	if (mpUserInputMacro->isOverlayBrowserShowed())
	{
		mOverlayBrowserCursor.cursorClick = false;
		mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
		mpUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
	}
}

void ape::VLFTUIManagerPlugin::mouseMovedCallback(const ape::Vector2 & mouseMovedValueRel, const ape::Vector2 & mouseMovedValueAbs)
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

void ape::VLFTUIManagerPlugin::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		if (auto rayGeometry = std::static_pointer_cast<ape::IRayGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			auto intersections = rayGeometry->getIntersections();
			std::list<ape::EntityWeakPtr> intersectionList;
			std::copy(intersections.begin(), intersections.end(), std::back_inserter(intersectionList));
			mClickedNodeNames.clear();
			mClickedNodeNames.resize(0);
			std::list<ape::EntityWeakPtr>::iterator i = intersectionList.begin();
			for (auto intersection : intersectionList)
			{
				if (auto entity = intersection.lock())
				{
					std::string entityName = entity->getName();
					ape::Entity::Type entityType = entity->getType();
					if (entityType >= ape::Entity::Type::GEOMETRY_FILE && entityType <= ape::Entity::Type::GEOMETRY_CLONE)
					{
						auto geometry = std::static_pointer_cast<ape::Geometry>(entity);
						if (auto clickedNode = geometry->getParentNode().lock())
						{
							mClickedNodeNames.push_back(clickedNode->getName());
						}
					}
				}
			}
			if (auto userNode = mpUserInputMacro->getUserNode().lock())
			{
				for (auto ingoredNode : userNode->getChildNodes())
				{
					if (auto node = ingoredNode.lock())
						eraseClickedNodeName(node);
				}
			}
			if (mClickedNodeNames.size())
			{
				if (auto clickedNode = mpSceneManager->getNode(mClickedNodeNames[0]).lock())
				{
					if (auto previouslyClickedNode = mClickedNode.lock())
					{
						previouslyClickedNode->showBoundingBox(false);
					}
					clickedNode->showBoundingBox(true);
					mClickedNode = clickedNode;
					APE_LOG_DEBUG("ClickedNode: " << clickedNode->getName() << " Position: " << clickedNode->getPosition().toString() << " DerivedPosition: " << clickedNode->getDerivedPosition().toString());
				}
			}
		}
	}
	if (event.type == ape::Event::Type::BROWSER_ELEMENT_CLICK)
	{
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			APE_LOG_DEBUG("BROWSER_ELEMENT_CLICK");
			mIsBrowserClicked = true;
		}
	}
}

void ape::VLFTUIManagerPlugin::eraseClickedNodeName(ape::NodeSharedPtr node)
{
	for (auto ignoredNode : node->getChildNodes())
	{
		if (auto ignoredNodeSP = ignoredNode.lock())
		{
			for (auto it = mClickedNodeNames.begin(); it != mClickedNodeNames.end(); )
			{
				if (*it == ignoredNodeSP->getName())
					it = mClickedNodeNames.erase(it);
				else
					++it;
			}
			eraseClickedNodeName(ignoredNodeSP);
		}
	}
}

void ape::VLFTUIManagerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	parseNodeJsPluginConfig();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mpUserInputMacro->registerCallbackForKeyPressedStringValue(std::bind(&VLFTUIManagerPlugin::keyPressedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMousePressedStringValue(std::bind(&VLFTUIManagerPlugin::mousePressedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMouseReleasedStringValue(std::bind(&VLFTUIManagerPlugin::mouseReleasedStringEventCallback, this, std::placeholders::_1));
	mpUserInputMacro->registerCallbackForMouseMovedValue(std::bind(&VLFTUIManagerPlugin::mouseMovedCallback, this, std::placeholders::_1, std::placeholders::_2));
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTUIManagerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream url;
	url << "http://localhost:" << mServerPort << "/virtualLearningFactoryUI/public/";
	mpSceneMakerMacro->makeOverlayBrowser(url.str());
	while (true)
	{
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTUIManagerPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTUIManagerPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTUIManagerPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTUIManagerPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
