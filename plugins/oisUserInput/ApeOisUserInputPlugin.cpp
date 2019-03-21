#include <fstream>
#include "ApeOisUserInputPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::OISUserInputPlugin::OISUserInputPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpKeyboard = NULL;
	mpMouse = NULL;
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	mTranslateSpeedFactorKeyboard = 3;
	mRotateSpeedFactorKeyboard = 1;
	mTranslateSpeedFactorMouse = 1.2;
	mRotateSpeedFactorMouse = 0.2;
	mGeneralSpeedFactor = 0;
	mViewPoses = std::vector<ape::UserInputMacro::ViewPose>();
	mViewPosesToggleIndex = 0;
	mIsKeyPressed = false;
	mpApeUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	mUserInputMacroPose = ape::UserInputMacro::ViewPose();
	mOverlayBrowserCursor = ape::UserInputMacro::OverlayBrowserCursor();
	APE_LOG_FUNC_LEAVE();
}

ape::OISUserInputPlugin::~OISUserInputPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::BROWSER, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	delete mpKeyboard;
	delete mpMouse;
	APE_LOG_FUNC_LEAVE();
}

void ape::OISUserInputPlugin::eventCallBack(const ape::Event& event)
{
}

void ape::OISUserInputPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	ape::OisWindowConfig oisWindowConfig;
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getConfigFolderPath() << "\\ApeOisUserInputPlugin.json";
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
						ape::Vector3 position(userPose[0].GetFloat(), userPose[1].GetFloat(), userPose[2].GetFloat());
						ape::Quaternion orientation(userPose[3].GetFloat(), userPose[4].GetFloat(), userPose[5].GetFloat(), userPose[6].GetFloat());
						ape::UserInputMacro::ViewPose userInputMacroPose;
						userInputMacroPose.userPosition = position;
						userInputMacroPose.userOrientation = orientation;
						mViewPoses.push_back(userInputMacroPose);
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
	APE_LOG_DEBUG("OISUserInputPlugin waiting for main window");
	while (mpSystemConfig->getWindowConfig().handle == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("OisUserInputPlugin main window was found");
	std::stringstream hwndStrStream;
	hwndStrStream << mpSystemConfig->getWindowConfig().handle;
	std::stringstream windowHndStr;
	windowHndStr << std::stoul(hwndStrStream.str(), nullptr, 16);
	oisWindowConfig.handler = windowHndStr.str();
	oisWindowConfig.width = mpSystemConfig->getWindowConfig().width;
	oisWindowConfig.height = mpSystemConfig->getWindowConfig().height;
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
	APE_LOG_FUNC_LEAVE();
}

