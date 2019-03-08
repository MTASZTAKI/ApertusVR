#include <fstream>
#include "ApeOisUserInputPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

Ape::OISUserInputPlugin::OISUserInputPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpKeyboard = NULL;
	mpMouse = NULL;
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::BROWSER, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
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
	mCursorNode = Ape::NodeWeakPtr();
	mCursorText = Ape::TextGeometryWeakPtr();
	mIsNewKeyEvent = false;
	mEnableOverlayBrowserKeyEvents = false;
	mUserNodeTogglePoses = std::vector<UserNodePose>();
	mUserNodeAnimatePoses = std::vector<UserNodePose>();
	mUserNodePosesToggleIndex = 0;
	mIsKeyPressed = false;
	mIsUserNodeAnimated = false;
	mpApeUserInputMacro = new UserInputMacro();
	fillUserNodeAnimatePoses();
	APE_LOG_FUNC_LEAVE();
}

Ape::OISUserInputPlugin::~OISUserInputPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::BROWSER, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::TEXTURE_UNIT, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	delete mpKeyboard;
	delete mpMouse;
	APE_LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::NODE_CREATE)
	{
		if (event.subjectName == "robotRootNode")
		{
			//mNodeToMove = mpSceneManager->getNode(event.subjectName);
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
		mOverlayBrowser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock());
		APE_LOG_DEBUG("overlayBrowser catched");
	}
	else if (event.type == Ape::Event::Type::TEXTURE_UNIT_CREATE)
	{
		mOverlayMouseTexture = std::static_pointer_cast<Ape::IUnitTexture>(mpSceneManager->getEntity(event.subjectName).lock());
		APE_LOG_DEBUG("overlayMouseTexture catched");
	}
	else if (event.type == Ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD)
	{
		APE_LOG_TRACE("BROWSER_FOCUS_ON_EDITABLE_FIELD");
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			if (auto focusChangedBrowser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				mEnableOverlayBrowserKeyEvents = focusChangedBrowser->isFocusOnEditableField() && overlayBrowser->getName() == focusChangedBrowser->getName();
				APE_LOG_TRACE("mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
				mIsNewKeyEvent = true;
			}
		}
	}
	else if (event.type == Ape::Event::Type::BROWSER_MOUSE_CLICK)
	{
		/*APE_LOG_DEBUG("BROWSER_MOUSE_CLICK");
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			APE_LOG_DEBUG("BROWSER_MOUSE_CLICK isFocusOnEditableField: " << overlayBrowser->isFocusOnEditableField());
		}*/
	}
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_CREATE)
	{
		APE_LOG_DEBUG("GEOMETRY_RAY_CREATE");
	}
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_INTERSECTION)
	{
		APE_LOG_TRACE("GEOMETRY_RAY_INTERSECTION");
		if (auto rayGeometry = mRayGeometry.lock())
		{
			auto intersections = rayGeometry->getIntersections();
			std::list<Ape::EntityWeakPtr> intersectionList;
			std::copy(intersections.begin(), intersections.end(), std::back_inserter(intersectionList));
			APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: intersections.size: " << intersectionList.size());
			bool removeItem = false;
			std::list<Ape::EntityWeakPtr>::iterator i = intersectionList.begin();
			while (i != intersectionList.end())
			{
				removeItem = false;
				if (auto entity = i->lock())
				{
					std::string entityName = entity->getName();
					Ape::Entity::Type entityType = entity->getType();
					//APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: entityName: " << entityName << " entityType: " << entityType);
					/*if (entityName.find(mUserNode.lock()->getName()) != std::string::npos)
						removeItem = true;
					else if (entityName.find("coord") != std::string::npos)
						removeItem = true;
					else if (entityName.find("cursor") != std::string::npos)
						removeItem = true;*/
				}
				if (removeItem)
					i = intersectionList.erase(i);
				else
					i++;
			}
			APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: intersections.size after erase: " << intersectionList.size());

			bool intersectionHandled = false;
			if (intersectionList.empty())
			{
				if (mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] || mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
				{
					clearNodeSelection();
					intersectionHandled = true;
				}
			}
			else
			{
				APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: -------------------------------------");
				for (auto intersection : intersectionList)
				{
					if (auto entity = intersection.lock())
					{
						std::string entityName = entity->getName();
						Ape::Entity::Type entityType = entity->getType();
						APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: entityName: " << entityName << " entityType: " << entityType);

						if (entityType >= Ape::Entity::Type::GEOMETRY_FILE && entityType <= Ape::Entity::Type::GEOMETRY_RAY)
						{
							auto geometry = std::static_pointer_cast<Ape::Geometry>(entity);
							if (auto selectedParentNode = geometry->getParentNode().lock())
							{
								APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: parentNode: " << selectedParentNode->getName());
								if (auto cursorText = mCursorText.lock())
								{
									cursorText->setCaption(entityName);
								}
								if (mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] || mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
								{
									if (isNodeSelected(selectedParentNode->getName()))
										removeNodeSelection(selectedParentNode->getName());
									else
										addNodeSelection(selectedParentNode->getName());
									intersectionHandled = true;
									break;
								}
							}
						}
						else if (entityType == Ape::Entity::Type::POINT_CLOUD)
						{
							auto pointCloud = std::static_pointer_cast<Ape::IPointCloud>(entity);
							if (auto selectedParentNode = pointCloud->getParentNode().lock())
							{
								APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: parentNode: " << selectedParentNode->getName());
								if (mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] || mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
								{
									if (isNodeSelected(selectedParentNode->getName()))
										removeNodeSelection(selectedParentNode->getName());
									else
										addNodeSelection(selectedParentNode->getName());
									intersectionHandled = true;
									break;
								}
							}
						}
					}
				}
			}
			APE_LOG_DEBUG("GEOMETRY_RAY_INTERSECTION: intersections handled: " << intersectionHandled);
		}
	}
}

