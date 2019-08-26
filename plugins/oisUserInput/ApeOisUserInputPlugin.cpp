#include <fstream>
#include "apeOisUserInputPlugin.h"
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
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mKeyCodeMap = std::map<OIS::KeyCode, bool>();
	mIsKeyPressed = false;
	APE_LOG_FUNC_LEAVE();
}

ape::OISUserInputPlugin::~OISUserInputPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
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
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	APE_LOG_DEBUG("waiting for main window");
	while (mpCoreConfig->getWindowConfig().handle == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	APE_LOG_DEBUG("main window was found");
	std::stringstream hwndStrStream;
	hwndStrStream << mpCoreConfig->getWindowConfig().handle;
	std::stringstream windowHndStr;
	windowHndStr << std::stoul(hwndStrStream.str(), nullptr, 16);
	OIS::ParamList pl;
	pl.insert(std::make_pair("WINDOW", windowHndStr.str()));
#ifdef WIN32
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#endif
	OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
	if (inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
	{
		OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
		mpKeyboard = keyboard;
		mpKeyboard->setEventCallback(this);
	}
	else if (inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
	{
		OIS::Mouse* mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
		mpMouse = mouse;
		mpMouse->setEventCallback(this);
		const OIS::MouseState &ms = mouse->getMouseState();
		ms.width = mpCoreConfig->getWindowConfig().width;
		ms.height = mpCoreConfig->getWindowConfig().height;
	}
	APE_LOG_FUNC_LEAVE();
}

bool ape::OISUserInputPlugin::keyPressed(const OIS::KeyEvent& e)
{
	APE_LOG_DEBUG("OIS::KeyCode: " << (OIS::KeyCode)e.key);
	mKeyCodeMap[e.key] = true;
	std::string keyAsString = mpKeyboard->getAsString(e.key);
	//if (!mKeyCodeMap[OIS::KeyCode::KC_LSHIFT] && !mKeyCodeMap[OIS::KeyCode::KC_RSHIFT])
	std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);
	APE_LOG_DEBUG("keyAsString:" << keyAsString);
	mpUserInputMacro->keyStringValue(keyAsString);
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
	return true;
}

bool ape::OISUserInputPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	return true;
}

bool ape::OISUserInputPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
}

bool ape::OISUserInputPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	return true;
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
