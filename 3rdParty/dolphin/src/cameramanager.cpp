/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include "eventmanager.h"
#include "event.h"
#include <cameramanager.h>
#include <gui.h>


void Dolphin::CameraManager::SendKeyDown(const OIS::KeyEvent arg)
{
	ogrecameraman->injectKeyDown(arg);
}

void Dolphin::CameraManager::SendKeyUp(const OIS::KeyEvent arg)
{
	ogrecameraman->injectKeyUp(arg);
}

void Dolphin::CameraManager::SendPointerMove(const OIS::MouseEvent arg)
{
	if (!Dolphin::Gui::GetSingleton().GuiInjectPointerMove(&arg))
	ogrecameraman->injectPointerMove(arg);
}

void Dolphin::CameraManager::SendPointerButtonDown(const OIS::MouseEvent arg, OIS::MouseButtonID id)
{
	if (!Dolphin::Gui::GetSingleton().GuiInjectPointerDown(&arg, id))
	ogrecameraman->injectPointerDown(arg, id);
}

void Dolphin::CameraManager::SendPointerButtonUp(const OIS::MouseEvent arg, OIS::MouseButtonID id)
{
	if (!Dolphin::Gui::GetSingleton().GuiInjectPointerUp(&arg, id))
	ogrecameraman->injectPointerDown(arg, id);
}


Dolphin::CameraManager::CameraManager(OgreBites::SdkCameraMan* ogrecamera)
{
	ogrecameraman = ogrecamera;
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(Dolphin::KEYPRESS, std::bind(&CameraManager::SendKeyDown, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_KeyReleaseEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(Dolphin::KEYRELEASE, std::bind(&CameraManager::SendKeyUp, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_PointerMoveEvent, const OIS::MouseEvent >::GetSingleton().SubscribeEvent(Dolphin::POINTERMOVE, std::bind(&CameraManager::SendPointerMove, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_PointerButtonPressEvent, const OIS::MouseEvent, OIS::MouseButtonID >::GetSingleton().SubscribeEvent(Dolphin::POINTERPRESS, std::bind(&CameraManager::SendPointerButtonDown, this, std::placeholders::_1, std::placeholders::_2));
	Dolphin::EventManager<Dolphin::DO_PointerButtonReleaseEvent, const OIS::MouseEvent, OIS::MouseButtonID >::GetSingleton().SubscribeEvent(Dolphin::POINTERRELEASE, std::bind(&CameraManager::SendPointerButtonUp, this, std::placeholders::_1, std::placeholders::_2));
}

Dolphin::CameraManager::~CameraManager()
{

}