void Ape::OISUserInputPlugin::animateUserNode(Ape::NodeSharedPtr userNode)
{
	while (mIsUserNodeAnimated)
	{
		for (auto userNodePose : mUserNodeAnimatePoses)
		{
			auto moveInterpolator = std::make_unique<Ape::Interpolator>(false);
			moveInterpolator->addSection(
				userNode->getPosition(),
				userNodePose.position,
				4.0,
				[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
			);
			auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
			rotateInterpolator->addSection(
				userNode->getOrientation(),
				userNodePose.orientation,
				4.0,
				[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
			);
			while (!moveInterpolator->isQueueEmpty() && !rotateInterpolator->isQueueEmpty() && mIsUserNodeAnimated)
			{
				if (!moveInterpolator->isQueueEmpty())
					moveInterpolator->iterateTopSection();
				if (!rotateInterpolator->isQueueEmpty())
					rotateInterpolator->iterateTopSection();
			}
		}
	}
}

void Ape::OISUserInputPlugin::fillUserNodeAnimatePoses()
{
	mUserNodeAnimatePoses.push_back(UserNodePose(Ape::Vector3(459.301, 205.316, -75.8723), Ape::Quaternion(-0.593951, 0.0556969, -0.799066, -0.0749312)));
	mUserNodeAnimatePoses.push_back(UserNodePose(Ape::Vector3(-106.345, 235.75, -468.079), Ape::Quaternion(0.0745079, -0.00571105, -0.994289, -0.0762119)));
	mUserNodeAnimatePoses.push_back(UserNodePose(Ape::Vector3(-533.586, 232.092, -66.402), Ape::Quaternion(0.543329, -0.041646, -0.836036, -0.0640818)));
	mUserNodeAnimatePoses.push_back(UserNodePose(Ape::Vector3(-106.345, 235.75, -468.079), Ape::Quaternion(0.0745079, -0.00571105, -0.994289, -0.0762119)));
	mUserNodeAnimatePoses.push_back(UserNodePose(Ape::Vector3(459.301, 205.316, -75.8723), Ape::Quaternion(-0.593951, 0.0556969, -0.799066, -0.0749312)));
}

void Ape::OISUserInputPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

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
			if (jsonDocument.HasMember("inputs"))
			{
				rapidjson::Value& inputs = jsonDocument["inputs"];
				if (inputs.IsArray())
				{
					for (auto& input : inputs.GetArray())
					{
						oisWindowConfig.inputs.push_back(input.GetString());
					}
				}
			}
			if (jsonDocument.HasMember("userPoses"))
			{
				rapidjson::Value& userPoses = jsonDocument["userPoses"];
				if (userPoses.IsArray())
				{
					for (auto& userPose : userPoses.GetArray())
					{
						Ape::Vector3 position(userPose[0].GetFloat(), userPose[1].GetFloat(), userPose[2].GetFloat());
						Ape::Quaternion orientation(userPose[3].GetFloat(), userPose[4].GetFloat(), userPose[5].GetFloat(), userPose[6].GetFloat());
						mUserNodeTogglePoses.push_back(UserNodePose(position, orientation));
					}
				}
			}
			if (jsonDocument.HasMember("cameraNames"))
			{
				rapidjson::Value& cameraNames = jsonDocument["cameraNames"];
				if (cameraNames.IsArray())
				{
					for (auto& cameraName : cameraNames.GetArray())
					{
						mpApeUserInputMacro->createCamera(cameraName.GetString());
					}
				}
			}
		}
		fclose(apeOisUserInputConfigFile);
	}

	/*if (auto cursorNode = mpSceneManager->createNode("cursorNode").lock())
	{
		if (auto cursorGeometry = std::static_pointer_cast<Ape::ITorusGeometry>(mpSceneManager->createEntity("cursor", Ape::Entity::GEOMETRY_TORUS).lock()))
		{
			cursorGeometry->setParameters(0.1f, 0.05f, Ape::Vector2(100, 100));
			cursorGeometry->setParentNode(cursorNode);
			cursorNode->setParentNode(userNode);
			cursorNode->setPosition(Ape::Vector3(0, 0, -10));
			cursorNode->setOrientation(Ape::Quaternion(0.5, -0.5, -0.5, 0.5));
			mCursorNode = cursorNode;
		}

		if (auto textNode = mpSceneManager->createNode("cursorTextNode").lock())
		{
			if (auto cursorText = std::static_pointer_cast<Ape::ITextGeometry>(mpSceneManager->createEntity("cursorText", Ape::Entity::GEOMETRY_TEXT).lock()))
			{
				cursorText->setParentNode(textNode);
				cursorText->setCaption("hello");
				cursorText->showOnTop(true);
				mCursorText = cursorText;
			}
			textNode->setParentNode(userNode);
			textNode->setPosition(Ape::Vector3(7, -1, -100));
		}
	}*/

	APE_LOG_DEBUG("OISUserInputPlugin waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("OisUserInputPlugin main window was found");

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

	if (auto rayNode = mpSceneManager->createNode("rayNode" + mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
	{
		if (auto rayGeometry = std::static_pointer_cast<Ape::IRayGeometry>(mpSceneManager->createEntity("rayQuery" + mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName, Ape::Entity::GEOMETRY_RAY).lock()))
		{
			rayGeometry->setIntersectingEnabled(true);
			rayGeometry->setParentNode(rayNode);
			mRayGeometry = rayGeometry;
		}
		//rayNode->setParentNode(mUserNode);
		mRayOverlayNode = rayNode;
	}

	APE_LOG_FUNC_LEAVE();
}

bool Ape::OISUserInputPlugin::keyPressed(const OIS::KeyEvent& e)
{
	APE_LOG_TRACE("-------------------------------------------------------");
	APE_LOG_TRACE("mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
	APE_LOG_TRACE("OIS::KeyCode: " << (OIS::KeyCode)e.key);

	mKeyCodeMap[e.key] = true;

	if (e.key == OIS::KeyCode::KC_C)
	{
		saveUserNodePose();
	}
	if (e.key == OIS::KeyCode::KC_V)
	{
		APE_LOG_DEBUG("Key pressed V");
		//toggleUserNodePoses();
	}
	if (e.key == OIS::KeyCode::KC_B)
	{
		/*if (auto userNode = mUserNode.lock())
		{
			auto moveInterpolator = std::make_unique<Ape::Interpolator>(false);
			moveInterpolator->addSection(
				userNode->getPosition(),
				Ape::Vector3(459.301, 205.316, -75.8723),
				5.0,
				[&](Ape::Vector3 pos) { userNode->setPosition(pos); }
			);
			auto rotateInterpolator = std::make_unique<Ape::Interpolator>(false);
			rotateInterpolator->addSection(
				userNode->getOrientation(),
				Ape::Quaternion(-0.593951, 0.0556969, -0.799066, -0.0749312),
				5.0,
				[&](Ape::Quaternion ori) { userNode->setOrientation(ori); }
			);
			while (!moveInterpolator->isQueueEmpty() && !rotateInterpolator->isQueueEmpty())
			{
				if (!moveInterpolator->isQueueEmpty())
					moveInterpolator->iterateTopSection();
				if (!rotateInterpolator->isQueueEmpty())
					rotateInterpolator->iterateTopSection();
			}
		}*/
	}
	if (e.key == OIS::KeyCode::KC_SPACE)
	{
		mGeneralSpeedFactor = 2;
	}
	else if (e.key == OIS::KeyCode::KC_P)
	{
		/*auto userNode = mUserNode.lock();
		if (userNode && !mIsUserNodeAnimated)
		{
			mIsUserNodeAnimated = true;
			std::thread animateThread((std::bind(&Ape::OISUserInputPlugin::animateUserNode, this, userNode)));
			animateThread.detach();
		}
		else
		{
			mIsUserNodeAnimated = false;
			if (auto userNode = mUserNode.lock())
			{
				if (auto dummyNode = mDummyNode.lock())
				{
					dummyNode->setPosition(userNode->getPosition());
				}
			}
		}*/
	}

	if (auto overlayBrowser = mOverlayBrowser.lock())
	{
		std::string keyAsString = mpKeyboard->getAsString(e.key);

		if (!mKeyCodeMap[OIS::KeyCode::KC_LSHIFT] && !mKeyCodeMap[OIS::KeyCode::KC_RSHIFT])
			std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);

		APE_LOG_TRACE("keyAsString:" << keyAsString);
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

		APE_LOG_TRACE("Before waiting: mIsNewKeyEvent: " << mIsNewKeyEvent);
		while (!mIsNewKeyEvent)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		mIsNewKeyEvent = false;
		APE_LOG_TRACE("After waiting mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
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
					APE_LOG_TRACE("X velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragModeLeft = true;
					selectedNodeInMap->translate(Ape::Vector3((mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs), 0, 0), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] || mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
				{
					APE_LOG_TRACE("Y velocity: " << mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs);
					mMouseState.isDragModeLeft = true;
					selectedNodeInMap->translate(Ape::Vector3(0, -(mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs), 0), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_LMENU] || mKeyCodeMap[OIS::KeyCode::KC_RMENU])
				{
					APE_LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
					mMouseState.isDragModeLeft = true;
					selectedNodeInMap->translate(Ape::Vector3(0, 0, -(mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs)), Ape::Node::TransformationSpace::WORLD);
				}
				if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
				{
					APE_LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
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
			//APE_LOG_DEBUG("cursorTexturePosition:" << cursorTexturePosition.x << ";" << cursorTexturePosition.y);
			//APE_LOG_DEBUG("cursorBrowserPosition:" << cursorBrowserPosition.x << ";" << cursorBrowserPosition.y);
		}
	}

	return true;
}

bool Ape::OISUserInputPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	APE_LOG_TRACE("-------------------------------------------------------");
	mMouseState.buttonDownMap[id] = true;
	mMouseState.posStart = e.state;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			APE_LOG_TRACE("overlayBrowser->mouseClick");
			overlayBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, true);
		}
	}
	return true;
}