bool ape::OISUserInputPlugin::keyPressed(const OIS::KeyEvent& e)
{
	APE_LOG_TRACE("OIS::KeyCode: " << (OIS::KeyCode)e.key);
	mKeyCodeMap[e.key] = true;
	if (e.key == OIS::KeyCode::KC_C)
	{
		mpApeUserInputMacro->saveViewPose();
	}
	if (e.key == OIS::KeyCode::KC_T)
	{
		APE_LOG_DEBUG("Key pressed T");
		toggleViewPoses(false);
	}
	if (e.key == OIS::KeyCode::KC_I)
	{
		toggleViewPoses(true);
	}
	if (e.key == OIS::KeyCode::KC_SPACE)
	{
		mGeneralSpeedFactor = 2;
	}
	std::string keyAsString = mpKeyboard->getAsString(e.key);
	//if (!mKeyCodeMap[OIS::KeyCode::KC_LSHIFT] && !mKeyCodeMap[OIS::KeyCode::KC_RSHIFT])
	std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);
	APE_LOG_TRACE("keyAsString:" << keyAsString);
	mpApeUserInputMacro->keyStringValue(keyAsString);
	//TODO_OISUserInputPlugin
	//std::wstring keyAsWString(keyAsString.begin(), keyAsString.end());
	//if (e.key == OIS::KeyCode::KC_BACK)
	//	keyAsWString = 8;
	//else if (e.key == OIS::KeyCode::KC_TAB)
	//	keyAsWString = 9;
	//else if (e.key == OIS::KeyCode::KC_RETURN)
	//	keyAsWString = 13;
	//else if (e.key == OIS::KeyCode::KC_LSHIFT || e.key == OIS::KeyCode::KC_RSHIFT)
	//	keyAsWString = 14;
	//else if (e.key == OIS::KeyCode::KC_SPACE)
	//	keyAsWString = 32;
	//else if (e.key == OIS::KeyCode::KC_END)
	//	keyAsWString = 35;
	//else if (e.key == OIS::KeyCode::KC_HOME)
	//	keyAsWString = 36;
	//else if (e.key == OIS::KeyCode::KC_LEFT)
	//	keyAsWString = 37;
	//else if (e.key == OIS::KeyCode::KC_UP)
	//	keyAsWString = 38;
	//else if (e.key == OIS::KeyCode::KC_RIGHT)
	//	keyAsWString = 39;
	//else if (e.key == OIS::KeyCode::KC_DOWN)
	//	keyAsWString = 40;
	//else if (e.key == OIS::KeyCode::KC_DELETE)
	//	keyAsWString = 46;
	//else if (e.key == OIS::KeyCode::KC_PERIOD)
	//	keyAsWString = 1046;

	//	mIsNewKeyEvent = false;
	//	overlayBrowser->keyASCIIValue(keyAsWString[0]);

	//	APE_LOG_TRACE("Before waiting: mIsNewKeyEvent: " << mIsNewKeyEvent);
	//	while (!mIsNewKeyEvent)
	//	{
	//		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	//	}
	//	mIsNewKeyEvent = false;
	//	APE_LOG_TRACE("After waiting mEnableOverlayBrowserKeyEvents: " << mEnableOverlayBrowserKeyEvents);
	//}
	return true;
}

bool ape::OISUserInputPlugin::keyReleased(const OIS::KeyEvent& e)
{
	mKeyCodeMap[e.key] = false;
	if (e.key == OIS::KeyCode::KC_SPACE)
	{
		mGeneralSpeedFactor = 2;
	}
	return true;
}

bool ape::OISUserInputPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	mMouseState.posCurrent = e.state;
	mMouseState.isMouseMoved = true;
	if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Left])
	{
		if (mKeyCodeMap[OIS::KeyCode::KC_LSHIFT] || mKeyCodeMap[OIS::KeyCode::KC_RSHIFT])
		{
			APE_LOG_TRACE("X velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
			mMouseState.isDragModeLeft = true;
			ape::UserInputMacro::Pose pose;
			pose.position += ape::Vector3((mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs), 0, 0);
			mpApeUserInputMacro->updateSelectedNodePose(pose);
		}
		if (mKeyCodeMap[OIS::KeyCode::KC_LCONTROL] || mKeyCodeMap[OIS::KeyCode::KC_RCONTROL])
		{
			APE_LOG_TRACE("Y velocity: " << mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs);
			mMouseState.isDragModeLeft = true;
			ape::UserInputMacro::Pose pose;
			pose.position += ape::Vector3(0, -(mMouseState.posCurrent.Y.abs - mMouseState.posPrevious.Y.abs), 0);
			mpApeUserInputMacro->updateSelectedNodePose(pose);
		}
		if (mKeyCodeMap[OIS::KeyCode::KC_LMENU] || mKeyCodeMap[OIS::KeyCode::KC_RMENU])
		{
			APE_LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
			mMouseState.isDragModeLeft = true;
			ape::UserInputMacro::Pose pose;
			pose.position += ape::Vector3(0, 0, -(mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs));
			mpApeUserInputMacro->updateSelectedNodePose(pose);
		}
		if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
		{
			APE_LOG_TRACE("Z velocity: " << mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs);
			mMouseState.isDragModeLeft = true;
			ape::UserInputMacro::Pose pose;
			ape::Quaternion qnorm;
			qnorm.FromAngleAxis(ape::Degree(mMouseState.posCurrent.X.abs - mMouseState.posPrevious.X.abs).toRadian(), ape::Vector3(0, 1, 0));
			qnorm.normalise();
			pose.orientation = pose.orientation * qnorm;
			mpApeUserInputMacro->updateSelectedNodePose(pose);
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
	ape::Vector2 cursorTexturePosition;
	cursorTexturePosition.x = (float)-e.state.X.abs / (float)mpSystemConfig->getWindowConfig().width;
	cursorTexturePosition.y = (float)-e.state.Y.abs / (float)mpSystemConfig->getWindowConfig().height;
	ape::Vector2 cursorBrowserPosition;
	cursorBrowserPosition.x = (float)e.state.X.abs / (float)mpSystemConfig->getWindowConfig().width;
	cursorBrowserPosition.y = (float)e.state.Y.abs / (float)mpSystemConfig->getWindowConfig().height;
	mOverlayBrowserCursor.cursorBrowserPosition = cursorBrowserPosition;
	mOverlayBrowserCursor.cursorTexturePosition = cursorTexturePosition;
	mOverlayBrowserCursor.cursorScrollPosition = ape::Vector2(0, e.state.Z.rel);
	mOverlayBrowserCursor.cursorClick = false;
	mpApeUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
	//APE_LOG_DEBUG("cursorTexturePosition:" << cursorTexturePosition.x << ";" << cursorTexturePosition.y);
	//APE_LOG_DEBUG("cursorBrowserPosition:" << cursorBrowserPosition.x << ";" << cursorBrowserPosition.y);
	return true;
}

bool ape::OISUserInputPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mMouseState.buttonDownMap[id] = true;
	mMouseState.posStart = e.state;
	if (id == OIS::MouseButtonID::MB_Left)
	{
		mOverlayBrowserCursor.cursorClick = true;
		mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
		mpApeUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);
	}
	return true;
}

