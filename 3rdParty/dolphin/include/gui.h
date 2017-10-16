/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef GUI_H
#define GUI_H

#include <vector>

namespace OgreBites
{
	class ParamsPanel;
	enum TrayLocation;
}

namespace OIS
{
	class KeyEvent;
	class MouseEvent;
	enum MouseButtonID;
}

namespace Ogre
{
	struct FrameEvent;
}


namespace Dolphin
{
	class GuiSdkTray;

	class Gui
	{
	public:
		typedef OgreBites::TrayLocation GuiItemsLocation;	//if change gui system the location type will be changed
		typedef OgreBites::ParamsPanel InfoPanel;
		
		static Gui& GetSingleton();
		Gui(Gui const&) = delete;
		void InitializeGui();
		void DestroyGui();
		void UpdateGui(const Ogre::FrameEvent& evt);

		void ToggleAdvancedFrameStats(const OIS::KeyEvent* arg);
		void InitiateShutDown(const OIS::KeyEvent* arg);

		void ShowFrameStats(GuiItemsLocation trayloc);	//ToDo: add own location class and a dictionary for OgreBites::TrayLocation ?
		void ShowLogo(GuiItemsLocation trayloc);
		void HideCursor();
		void Hide(std::string name);
		void Show(std::string name);
		bool IsVisible(std::string name);
		bool IsDialogVisible();
		bool GuiInjectPointerMove(const OIS::MouseEvent* arg);
		bool GuiInjectPointerDown(const OIS::MouseEvent* arg, OIS::MouseButtonID id);
		bool GuiInjectPointerUp(const OIS::MouseEvent* arg, OIS::MouseButtonID id);
		GuiItemsLocation GetTrayLocation(std::string name);
		void MoveWidgetToTray(std::string name, GuiItemsLocation trayloc);
		void RemoveWidgetFromTray(std::string name);
		InfoPanel* CreateInfoPanel(GuiItemsLocation trayloc, std::string name, float width, std::vector<std::string> infos);
		void SetInfoPanelParam(std::string name, unsigned int index, std::string ParamValue);
		std::string GetInfoPanelParam(std::string name, unsigned int index);
	private:
		GuiSdkTray* guisdktray;
		Gui() {};

		
	};
}
#endif //GUI_H
