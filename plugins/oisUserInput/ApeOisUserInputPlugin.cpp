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
	mTranslateSpeedFactorKeyboard = 3;
	mRotateSpeedFactorKeyboard = 1;
	mTranslateSpeedFactorMouse = 1.2;
	mRotateSpeedFactorMouse = 0.2;
	mGeneralSpeedFactor = 0;
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
	//mUserNodePoses.push_back(UserNodePose(Ape::Vector3(119.989, 1.55319, -248.898), Ape::Quaternion(0.672635, 0.00571758, 0.739926, -0.0062896)));
	mUserNodePoses.push_back(UserNodePose(Ape::Vector3(62.3872, 138.513, -227.567), Ape::Quaternion(-0.526266, 0, -0.85032, 0)));
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
	/*else if (event.type == Ape::Event::Type::NODE_ORIENTATION)
	{
		if (event.subjectName == mHeadNodeName)
		{
			if (auto dummyNode = mDummyNode.lock())
			{
				if (auto headNode = mHeadNode.lock())
				{
					dummyNode->setOrientation(headNode->getDerivedOrientation());
				}
			}
		}
	}*/
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
						LOG(LOG_TYPE_DEBUG, "GEOMETRY_RAY_INTERSECTION: geometry: " << geometry->getName() << " type: " << geometry->getType());
						if (auto selectedParentNode = geometry->getParentNode().lock())
						{
							LOG(LOG_TYPE_DEBUG, "GEOMETRY_RAY_INTERSECTION: parentNode: " << selectedParentNode->getName());
							if (!mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] && !mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
							{
								clearNodeSelection();
								addNodeSelection(selectedParentNode->getName());
							}
							else
							{
								if (isNodeSelected(selectedParentNode->getName()))
									removeNodeSelection(selectedParentNode->getName());
								else
									addNodeSelection(selectedParentNode->getName());
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
			mHeadNodeName = headNode->getName();
		}
		if (auto dummyNode = mDummyNode.lock())
		{
			toggleUserNodePoses(userNode);
			toggleUserNodePoses(dummyNode);
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
			OIS::Mouse* mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
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
	if (e.key == OIS::KeyCode::KC_SPACE)
	{
		mGeneralSpeedFactor = 2;
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
		else if (e.key == OIS::KeyCode::KC_C)
		{
			if (auto userNode = mUserNode.lock())
			{
				saveUserNodePose();
			}
		}
		else if (e.key == OIS::KeyCode::KC_V)
		{
			if (auto userNode = mUserNode.lock())
			{
				if (auto dummyNode = mDummyNode.lock())
				{
					toggleUserNodePoses(userNode);
					toggleUserNodePoses(dummyNode);
				}
			}
		}

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
	if (e.key == OIS::KeyCode::KC_SPACE)
	{
		mGeneralSpeedFactor = 2;
	}
	return true;
}

bool Ape::OISUserInputPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	mMouseState.posCurrent = e.state;
	mMouseState.isMouseMoved = true;
	if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Left])
	{
		for (auto nodeIt = mSelectedNodes.begin(); nodeIt != mSelectedNodes.end(); nodeIt++)
		{
			if (auto selectedNodeInMap = nodeIt->second.lock())
			{
				if (mKeyCodeMap[OIS::KeyCode::KC_LSHIFT] || mKeyCodeMap[OIS::KeyCode::KC_RSHIFT])
				{
					LOG_TRACE("X velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragModeLeft = true;
					selectedNodeInMap->translate(Ape::Vector3((mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs), 0, 0), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] || mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
				{
					LOG_TRACE("Y velocity: " << mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs);
					mMouseState.isDragModeLeft = true;
					selectedNodeInMap->translate(Ape::Vector3(0, -(mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs), 0), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_LMENU] || mKeyCodeMap[OIS::KeyCode::KC_RMENU])
				{
					LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragModeLeft = true;
					selectedNodeInMap->translate(Ape::Vector3(0, 0, -(mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs)), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
				{
					LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragModeLeft = true;
					selectedNodeInMap->rotate(Ape::Degree(mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs).toRadian(), Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
				}
			}
		}
	}
	if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Middle])
	{
		mMouseState.isDragModeMiddle = true;
	}
	if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Right])
	{
		mMouseState.isDragModeRight = true;
	}
	mMouseState.scrollVelocity = mMouseState.posCurrent.Z.abs - mMouseState.posPrevious.Z.abs;
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
	LOG_TRACE("mMouseState.isDragModeLeft: " << mMouseState.isDragModeLeft);
	LOG_TRACE("mMouseState.isDragModeMiddle: " << mMouseState.isDragModeMiddle);
	LOG_TRACE("mMouseState.isDragModeRight: " << mMouseState.isDragModeRight);
	mMouseState.buttonDownMap[id] = false;
	mMouseState.posEnd = e.state;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			overlayBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, false);
			LOG_TRACE("overlayBrowser->mouseClick");
		}

		if (!mMouseState.isDragModeLeft)
		{
			/*if (!mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] && !mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
			{
				clearNodeSelection();
			}*/

			if (auto rayOverlayNode = mRayOverlayNode.lock())
			{
				LOG_TRACE("rayGeomtery->fireIntersectionQuery x: " << e.state.X.abs << " y: " << e.state.Y.abs);
				rayOverlayNode->setPosition(Ape::Vector3(e.state.X.abs, e.state.Y.abs, 0));
				if (auto rayGeomtery = mRayGeometry.lock())
					rayGeomtery->fireIntersectionQuery();
			}
		}
		mMouseState.isDragModeLeft = false;
	}
	else if (id == OIS::MouseButtonID::MB_Middle)
	{
		mMouseState.isDragModeMiddle = false;
	}
	else if (id == OIS::MouseButtonID::MB_Right)
	{
		mMouseState.isDragModeRight = false;
	}
	return true;
}

