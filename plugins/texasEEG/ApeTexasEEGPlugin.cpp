#include <iostream>
#include "ApeTexasEEGPlugin.h"

ApeTexasEEGPlugin::ApeTexasEEGPlugin()
{
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeTexasEEGPlugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
}

ApeTexasEEGPlugin::~ApeTexasEEGPlugin()
{
	std::cout << "ApeTexasEEGPlugin dtor" << std::endl;
}

void ApeTexasEEGPlugin::nodeEventCallBack(const Ape::Event& event)
{
	//std::cout  << "event called on the " << event.subjectName << " node" << std::endl;
}

void ApeTexasEEGPlugin::Init()
{
	std::cout << "ApeTexasEEGPlugin::init" << std::endl;
	std::cout << "ApeTexasEEGPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "ApeTexasEEGPlugin main window was found" << std::endl;
	std::stringstream hwndStrStream;
	hwndStrStream << mpMainWindow->getHandle();
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

	if (inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
	{
		OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
		mpMouse = mouse;
		mpMouse->setEventCallback(this);
		const OIS::MouseState &ms = mouse->getMouseState();
		ms.width = mpMainWindow->getWidth();
		ms.height = mpMainWindow->getHeight();
	}
}

void ApeTexasEEGPlugin::Run()
{
	while (true)
	{
		if (mpMouse)
			mpMouse->capture();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeTexasEEGPlugin::nodeEventCallBack, this, std::placeholders::_1));
}

void ApeTexasEEGPlugin::Step()
{

}

void ApeTexasEEGPlugin::Stop()
{

}

void ApeTexasEEGPlugin::Suspend()
{

}

void ApeTexasEEGPlugin::Restart()
{

}

bool ApeTexasEEGPlugin::mouseMoved(const OIS::MouseEvent & e)
{
	return true;
}

bool ApeTexasEEGPlugin::mousePressed(const OIS::MouseEvent & e, OIS::MouseButtonID id)
{
	return true;
}

bool ApeTexasEEGPlugin::mouseReleased(const OIS::MouseEvent & e, OIS::MouseButtonID id)
{
	return true;
}