bool Ape::OISUserInputPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	APE_LOG_TRACE("mMouseState.isDragModeLeft: " << mMouseState.isDragModeLeft);
	APE_LOG_TRACE("mMouseState.isDragModeMiddle: " << mMouseState.isDragModeMiddle);
	APE_LOG_TRACE("mMouseState.isDragModeRight: " << mMouseState.isDragModeRight);
	mMouseState.buttonDownMap[id] = false;
	mMouseState.posEnd = e.state;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		if (auto overlayBrowser = mOverlayBrowser.lock())
		{
			overlayBrowser->mouseClick(Ape::Browser::MouseClick::LEFT, false);
			APE_LOG_TRACE("overlayBrowser->mouseClick");
		}

		if (!mMouseState.isDragModeLeft)
		{
			if (auto rayOverlayNode = mRayOverlayNode.lock())
			{
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
	APE_LOG_TRACE("nodeName: " << nodeName);
	std::map<std::string, Ape::NodeWeakPtr>::iterator findIt;
	findIt = mSelectedNodes.find(nodeName);
	return (findIt != mSelectedNodes.end());
}

void Ape::OISUserInputPlugin::addNodeSelection(std::string nodeName)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_TRACE("nodeName: " << nodeName);
	if (auto findNode = mpSceneManager->getNode(nodeName).lock())
	{
		Ape::NodeWeakPtrVector childNodes = findNode->getChildNodes();
		APE_LOG_DEBUG("childNodes size: " << childNodes.size());
		for (auto childNode : childNodes)
			if (auto childNodeSP = childNode.lock())
				APE_LOG_DEBUG("childNode: " << childNodeSP->getName());
		mSelectedNodes.insert(std::pair<std::string, Ape::NodeWeakPtr>(findNode->getName(), findNode));
		findNode->showBoundingBox(true);
	}
	APE_LOG_FUNC_LEAVE();
}

