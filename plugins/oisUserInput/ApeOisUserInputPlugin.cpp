#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "ApeOisUserInputPlugin.h"

Ape::OISUserInputPlugin::OISUserInputPlugin()
{
	LOG_FUNC_ENTER();
	mpKeyboard = NULL;
	mpMouse = NULL;
	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	mTranslateSpeedFactor = 3;
	mRotateSpeedFactor = 1;
	mOverlayBrowser = Ape::BrowserWeakPtr();
	mOverlayMouseTexture = Ape::UnitTextureWeakPtr();
	mRayGeometry = Ape::RayGeometryWeakPtr();
	mRayOverlayNode = Ape::NodeWeakPtr();
	mHeadNode = Ape::NodeWeakPtr();
	mDummyNode = Ape::NodeWeakPtr();
	mIsNewKeyEvent = false;
	mEnableOverlayBrowserKeyEvents = false;
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::BROWSER, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mUserNodePoses = std::vector<UserNodePose>();

	/*gallery*/
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-941.765, 47.9583, 631.489), Ape::Quaternion(0.977507, -0.210903, -4.55392e-07, 2.41213e-07)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-895.301, -22.3373, -44.3792), Ape::Quaternion(0.9987, -0.0509765, -4.37447e-07, 3.08579e-07)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, 0), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, 0), Ape::Quaternion(0, 0, 1, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, 0), Ape::Quaternion(1, 0, -0.05, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, -600), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, -600), Ape::Quaternion(0, 0, 1, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, -600, -600), Ape::Quaternion(1, 0, -0.05, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, -500), Ape::Quaternion(0, 0, 1, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, -500), Ape::Quaternion(1, 0, -0.05, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, 0, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, 0, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, 0, -500), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, -600, 100), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-1200, 0, 100), Ape::Quaternion(1, 0, 0, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-600, 0, 100), Ape::Quaternion(1, 0, 0, 0)));


	/*mUserNodePoses.push_back(UserNodePose(Ape::Vector3(100.079, -583, -478.537), Ape::Quaternion(0.250597, 0, 0.968092, 0)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-50.2942, -267, -39.543), Ape::Quaternion(0.108578, 0.000922807, -0.994053, 0.00844969)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(109.65, -254.404, 683.883), Ape::Quaternion(-0.994235, -0.00845123, -0.106884, 0.000908416)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(182.939, -582, 322.612), Ape::Quaternion(0.299624, 3.09414e-10, 0.954057, -8.78543e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-33.8135, -582, 524.087), Ape::Quaternion(-0.0356937, -1.2817e-12, 0.999363, -9.31436e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(136.762, -582, 607.147), Ape::Quaternion(-0.938136, -8.84368e-10, -0.346269, 2.9235e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-636.561, -267, 251.884), Ape::Quaternion(0.809606, 7.34888e-10, -0.586974, 5.72286e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(47.2687, -579, -656.354), Ape::Quaternion(-0.152424, -1.73481e-10, -0.988316, 9.15137e-10)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(95.8991, -579, -640.988), Ape::Quaternion(-0.152424, -1.73481e-10, -0.988316, 9.15137e-10)));

	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(332.168, -562, -343.67), Ape::Quaternion(-0.919263, 0, 0.393645, 0)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(419.488, -510.18, -28.347), Ape::Quaternion(-0.728918, 0.0309977, 0.683282, 0.029057)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-269.323, -469.674, 275.482), Ape::Quaternion(0.506584, -0.0475041, 0.857121, 0.0803752)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(-374.755, -506.984, 53.2619), Ape::Quaternion(0.689286, -0.0528335, 0.720448, 0.0552219)));*/

	mUserNodePosesToggleIndex = 0;
	mIsKeyPressed = false;
	LOG_FUNC_LEAVE();
}

