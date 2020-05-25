#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "apeVLFTUIManagerPlugin.h"

ape::VLFTUIManagerPlugin::VLFTUIManagerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
	mpPluginManager = ape::IPluginManager::getSingletonPtr();
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
	mIsBrowserHovered = false;
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


void ape::VLFTUIManagerPlugin::mousePressedStringEventCallback(const std::string & keyValue)
{
	if (keyValue == "left")
	{
		mOverlayBrowserCursor.cursorClick = true;
		mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
		mpUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
		if (!mIsBrowserHovered)
			mpUserInputMacro->rayQuery(ape::Vector3(mMouseMovedValueAbs.x, mMouseMovedValueAbs.y, 0));
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
					//APE_LOG_DEBUG("entityName: " << entityName);
					ape::Entity::Type entityType = entity->getType();
					if (entityType >= ape::Entity::Type::GEOMETRY_FILE && entityType <= ape::Entity::Type::GEOMETRY_CLONE)
					{
						auto geometry = std::static_pointer_cast<ape::Geometry>(entity);
						//APE_LOG_DEBUG("geometry: " << entityName);
						if (auto clickedNode = geometry->getParentNode().lock())
						{
							//APE_LOG_DEBUG("clickedNode: " << clickedNode->getName());
							mClickedNodeNames.push_back(clickedNode->getName());
						}
						else
						{
							;// APE_LOG_DEBUG("noParent: " << entityName);
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
				for (auto clickedNodeName : mClickedNodeNames)
				{
					if (auto clickedNode = mpSceneManager->getNode(clickedNodeName).lock())
					{
						if (clickedNode->isVisible())
						{
							if (auto previouslyClickedNode = mClickedNode.lock())
							{
								previouslyClickedNode->showBoundingBox(false);
							}
							/*while (auto parentNode = clickedNode->getParentNode().lock())
							{
								clickedNode = parentNode;
							}*/
							clickedNode->showBoundingBox(true);
							mClickedNode = clickedNode;
							break;
							//APE_LOG_DEBUG("ClickedNode: " << clickedNode->getName() << " Position: " << clickedNode->getPosition().toString() << " DerivedPosition: " << clickedNode->getDerivedPosition().toString());
						}
					}
				}
			}
		}
	}
	else if (event.type == ape::Event::Type::BROWSER_ELEMENT_CLICK)
	{
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			//APE_LOG_DEBUG("BROWSER_ELEMENT_CLICK");
			mIsBrowserClicked = true;
			if (browser->getClickedElementName().find("connect") != std::string::npos)
			{
				std::string userType;
				std::string userName;
				std::string roomName;
				auto userTypePos = browser->getClickedElementName().find(";userType:");
				auto roomNamePos = browser->getClickedElementName().find(";roomName:");
				auto userNamePos = browser->getClickedElementName().find(";userName:");
				userType = browser->getClickedElementName().substr(userTypePos + 10, roomNamePos - (userTypePos + 10));
				roomName = browser->getClickedElementName().substr(roomNamePos + 10, userNamePos - (roomNamePos + 10));
				userName = browser->getClickedElementName().substr(userNamePos + 10, browser->getClickedElementName().length());
				APE_LOG_DEBUG("userType: " << userType);
				APE_LOG_DEBUG("roomName: " << roomName);
				APE_LOG_DEBUG("userName: " << userName);
				if (userType == "_Local")
				{
					std::string urlSceneConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/" + roomName + "/apeVLFTSceneLoaderPlugin.json";
					std::string locationSceneConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTSceneLoaderPlugin.json";
					std::string urlAnimationConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/" + roomName + "/apeVLFTAnimationPlayerPlugin.json";
					std::string locationAnimationConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTAnimationPlayerPlugin.json";
					std::vector<std::string> urls;
					std::vector<std::string> locations;
					urls.push_back(urlAnimationConfig);
					locations.push_back(locationAnimationConfig);
					urls.push_back(urlSceneConfig);
					locations.push_back(locationSceneConfig);
					mpSceneNetwork->downloadConfigs(urls, locations);
					mpPluginManager->loadPlugin("apeSampleScenePlugin");
					mpPluginManager->loadPlugin("apeVLFTSceneLoaderPlugin");
					mpPluginManager->loadPlugin("apeVLFTAnimationPlayerPlugin");
				}
				else if (userType == "_Teacher" || userType == "_Student")
				{
					std::string urlAnimationConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/" + roomName + "/apeVLFTAnimationPlayerPlugin.json";
					std::string locationAnimationConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTAnimationPlayerPlugin.json";
					std::vector<std::string> urls;
					std::vector<std::string> locations;
					urls.push_back(urlAnimationConfig);
					locations.push_back(locationAnimationConfig);
					mpSceneNetwork->connectToRoom(roomName, urls, locations);
					mpPluginManager->loadPlugin("apeVLFTAnimationPlayerPlugin");
				}
			}
		}
	}
	else if (event.type == ape::Event::Type::BROWSER_HOVER_IN)
	{
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			//APE_LOG_DEBUG("BROWSER_HOVER_IN");
			mIsBrowserHovered = true;
		}
	}
	else if (event.type == ape::Event::Type::BROWSER_HOVER_OUT)
	{
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			//APE_LOG_DEBUG("BROWSER_HOVER_OUT");
			mIsBrowserHovered = false;
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