bool Ape::OISUserInputPlugin::removeNodeSelection(std::string nodeName)
{
	APE_LOG_TRACE("nodeName: " << nodeName);
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
	APE_LOG_FUNC_ENTER();
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
	APE_LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::saveUserNodePose()
{
	APE_LOG_FUNC_ENTER();
	/*if (auto userNode = mUserNode.lock())
	{
		std::ofstream userNodePoseFile;
		userNodePoseFile.open("userNodePoseFile.txt", std::ios::app);
		userNodePoseFile << "[ " << userNode->getPosition().x << ", " << userNode->getPosition().y << ", " << userNode->getPosition().z << ", " <<
			userNode->getOrientation().w << ", " << userNode->getOrientation().x << ", " << userNode->getOrientation().y << ", " << userNode->getOrientation().z
			<< " ]," << std::endl;
		userNodePoseFile.close();
	}*/
	APE_LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::toggleUserNodePoses(Ape::NodeSharedPtr userNode)
{
	if (mUserNodeTogglePoses.size() > 0 && mUserNodePosesToggleIndex < mUserNodeTogglePoses.size())
	{
		userNode->setPosition(mUserNodeTogglePoses[mUserNodePosesToggleIndex].position);
		userNode->setOrientation(mUserNodeTogglePoses[mUserNodePosesToggleIndex].orientation);
		APE_LOG_DEBUG("Camera position and orientation are toggled: " << userNode->getPosition().toString() << " | " << userNode->getOrientation().toString());
		mUserNodePosesToggleIndex++;
		if (mUserNodeTogglePoses.size() == mUserNodePosesToggleIndex)
			mUserNodePosesToggleIndex = 0;
	}
}

void Ape::OISUserInputPlugin::moveUserNodeByKeyBoard()
{
	if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
	{
		mGeneralSpeedFactor += 3;
	}
	int transScalar = mTranslateSpeedFactorKeyboard + mGeneralSpeedFactor;
	if (mKeyCodeMap[OIS::KeyCode::KC_PGUP])
	{
		mUserInputMacroPose.userTranslate = Ape::Vector3(0, +transScalar, 0);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_PGDOWN])
	{
		mUserInputMacroPose.userTranslate = Ape::Vector3(0, -transScalar, 0);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_D])
	{
		mUserInputMacroPose.userTranslate = Ape::Vector3(+transScalar, 0, 0);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_A])
	{
		mUserInputMacroPose.userTranslate = Ape::Vector3(-transScalar, 0, 0);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_W])
	{
		mUserInputMacroPose.userTranslate = Ape::Vector3(0, 0, -transScalar);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_S])
	{
		mUserInputMacroPose.userTranslate = Ape::Vector3(0, 0, +transScalar);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_LEFT])
	{
		mUserInputMacroPose.userRotateAngle = 0.017f * mRotateSpeedFactorKeyboard;
		mUserInputMacroPose.userRotateAxis = Ape::Vector3(0, 1, 0);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_RIGHT])
	{
		mUserInputMacroPose.userRotateAngle = -0.017f * mRotateSpeedFactorKeyboard;
		mUserInputMacroPose.userRotateAxis = Ape::Vector3(0, 1, 0);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_UP])
	{
		mUserInputMacroPose.userRotateAngle = 0.017f * mRotateSpeedFactorKeyboard;
		mUserInputMacroPose.userRotateAxis = Ape::Vector3(1, 0, 0);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_DOWN])
	{
		mUserInputMacroPose.userRotateAngle = -0.017f * mRotateSpeedFactorKeyboard;
		mUserInputMacroPose.userRotateAxis = Ape::Vector3(1, 0, 0);
	}
	mpApeUserInputMacro->updatePose(mUserInputMacroPose);
}