Ape::OISUserInputPlugin::~OISUserInputPlugin()
{
	LOG_FUNC_ENTER();
	delete mpKeyboard;
	delete mpMouse;
	LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE)
	{
		if (event.subjectName == "robotRootNode")
		{
			mNodeToMove = mpScene->getNode(event.subjectName);
		}
	}
	else if (event.type == Ape::Event::Type::BROWSER_OVERLAY)
	{
		mOverlayBrowser = std::static_pointer_cast<Ape::IBrowser>(mpScene->getEntity(event.subjectName).lock());
		LOG(LOG_TYPE_DEBUG, "overlayBrowser catched");
	}
	else if (event.type == Ape::Event::Type::TEXTURE_UNIT_CREATE)
	{
		mOverlayMouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpScene->getEntity(event.subjectName).lock());
		LOG(LOG_TYPE_DEBUG, "overlayMouseTexture catched");
	}
	else if (event.type == Ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD)
	{
		LOG_TRACE("BROWSER_FOCUS_ON_EDITABLE_FIELD");
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			if (auto focusChangedBrowser = std::static_pointer_cast<Ape::IBrowser>(mpScene->getEntity(event.subjectName).lock()))
			{
				mEnableOverlayBrowserKeyEvents = focusChangedBrowser->isFocusOnEditableField() && overlayBrowser->getName() == focusChangedBrowser->getName();
				LOG_TRACE("mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
				mIsNewKeyEvent = true;
			}
		}
	}
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		LOG_TRACE("GEOMETRY_RAY_INTERSECTION");
		if (auto rayGeometry = mRayGeometry.lock())
		{
			auto intersections = rayGeometry->getIntersections();
			for (auto intersection : intersections)
			{
				if (auto geometry = intersection.lock())
				{
					std::size_t found = geometry->getName().find(mUserNode.lock()->getName()); 
					std::size_t foundCoord = geometry->getName().find("coord");
					if (found == std::string::npos && foundCoord == std::string::npos) //Ignore our avatar and coordinatesystems
					{
						LOG(LOG_TYPE_DEBUG, "GEOMETRY_RAY_INTERSECTION: " << geometry->getName() << " type: " << geometry->getType());
						if (auto selectedParentNode = geometry->getParentNode().lock())
						{
							if (!mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] && !mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
							{
								clearNodeSelection();
								addNodeSelection(selectedParentNode);
							}
							else
							{
								if (isNodeSelected(selectedParentNode->getName()))
									removeNodeSelection(selectedParentNode->getName());
								else
									addNodeSelection(selectedParentNode);
							}
						}
						break;
					}
				}
			}
		}
	}
}

