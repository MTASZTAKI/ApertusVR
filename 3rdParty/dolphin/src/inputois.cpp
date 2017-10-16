/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "inputois.h"

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OgreRenderWindow.h>

#include "gui.h"
#include "eventmanager.h"
#include "event.h"

Dolphin::InputOis& Dolphin::InputOis::GetSingleton()
{
	static Dolphin::InputOis instance;
	return instance;
}

OIS::InputManager* Dolphin::InputOis::GetInputManager()
{
	return mInputManager;
}

OIS::Mouse* Dolphin::InputOis::GetMouse()
{
	return mMouse;
}

OIS::Keyboard* Dolphin::InputOis::GetKeyboard()
{
	return mKeyboard;
}

void Dolphin::InputOis::CaptureMouse()
{
	mMouse->capture();
}

void Dolphin::InputOis::CaptureKeyboard()
{
	mKeyboard->capture();
}

void Dolphin::InputOis::InitializeOIS(Ogre::RenderWindow* mWindow)
{
	//create FrameListener
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);
}

//-------------------------------------------------------------------------------------
bool Dolphin::InputOis::keyPressed(const OIS::KeyEvent &arg)
{
	if (Dolphin::Gui::GetSingleton().IsDialogVisible()) return true;   // don't process any more keys if dialog is up
	
	if (arg.key == OIS::KC_A)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_A, arg);
	}
	else if (arg.key == OIS::KC_B)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_B, arg);
	}
	else if (arg.key == OIS::KC_F)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_F, arg);
	}
	else if (arg.key == OIS::KC_G)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_G, arg);
	}
	else if (arg.key == OIS::KC_R)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_R, arg);
	}
	else if (arg.key == OIS::KC_T)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_T, arg);
	}
	else if (arg.key == OIS::KC_F5)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_F5, arg);
	}
	else if (arg.key == OIS::KC_SYSRQ)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_PRINTSCREEN, arg);
	}
	else if (arg.key == OIS::KC_ESCAPE)
	{
		Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS_ESC, arg);
	}
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYPRESS, arg);
	return true;
}

bool Dolphin::InputOis::keyReleased(const OIS::KeyEvent &arg)
{
	Dolphin::EventManager<Dolphin::DO_KeyReleaseEvent, const OIS::KeyEvent >::GetSingleton().TriggerEvent(KEYRELEASE, arg);
	return true;
}

bool Dolphin::InputOis::mouseMoved(const OIS::MouseEvent &arg)
{
	Dolphin::EventManager<Dolphin::DO_PointerMoveEvent, const OIS::MouseEvent >::GetSingleton().TriggerEvent(POINTERMOVE, arg);
	return true;
}

bool Dolphin::InputOis::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	Dolphin::EventManager<Dolphin::DO_PointerButtonPressEvent, const OIS::MouseEvent, OIS::MouseButtonID>::GetSingleton().TriggerEvent(POINTERPRESS, arg, id);
	return true;
}

bool Dolphin::InputOis::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	Dolphin::EventManager<Dolphin::DO_PointerButtonReleaseEvent, const OIS::MouseEvent, OIS::MouseButtonID>::GetSingleton().TriggerEvent(POINTERRELEASE, arg, id);
	return true;
}

void Dolphin::InputOis::DestroyInputs()
{
	if (mInputManager)
	{
		mInputManager->destroyInputObject(mMouse);
		mInputManager->destroyInputObject(mKeyboard);
		OIS::InputManager::destroyInputSystem(mInputManager);
	}
}