bool Ape::OISUserInputPlugin::isNodeSelected(std::string nodeName)
{
	LOG_TRACE("nodeName: " << nodeName);
	std::map<std::string, Ape::NodeWeakPtr>::iterator findIt;
	findIt = mSelectedNodes.find(nodeName);
	return (findIt != mSelectedNodes.end());
}

void Ape::OISUserInputPlugin::addNodeSelection(std::string nodeName)
{
	if (auto nodeSharedPtr = mpScene->getNode(nodeName).lock())
	{
		Ape::NodeWeakPtrVector childNodes = nodeSharedPtr->getChildNodes();
		LOG_DEBUG("childNodes size: " << childNodes.size());
		for (auto childNode : childNodes)
			if (auto childNodeSP = childNode.lock())
				LOG_DEBUG("childNode: " << childNodeSP->getName());
		mSelectedNodes.insert(std::pair<std::string, Ape::NodeWeakPtr>(nodeSharedPtr->getName(), nodeSharedPtr));
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
	LOG(LOG_TYPE_DEBUG, "Camera position and orientation are toggled: " << userNode->getPosition().toString() << " | " << userNode->getOrientation().toString());
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
				if (!dummyNode->getOrientation().equals(headNode->getDerivedOrientation(), Ape::Radian(0.0f)))
				{
					dummyNode->setOrientation(headNode->getDerivedOrientation());
				}
			}

			if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
			{
				mGeneralSpeedFactor += 3;
			}
			int transScalar = mTranslateSpeedFactorKeyboard + mGeneralSpeedFactor;
			if (mKeyCodeMap[OIS::KeyCode::KC_PGUP])
			{
				dummyNode->translate(Ape::Vector3(0, +transScalar, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_PGDOWN])
			{
				dummyNode->translate(Ape::Vector3(0, -transScalar, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_D])
			{
				dummyNode->translate(Ape::Vector3(+transScalar, 0, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_A])
			{
				dummyNode->translate(Ape::Vector3(-transScalar, 0, 0), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_W])
			{
				dummyNode->translate(Ape::Vector3(0, 0, -transScalar), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_S])
			{
				dummyNode->translate(Ape::Vector3(0, 0, +transScalar), Ape::Node::TransformationSpace::LOCAL);
				userNode->setPosition(dummyNode->getPosition());
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_LEFT])
			{
				userNode->rotate(0.017f * mRotateSpeedFactorKeyboard, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_RIGHT])
			{
				userNode->rotate(-0.017f * mRotateSpeedFactorKeyboard, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_UP])
			{
				userNode->rotate(0.017f * mRotateSpeedFactorKeyboard, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
			}
			if (mKeyCodeMap[OIS::KeyCode::KC_DOWN])
			{
				userNode->rotate(-0.017f * mRotateSpeedFactorKeyboard, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
			}
		}
	}
}

void Ape::OISUserInputPlugin::moveUserNodeByMouse()
{
	if (auto userNode = mUserNode.lock())
	{
		if (auto dummyNode = mDummyNode.lock())
		{
			if (auto headNode = mHeadNode.lock())
			{
				if (!dummyNode->getOrientation().equals(headNode->getDerivedOrientation(), Ape::Radian(0.0f)))
				{
					dummyNode->setOrientation(headNode->getDerivedOrientation());
				}
			}

			if (mMouseState.isMouseMoved)
			{
				if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Right] && mMouseState.isDragModeRight)
				{
					userNode->rotate(Ape::Degree(-mMouseState.posCurrent.Y.rel).toRadian() * mRotateSpeedFactorMouse, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
					userNode->rotate(Ape::Degree(-mMouseState.posCurrent.X.rel).toRadian() * mRotateSpeedFactorMouse, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
				}
				if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Middle] && mMouseState.isDragModeMiddle)
				{
					dummyNode->translate(Ape::Vector3(1, 0, 0) * -(mMouseState.posCurrent.X.rel * mTranslateSpeedFactorMouse), Ape::Node::TransformationSpace::LOCAL);
					dummyNode->translate(Ape::Vector3(0, 1, 0) * +(mMouseState.posCurrent.Y.rel * mTranslateSpeedFactorMouse), Ape::Node::TransformationSpace::LOCAL);
					userNode->setPosition(dummyNode->getPosition());
				}
				if (mMouseState.scrollVelocity != 0)
				{
					LOG_TRACE("z: " << mMouseState.scrollVelocity);
					int transScalar = (mMouseState.scrollVelocity / 3) * mTranslateSpeedFactorMouse;
					if (transScalar < 0)
						transScalar -= mGeneralSpeedFactor;
					if (transScalar > 0)
						transScalar += mGeneralSpeedFactor;
					dummyNode->translate(Ape::Vector3(0, 0, -transScalar), Ape::Node::TransformationSpace::LOCAL);
					userNode->setPosition(dummyNode->getPosition());
				}
				mMouseState.isMouseMoved = false;
			}
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