void Ape::OISUserInputPlugin::moveUserNodeByMouse()
{
	if (mMouseState.isMouseMoved)
	{
		if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Right] && mMouseState.isDragModeRight)
		{
			//userNode->rotate(Ape::Degree(-mMouseState.posCurrent.Y.rel).toRadian() * mRotateSpeedFactorMouse, Ape::Vector3(1, 0, 0), Ape::Node::TransformationSpace::LOCAL);
			//userNode->rotate(Ape::Degree(-mMouseState.posCurrent.X.rel).toRadian() * mRotateSpeedFactorMouse, Ape::Vector3(0, 1, 0), Ape::Node::TransformationSpace::WORLD);
		}
		if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Middle] && mMouseState.isDragModeMiddle)
		{
			/*dummyNode->translate(Ape::Vector3(1, 0, 0) * -(mMouseState.posCurrent.X.rel * mTranslateSpeedFactorMouse), Ape::Node::TransformationSpace::LOCAL);
			dummyNode->translate(Ape::Vector3(0, 1, 0) * +(mMouseState.posCurrent.Y.rel * mTranslateSpeedFactorMouse), Ape::Node::TransformationSpace::LOCAL);
			userNode->setPosition(dummyNode->getPosition());*/
		}
		if (mMouseState.scrollVelocity != 0)
		{
			APE_LOG_TRACE("z: " << mMouseState.scrollVelocity);
			int transScalar = (mMouseState.scrollVelocity / 3) * mTranslateSpeedFactorMouse;
			if (transScalar < 0)
				transScalar -= mGeneralSpeedFactor;
			if (transScalar > 0)
				transScalar += mGeneralSpeedFactor;
			/*dummyNode->translate(Ape::Vector3(0, 0, -transScalar), Ape::Node::TransformationSpace::LOCAL);
			userNode->setPosition(dummyNode->getPosition());*/
		}
		mMouseState.isMouseMoved = false;
	}
}

void Ape::OISUserInputPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
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
	APE_LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::OISUserInputPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
