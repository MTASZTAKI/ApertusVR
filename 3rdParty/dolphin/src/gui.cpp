/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "gui.h"

#include <SdkTrays.h>

#include "guisdktray.h"
#include "eventmanager.h"
#include "event.h"

Dolphin::Gui& Dolphin::Gui::GetSingleton()
{
	static Dolphin::Gui instance;
	return instance;
}

void Dolphin::Gui::ToggleAdvancedFrameStats(const OIS::KeyEvent* arg)
{
	guisdktray->ToggleAdvancedFrameStats(*arg);
}

void Dolphin::Gui::InitiateShutDown(const OIS::KeyEvent* arg)
{
	guisdktray->InitiateShutDown(*arg);
}

void Dolphin::Gui::InitializeGui()
{
	guisdktray = new GuiSdkTray();
	guisdktray->InitializeGuiSdkTray();
}

void Dolphin::Gui::DestroyGui()
{
	if (guisdktray) delete guisdktray;
}

void Dolphin::Gui::UpdateGui(const Ogre::FrameEvent& evt)
{
	guisdktray->UpdateGuiSdkTray(evt);
}

void Dolphin::Gui::ShowFrameStats(GuiItemsLocation trayloc)
{
	guisdktray->ShowFrameStats(trayloc);
}

void Dolphin::Gui::ShowLogo(GuiItemsLocation trayloc)
{
	guisdktray->ShowLogo(trayloc);
}

void Dolphin::Gui::HideCursor()
{
	guisdktray->HideCursor();
}

Dolphin::Gui::InfoPanel* Dolphin::Gui::CreateInfoPanel(GuiItemsLocation trayloc, std::string name, float width, std::vector<std::string> infos)
{
	return guisdktray->CreateInfoPanel(trayloc, name, width, infos);
}

void Dolphin::Gui::SetInfoPanelParam(std::string name, unsigned int index, std::string ParamValue)
{
	guisdktray->SetInfoPanelParam(name, index, ParamValue);
}

std::string Dolphin::Gui::GetInfoPanelParam(std::string name, unsigned int index)
{
	return guisdktray->GetInfoPanelParam(name, index);
}

void Dolphin::Gui::Hide(std::string name)
{
	guisdktray->Hide(name);
}

void Dolphin::Gui::Show(std::string name)
{
	guisdktray->Show(name);
}

bool Dolphin::Gui::IsVisible(std::string name)
{
	return guisdktray->IsVisible(name);
}

bool Dolphin::Gui::IsDialogVisible()
{
	return guisdktray->IsDialogVisible();
}

Dolphin::Gui::GuiItemsLocation Dolphin::Gui::GetTrayLocation(std::string name)
{
	return  guisdktray->GetTrayLocation(name);
}

void Dolphin::Gui::MoveWidgetToTray(std::string name, GuiItemsLocation trayloc)
{
	guisdktray->MoveWidgetToTray(name, OgreBites::TL_TOPRIGHT);
}

void Dolphin::Gui::RemoveWidgetFromTray(std::string name)
{
	guisdktray->RemoveWidgetFromTray(name);
}

bool Dolphin::Gui::GuiInjectPointerMove(const OIS::MouseEvent* arg)
{
	return guisdktray->GuiInjectPointerMove(arg);
}

bool Dolphin::Gui::GuiInjectPointerDown(const OIS::MouseEvent* arg, OIS::MouseButtonID id)
{
	return guisdktray->GuiInjectPointerDown(arg, id);
}

bool Dolphin::Gui::GuiInjectPointerUp(const OIS::MouseEvent* arg, OIS::MouseButtonID id)
{
	return guisdktray->GuiInjectPointerUp(arg, id);
}