void Ape::OISUserInputPlugin::Init()
{
	LOG_FUNC_ENTER();

	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		mUserNode = userNode;
		mDummyNode = mpScene->createNode(userNode->getName() + "_DummyNode");
		if (auto headNode = mpScene->getNode(userNode->getName() + "_HeadNode").lock())
		{
			mHeadNode = headNode;
		}
	}

	Ape::OisWindowConfig oisWindowConfig;
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "\\ApeOisUserInputPlugin.json";
	FILE* apeOisUserInputConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeOisUserInputConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeOisUserInputConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& inputs = jsonDocument["inputs"];
			for (auto& input : inputs.GetArray())
			{
				oisWindowConfig.inputs.push_back(input.GetString());
			}
		}
		fclose(apeOisUserInputConfigFile);
	}

	LOG(LOG_TYPE_DEBUG, "OISUserInputPlugin waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "OisUserInputPlugin main window was found");

	std::stringstream hwndStrStream;
	hwndStrStream << mpMainWindow->getHandle();
	std::stringstream windowHndStr;
	windowHndStr << std::stoul(hwndStrStream.str(), nullptr, 16);
	oisWindowConfig.handler = windowHndStr.str();
	oisWindowConfig.width = mpMainWindow->getWidth();
	oisWindowConfig.height = mpMainWindow->getHeight();



	OIS::ParamList pl;
	pl.insert(std::make_pair("WINDOW", oisWindowConfig.handler));
#ifdef WIN32
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#endif
	OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
	for (auto it = oisWindowConfig.inputs.begin(); it != oisWindowConfig.inputs.end(); it++)
	{
		if ((*it) == "keyboard" && inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
		{
			OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
			mpKeyboard = keyboard;
			mpKeyboard->setEventCallback(this);
		}
		else if ((*it) == "mouse" && inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
		{
			OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
			mpMouse = mouse;
			mpMouse->setEventCallback(this);
			const OIS::MouseState &ms = mouse->getMouseState();
			ms.width = oisWindowConfig.width;
			ms.height = oisWindowConfig.height;
		}
	}

	if (auto rayNode = mpScene->createNode("rayNode" + mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		if (auto rayGeometry = std::static_pointer_cast<Ape::IRayGeometry>(mpScene->createEntity("rayQuery" + mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName, Ape::Entity::GEOMETRY_RAY).lock()))
		{
			rayGeometry->setIntersectingEnabled(true);
			rayGeometry->setParentNode(rayNode);
			mRayGeometry = rayGeometry;
		}
		rayNode->setParentNode(mUserNode);
		mRayOverlayNode = rayNode;
	}

	LOG_FUNC_LEAVE();
}

bool Ape::OISUserInputPlugin::keyPressed(const OIS::KeyEvent& e)
{
	LOG_TRACE("-------------------------------------------------------");
	LOG_TRACE("mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
	LOG_TRACE("OIS::KeyCode: " << (OIS::KeyCode)e.key);

	mKeyCodeMap[e.key] = true;

	if (e.key == OIS::KeyCode::KC_C)
	{
		saveUserNodePose();
	}

	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		std::string keyAsString = mpKeyboard->getAsString(e.key);

		if (!mKeyCodeMap[OIS::KeyCode::KC_LSHIFT] && !mKeyCodeMap[OIS::KeyCode::KC_RSHIFT])
			std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);

		LOG_TRACE("keyAsString:" << keyAsString);
		std::wstring keyAsWString(keyAsString.begin(), keyAsString.end());
		if (e.key == OIS::KeyCode::KC_BACK)
			keyAsWString = 8;
		else if (e.key == OIS::KeyCode::KC_TAB)
			keyAsWString = 9;
		else if (e.key == OIS::KeyCode::KC_RETURN)
			keyAsWString = 13;
		else if (e.key == OIS::KeyCode::KC_LSHIFT || e.key == OIS::KeyCode::KC_RSHIFT)
			keyAsWString = 14;
		else if (e.key == OIS::KeyCode::KC_SPACE)
			keyAsWString = 32;
		else if (e.key == OIS::KeyCode::KC_END)
			keyAsWString = 35;
		else if (e.key == OIS::KeyCode::KC_HOME)
			keyAsWString = 36;
		else if (e.key == OIS::KeyCode::KC_LEFT)
			keyAsWString = 37;
		else if (e.key == OIS::KeyCode::KC_UP)
			keyAsWString = 38;
		else if (e.key == OIS::KeyCode::KC_RIGHT)
			keyAsWString = 39;
		else if (e.key == OIS::KeyCode::KC_DOWN)
			keyAsWString = 40;
		else if (e.key == OIS::KeyCode::KC_DELETE)
			keyAsWString = 46;
		else if (e.key == OIS::KeyCode::KC_PERIOD)
			keyAsWString = 1046;
		mIsNewKeyEvent = false;
		overlayBrowser->keyASCIIValue(keyAsWString[0]);

		LOG_TRACE("Before waiting: mIsNewKeyEvent: " << mIsNewKeyEvent);
		while (!mIsNewKeyEvent)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		mIsNewKeyEvent = false;
		LOG_TRACE("After waiting mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
	}

	return true;
}

bool Ape::OISUserInputPlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	return true;
}

bool Ape::OISUserInputPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	mMouseState.posCurrent = e.state;
	mMouseState.isMouseMoved = true;
	for (auto nodeIt = mSelectedNodes.begin(); nodeIt != mSelectedNodes.end(); nodeIt++)
	{
		if (auto selectedNodeInMap = nodeIt->second.lock())
		{
			if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Left])
			{
				if (mKeyCodeMap[OIS::KeyCode::KC_LSHIFT] || mKeyCodeMap[OIS::KeyCode::KC_RSHIFT])
				{
					LOG_TRACE("X velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragMode = true;
					selectedNodeInMap->translate(Ape::Vector3((mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs), 0, 0), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] || mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
				{
					LOG_TRACE("Y velocity: " << mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs);
					mMouseState.isDragMode = true;
					selectedNodeInMap->translate(Ape::Vector3(0, -(mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs), 0), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_LMENU] || mKeyCodeMap[OIS::KeyCode::KC_RMENU])
				{
					LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragMode = true;
					selectedNodeInMap->translate(Ape::Vector3(0, 0, -(mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs)), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
				{
					LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragMode = true;
					selectedNodeInMap->rotate(Ape::Degree(mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
				}
			}
		}
	}
	mMouseState.posPrevious = mMouseState.posCurrent;

	if (auto overlayMouseTexture = mOverlayMouseTexture.lock())
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			Ape::Vector2 cursorTexturePosition;
			cursorTexturePosition.x = (float)-e.state.X.abs / (float)mpMainWindow->getWidth();
			cursorTexturePosition.y = (float)-e.state.Y.abs / (float)mpMainWindow->getHeight();
			overlayMouseTexture->setTextureScroll(cursorTexturePosition.x, cursorTexturePosition.y);
			Ape::Vector2 cursorBrowserPosition;
			cursorBrowserPosition.x = ((float)e.state.X.abs / (float)mpMainWindow->getWidth()) * overlayBrowser->getResoultion().x;
			cursorBrowserPosition.y = ((float)e.state.Y.abs / (float)mpMainWindow->getHeight()) * overlayBrowser->getResoultion().y;
			overlayBrowser->mouseMoved(cursorBrowserPosition);
			overlayBrowser->mouseScroll(Ape::Vector2(0, e.state.Z.rel));
			//LOG(LOG_TYPE_DEBUG, "cursorTexturePosition:" << cursorTexturePosition.x << ";" << cursorTexturePosition.y);
			//LOG(LOG_TYPE_DEBUG, "cursorBrowserPosition:" << cursorBrowserPosition.x << ";" << cursorBrowserPosition.y);
		}
	}
	return true;
}

bool Ape::OISUserInputPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	LOG_TRACE("-------------------------------------------------------");
	mMouseState.buttonDownMap[id] = true;
	mMouseState.posStart = e.state;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			LOG_TRACE("overlayBrowser->mouseClick");
			overlayBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, true);
		}
	}
	return true;
}

