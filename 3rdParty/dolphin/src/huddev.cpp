/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "huddev.h"
#include "gui.h"
#include "renderogre.h"
#include "eventmanager.h"
#include "event.h"

Dolphin::HudDev::HudDev()
{
	Dolphin::EventManager<DO_GuiEvent>::GetSingleton().SubscribeEvent(TIMETOCREATEGUI, std::bind(&HudDev::CreateHudDev, this));
}


Dolphin::HudDev::~HudDev()
{
}

void Dolphin::HudDev::ToggleAdvancedFrameStats(const OIS::KeyEvent arg)
{
	Dolphin::Gui::GetSingleton().ToggleAdvancedFrameStats(&arg);  //This toggle implement in sdktray class...
}

void Dolphin::HudDev::InitiateShutDown(const OIS::KeyEvent arg)
{
	Dolphin::Gui::GetSingleton().InitiateShutDown(&arg);
}

void Dolphin::HudDev::ToggleDebuggingDetails(const OIS::KeyEvent arg)
{
	if (Dolphin::Gui::GetSingleton().GetTrayLocation("DetailsPanel") == OgreBites::TL_NONE)
	{
		Dolphin::Gui::GetSingleton().MoveWidgetToTray("DetailsPanel", OgreBites::TL_TOPRIGHT);
		Dolphin::Gui::GetSingleton().Show("DetailsPanel");
	}
	else
	{
		Dolphin::Gui::GetSingleton().RemoveWidgetFromTray("DetailsPanel");
		Dolphin::Gui::GetSingleton().Hide("DetailsPanel");
	}
}


void Dolphin::HudDev::CreateHudDev()
{
	Dolphin::Gui::GetSingleton().ShowFrameStats(OgreBites::TL_BOTTOMLEFT);
	Dolphin::Gui::GetSingleton().ShowLogo(OgreBites::TL_BOTTOMRIGHT);
	Dolphin::Gui::GetSingleton().HideCursor();

	std::vector<std::string> items;
	items.push_back("cam.pX");
	items.push_back("cam.pY");
	items.push_back("cam.pZ");
	items.push_back("");
	items.push_back("cam.oW");
	items.push_back("cam.oX");
	items.push_back("cam.oY");
	items.push_back("cam.oZ");
	items.push_back("");
	items.push_back("Filtering");
	items.push_back("Poly Mode");

	Dolphin::Gui::GetSingleton().CreateInfoPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);

	Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 9, "Bilinear");
	Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 10, "Solid");

	Dolphin::Gui::GetSingleton().Hide("DetailsPanel");

	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(KEYPRESS_F, std::bind(&HudDev::ToggleAdvancedFrameStats, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(KEYPRESS_G, std::bind(&HudDev::ToggleDebuggingDetails, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(KEYPRESS_ESC, std::bind(&HudDev::InitiateShutDown, this, std::placeholders::_1));
	Dolphin::EventManager<DO_GuiEvent>::GetSingleton().SubscribeEvent(TIMETOUPDATEGUI, std::bind(&HudDev::UpdateHudDev, this));
	
}

void Dolphin::HudDev::UpdateHudDev()
{
	OgreBites::SdkCameraMan* mCameraMan = RenderOgre::getmCameraMan();
	Ogre::Camera* mCamera = RenderOgre::getmCamera();
	
	if (Dolphin::Gui::GetSingleton().IsVisible("DetailsPanel"))   // if details panel is visible, then update its contents
	{
		Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
		Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
		Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
		Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
		Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
		Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
		Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 7, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
	}
}