bool ape::OISUserInputPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	APE_LOG_TRACE("mMouseState.isDragModeLeft: " << mMouseState.isDragModeLeft);
	APE_LOG_TRACE("mMouseState.isDragModeMiddle: " << mMouseState.isDragModeMiddle);
	APE_LOG_TRACE("mMouseState.isDragModeRight: " << mMouseState.isDragModeRight);
	mMouseState.buttonDownMap[id] = false;
	mMouseState.posEnd = e.state;

	if (id == OIS::MouseButtonID::MB_Left)
	{
		mOverlayBrowserCursor.cursorClick = false;
		mOverlayBrowserCursor.cursorClickType = ape::Browser::MouseClick::LEFT,
		mpApeUserInputMacro->updateOverLayBrowserCursor(mOverlayBrowserCursor);

		if (!mMouseState.isDragModeLeft)
		{
			mpApeUserInputMacro->rayQuery(ape::Vector3(e.state.X.abs, e.state.Y.abs, 0));
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

void ape::OISUserInputPlugin::toggleViewPoses(bool isInterpolated)
{
	if (mViewPoses.size() > 0 && mViewPosesToggleIndex < mViewPoses.size())
	{
		if (!isInterpolated)
		{
			mpApeUserInputMacro->updateViewPose(mViewPoses[mViewPosesToggleIndex]);
		}
		else
		{
			mpApeUserInputMacro->interpolateViewPose(mViewPoses[mViewPosesToggleIndex], 5000);
		}
		APE_LOG_DEBUG("View pose is toggled: " << mUserInputMacroPose.userPosition.toString() << " | " << mUserInputMacroPose.userOrientation.toString());
		mViewPosesToggleIndex++;
		if (mViewPoses.size() == mViewPosesToggleIndex)
			mViewPosesToggleIndex = 0;
	}
}

void ape::OISUserInputPlugin::updateViewPoseByKeyBoard()
{
	if (mKeyCodeMap[OIS::KeyCode::KC_SPACE])
	{
		mGeneralSpeedFactor += 3;
	}
	int transScalar = mTranslateSpeedFactorKeyboard + mGeneralSpeedFactor;
	if (mKeyCodeMap[OIS::KeyCode::KC_PGUP])
	{
		mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(0, +transScalar, 0);
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_PGDOWN])
	{
		mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(0, -transScalar, 0);
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_D])
	{
		mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(+transScalar, 0, 0);
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_A])
	{
		mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(-transScalar, 0, 0);
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_W])
	{
		mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(0, 0, -transScalar);
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_S])
	{
		mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(0, 0, +transScalar);
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_LEFT])
	{
		ape::Quaternion qnorm;
		qnorm.FromAngleAxis(ape::Radian(0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(0, 1, 0));
		qnorm.normalise();
		mUserInputMacroPose.userOrientation = mUserInputMacroPose.userOrientation * qnorm;
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_RIGHT])
	{
		ape::Quaternion qnorm;
		qnorm.FromAngleAxis(ape::Radian(-0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(0, 1, 0));
		qnorm.normalise();
		mUserInputMacroPose.userOrientation = mUserInputMacroPose.userOrientation * qnorm;
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_UP])
	{
		ape::Quaternion qnorm;
		qnorm.FromAngleAxis(ape::Radian(0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(1, 0, 0));
		qnorm.normalise();
		mUserInputMacroPose.userOrientation = mUserInputMacroPose.userOrientation * qnorm;
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
	if (mKeyCodeMap[OIS::KeyCode::KC_DOWN])
	{
		ape::Quaternion qnorm;
		qnorm.FromAngleAxis(ape::Radian(-0.017f * mRotateSpeedFactorKeyboard), ape::Vector3(1, 0, 0));
		qnorm.normalise();
		mUserInputMacroPose.userOrientation = mUserInputMacroPose.userOrientation * qnorm;
		mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
	}
}

void ape::OISUserInputPlugin::updateViewPoseByMouse()
{
	if (mMouseState.isMouseMoved)
	{
		if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Right] && mMouseState.isDragModeRight)
		{
			ape::Quaternion qnorm;
			qnorm.FromAngleAxis(ape::Degree(-mMouseState.posCurrent.Y.rel).toRadian() * mRotateSpeedFactorMouse, ape::Vector3(1, 0, 0));
			qnorm.normalise();
			mUserInputMacroPose.userOrientation = mUserInputMacroPose.userOrientation * qnorm;
			mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
			qnorm.FromAngleAxis(ape::Degree(-mMouseState.posCurrent.X.rel).toRadian() * mRotateSpeedFactorMouse, ape::Vector3(0, 1, 0));
			qnorm.normalise();
			mUserInputMacroPose.userOrientation = mUserInputMacroPose.userOrientation * qnorm;
			mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
		}
		if (mMouseState.buttonDownMap[OIS::MouseButtonID::MB_Middle] && mMouseState.isDragModeMiddle)
		{
			mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(1, 0, 0) * -(mMouseState.posCurrent.X.rel * mTranslateSpeedFactorMouse);
			mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
			mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(0, 1, 0) * +(mMouseState.posCurrent.Y.rel * mTranslateSpeedFactorMouse);
			mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
		}
		if (mMouseState.scrollVelocity != 0)
		{
			APE_LOG_TRACE("z: " << mMouseState.scrollVelocity);
			int transScalar = (mMouseState.scrollVelocity / 3) * mTranslateSpeedFactorMouse;
			if (transScalar < 0)
				transScalar -= mGeneralSpeedFactor;
			if (transScalar > 0)
				transScalar += mGeneralSpeedFactor;
			mUserInputMacroPose.userPosition += mUserInputMacroPose.userOrientation * ape::Vector3(0, 0, -transScalar);
			mpApeUserInputMacro->updateViewPose(mUserInputMacroPose);
		}
		mMouseState.isMouseMoved = false;
	}
}

void ape::OISUserInputPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		if (mpKeyboard)
			mpKeyboard->capture();
		if (mpMouse)
			mpMouse->capture();
		updateViewPoseByKeyBoard();
		updateViewPoseByMouse();
		std::this_thread::sleep_for (std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::OISUserInputPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OISUserInputPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OISUserInputPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::OISUserInputPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