bool Ape::OISUserInputPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	LOG_TRACE("mMouseState.isDragMode: " << mMouseState.isDragMode);
	mMouseState.buttonDownMap[id] = false;
	mMouseState.posEnd = e.state;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			overlayBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, false);
			LOG_TRACE("overlayBrowser->mouseClick");
		}

		if (!mMouseState.isDragMode)
		{
			if (!mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] && !mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
			{
				clearNodeSelection();
			}

			if (auto rayOverlayNode = mRayOverlayNode.lock())
			{
				LOG_TRACE("rayGeomtery->fireIntersectionQuery x: " << e.state.X.abs << " y: " << e.state.Y.abs);
				rayOverlayNode->setPosition(Ape::Vector3(e.state.X.abs, e.state.Y.abs, 0));
				if (auto rayGeomtery = mRayGeometry.lock())
					rayGeomtery->fireIntersectionQuery();
			}
		}
	}

	mMouseState.isDragMode = false;
	return true;
}

bool Ape::OISUserInputPlugin::isNodeSelected(std::string nodeName)
{
	LOG_TRACE("nodeName: " << nodeName);
	std::map<std::string, Ape::NodeWeakPtr>::iterator findIt;
	findIt = mSelectedNodes.find(nodeName);
	return (findIt != mSelectedNodes.end());
}

void Ape::OISUserInputPlugin::addNodeSelection(Ape::NodeWeakPtr node)
{
	if (auto nodeSharedPtr = node.lock())
	{
		LOG_TRACE("nodeName: " << nodeSharedPtr->getName());
		mSelectedNodes.insert(std::pair<std::string, Ape::NodeWeakPtr>(nodeSharedPtr->getName(), node));
		nodeSharedPtr->showBoundingBox(true);
	}
}

bool Ape::OISUserInputPlugin::removeNodeSelection(std::string nodeName)
{
	LOG_TRACE("nodeName: " << nodeName);
	std::map<std::string, Ape::NodeWeakPtr>::iterator findIt;
	findIt = mSelectedNodes.find(nodeName);
	if (findIt != mSelectedNodes.end())
	{
		if (auto findNode = findIt->second.lock())
		{
			findNode->showBoundingBox(false);
			mSelectedNodes.erase(findIt);
			return true;
		}
	}
	return false;
}

