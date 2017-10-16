/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include <SdkTrays.h>

#include "guisdktray.h"
#include "inputois.h"
#include "renderogre.h"
#include "eventmanager.h"
#include "event.h"

Dolphin::GuiSdkTray::GuiSdkTray()
{
}

Dolphin::GuiSdkTray::~GuiSdkTray()
{
	delete mInputContext;
}

void Dolphin::GuiSdkTray::ToggleAdvancedFrameStats(const OIS::KeyEvent arg)
{
	mTrayMgr->toggleAdvancedFrameStats();
}

void Dolphin::GuiSdkTray::InitiateShutDown(const OIS::KeyEvent arg)
{
	RenderOgre::SetShutDown(true);
}

void Dolphin::GuiSdkTray::InitializeGuiSdkTray()
{
	mInputContext = new OgreBites::InputContext();
	Ogre::RenderWindow* mWindow = RenderOgre::getmWindow();
	mInputContext->mKeyboard = Dolphin::InputOis::GetSingleton().GetKeyboard();
	mInputContext->mMouse = Dolphin::InputOis::GetSingleton().GetMouse();
	mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, *mInputContext);
	Dolphin::EventManager<Dolphin::DO_GuiEvent>::GetSingleton().TriggerEvent(TIMETOCREATEGUI);
}

void Dolphin::GuiSdkTray::UpdateGuiSdkTray(const Ogre::FrameEvent& evt)
{
	if (!mTrayMgr->isDialogVisible())
	{
		mTrayMgr->frameRenderingQueued(evt);
		Dolphin::EventManager<Dolphin::DO_GuiEvent>::GetSingleton().TriggerEvent(TIMETOUPDATEGUI);
	}
}

void Dolphin::GuiSdkTray::ShowFrameStats(OgreBites::TrayLocation trayloc)
{
	mTrayMgr->showFrameStats(trayloc);
}

void Dolphin::GuiSdkTray::ShowLogo(OgreBites::TrayLocation trayloc)
{
	mTrayMgr->showLogo(trayloc);
}

void Dolphin::GuiSdkTray::HideCursor()
{
	mTrayMgr->hideCursor();
}

OgreBites::ParamsPanel* Dolphin::GuiSdkTray::CreateInfoPanel(OgreBites::TrayLocation trayloc, Ogre::String name, Ogre::Real width, std::vector<std::string> infos)
{
	Ogre::StringVector osv;
	for (auto const& value : infos)
	{
		osv.push_back(value);
	}
	return mTrayMgr->createParamsPanel(trayloc, name, width, osv);
}

void Dolphin::GuiSdkTray::SetInfoPanelParam(std::string name, unsigned int index, std::string paramvalue)
{
	OgreBites::ParamsPanel* paramspanel = static_cast<OgreBites::ParamsPanel*>(mTrayMgr->getWidget(name));
	paramspanel->setParamValue(index, paramvalue);
}

std::string Dolphin::GuiSdkTray::GetInfoPanelParam(std::string name, unsigned int index)
{
	OgreBites::ParamsPanel* paramspanel = static_cast<OgreBites::ParamsPanel*>(mTrayMgr->getWidget(name));
	return paramspanel->getParamValue(index);
}

void Dolphin::GuiSdkTray::Hide(std::string name)
{
	mTrayMgr->getWidget(name)->hide();
}

void Dolphin::GuiSdkTray::Show(std::string name)
{
	mTrayMgr->getWidget(name)->show();
}

bool Dolphin::GuiSdkTray::IsVisible(std::string name)
{
	return mTrayMgr->getWidget(name)->isVisible();
}

OgreBites::TrayLocation Dolphin::GuiSdkTray::GetTrayLocation(std::string name)
{
	return  mTrayMgr->getWidget(name)->getTrayLocation();
}

void Dolphin::GuiSdkTray::MoveWidgetToTray(std::string name, OgreBites::TrayLocation)
{
	mTrayMgr->moveWidgetToTray(mTrayMgr->getWidget(name), OgreBites::TL_TOPRIGHT, 0);
}

void Dolphin::GuiSdkTray::RemoveWidgetFromTray(std::string name)
{
	mTrayMgr->removeWidgetFromTray(mTrayMgr->getWidget(name));
}

bool Dolphin::GuiSdkTray::IsDialogVisible()
{
	return mTrayMgr->isDialogVisible();
}

bool Dolphin::GuiSdkTray::GuiInjectPointerMove(const OIS::MouseEvent* arg)
{
	return mTrayMgr->injectPointerMove(*arg);
}

bool Dolphin::GuiSdkTray::GuiInjectPointerDown(const OIS::MouseEvent* arg, OIS::MouseButtonID id)
{
	return mTrayMgr->injectPointerDown(*arg, id);
}

bool Dolphin::GuiSdkTray::GuiInjectPointerUp(const OIS::MouseEvent* arg, OIS::MouseButtonID id)
{
	return mTrayMgr->injectPointerUp(*arg, id);
}