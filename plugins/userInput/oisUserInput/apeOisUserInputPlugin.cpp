#include <fstream>
#include "apeOisUserInputPlugin.h"

ape::OISUserInputPlugin::OISUserInputPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpKeyboard = NULL;
	mpMouse = NULL;
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&OISUserInputPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
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
	if (inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
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
	//APE_LOG_DEBUG("OIS::KeyCode: " << (OIS::KeyCode)e.key);
	std::string keyAsString = mpKeyboard->getAsString(e.key);
	std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);
	//APE_LOG_DEBUG("keyAsString:" << keyAsString);
	mpUserInputMacro->keyPressedStringValue(keyAsString);
	return true;
}

bool ape::OISUserInputPlugin::keyReleased(const OIS::KeyEvent& e)
{
	//APE_LOG_DEBUG("OIS::KeyCode: " << (OIS::KeyCode)e.key);
	std::string keyAsString = mpKeyboard->getAsString(e.key);
	std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);
	//APE_LOG_DEBUG("keyAsString:" << keyAsString);
	mpUserInputMacro->keyReleasedStringValue(keyAsString);
	return true;
}

bool ape::OISUserInputPlugin::mouseMoved(const OIS::MouseEvent& e)
{
	ape::Vector2 posRel = ape::Vector2(e.state.X.rel, e.state.Y.rel);
	ape::Vector2 posAbs = ape::Vector2(e.state.X.abs, e.state.Y.abs);
	mpUserInputMacro->mouseMovedValue(posRel, posAbs);
	mpUserInputMacro->mouseScrolledValue(e.state.Z.rel);
	return true;
}

bool ape::OISUserInputPlugin::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	std::string mouseAsString;
	if (id == OIS::MouseButtonID::MB_Left)
		mouseAsString = "left";
	else if (id == OIS::MouseButtonID::MB_Right)
		mouseAsString = "right";
	else if (id == OIS::MouseButtonID::MB_Middle)
		mouseAsString = "middle";
	mpUserInputMacro->mousePressedStringValue(mouseAsString);
	return true;
}

bool ape::OISUserInputPlugin::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	std::string mouseAsString;
	if (id == OIS::MouseButtonID::MB_Left)
		mouseAsString = "left";
	else if (id == OIS::MouseButtonID::MB_Right)
		mouseAsString = "right";
	else if (id == OIS::MouseButtonID::MB_Middle)
		mouseAsString = "middle";
	mpUserInputMacro->mouseReleasedStringValue(mouseAsString);
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