void Ape::OISUserInputPlugin::clearNodeSelection()
{
	LOG_TRACE("");
	auto nodeIt = mSelectedNodes.begin();
	while (nodeIt != mSelectedNodes.end())
	{
		if (auto selectedNodeInMap = nodeIt->second.lock())
		{
			selectedNodeInMap->showBoundingBox(false);
			nodeIt = mSelectedNodes.erase(nodeIt);
		}
		else
		{
			++nodeIt;
		}
	}
}

void Ape::OISUserInputPlugin::saveUserNodePose()
{
	if (auto userNode = mUserNode.lock())
	{
		std::ofstream userNodePoseFile;
		userNodePoseFile.open("userNodePoseFile.txt", std::ios::app);
		userNodePoseFile << userNode->getPosition().x << "," << userNode->getPosition().y << "," << userNode->getPosition().z << " : " <<
			userNode->getOrientation().w << "," << userNode->getOrientation().x << "," << userNode->getOrientation().y << "," << userNode->getOrientation().z << std::endl;
		userNodePoseFile.close();
	}
}

void Ape::OISUserInputPlugin::toggleUserNodePoses(Ape::NodeSharedPtr userNode)
{
	userNode->setPosition(mUserNodePoses[mUserNodePosesToggleIndex].position);
	userNode->setOrientation(mUserNodePoses[mUserNodePosesToggleIndex].orientation);
	mUserNodePosesToggleIndex++;
	if (mUserNodePoses.size() == mUserNodePosesToggleIndex)
		mUserNodePosesToggleIndex = 0;
}


void Ape::OISUserInputPlugin::moveUserNodeByKeyBoard()
{
	if (auto userNode = mUserNode.lock())
	{
		if (auto dummyNode = mDummyNode.lock())
		{
			if (auto headNode = mHeadNode.lock())
			{
				dummyNode->setOrientation(headNode->getDerivedOrientation());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_PGUP])
			{
				dummyNode->translate(Ape::Vector3(0, 1 * mTranslateSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_PGDOWN])
			{
				dummyNode->translate(Ape::Vector3(0, -1 * mTranslateSpeedFactor, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_D])
			{
				dummyNode->translate(Ape::Vector3(1 * mTranslateSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_A])
			{
				dummyNode->translate(Ape::Vector3(-1 * mTranslateSpeedFactor, 0, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_W])
			{
				dummyNode->translate(Ape::Vector3(0, 0, -1 * mTranslateSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_S])
			{
				dummyNode->translate(Ape::Vector3(0, 0, 1 * mTranslateSpeedFactor), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_LEFT])
			{
				userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_RIGHT])
			{
				userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_UP])
			{
				userNode->rotate(0.017f * mRotateSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_DOWN])
			{
				userNode->rotate(-0.017f * mRotateSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
			}
		}
	}
}

void Ape::OISUserInputPlugin::moveUserNodeByMouse()
{
	if (auto userNode = mUserNode.lock())
	{
		if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Right] && mMouseState.isMouseMoved)
		{
			float rotateSpeedFactor = 0.2;
			userNode->rotate(Ape::Degree(-mMouseState.posCurrent.Y.rel).toRadian() * rotateSpeedFactor, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
			userNode->rotate(Ape::Degree(-mMouseState.posCurrent.X.rel).toRadian() * rotateSpeedFactor, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			mMouseState.isMouseMoved = false;
		}
	}
}

void Ape::OISUserInputPlugin::Run()
{
	while (true)
	{
		if (mpKeyboard)
			mpKeyboard->capture();
		if (mpMouse)
			mpMouse->capture();
		if (!mEnableOverlayBrowserKeyEvents)
		{
			moveUserNodeByKeyBoard();
			moveUserNodeByMouse();
		}
		std::this_thread::sleep_for (std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
}

void Ape::OISUserInputPlugin::Step()
{
	
}

void Ape::OISUserInputPlugin::Stop()
{
	
}

void Ape::OISUserInputPlugin::Suspend()
{
	
}

void Ape::OISUserInputPlugin::Restart()
{
	
}
