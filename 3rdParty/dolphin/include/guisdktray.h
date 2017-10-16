/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef GUISDKTRAY_H
#define GUISDKTRAY_H

#include <vector>

namespace OgreBites
{
	class ParamsPanel;
	class SdkTrayManager;
	struct InputContext;
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
	class GuiSdkTray
	{
	public:
		GuiSdkTray();
		virtual ~GuiSdkTray();
		void InitializeGuiSdkTray();
		void UpdateGuiSdkTray(const Ogre::FrameEvent& evt);
		void ToggleAdvancedFrameStats(const OIS::KeyEvent arg);
		void InitiateShutDown(const OIS::KeyEvent arg);

		void ShowFrameStats(OgreBites::TrayLocation trayloc);	//ToDo: add own location class and a dictionary for OgreBites::TrayLocation ?
		void ShowLogo(OgreBites::TrayLocation trayloc);
		void HideCursor();
		void Hide(std::string name);
		void Show(std::string name);
		bool IsVisible(std::string name);
		bool IsDialogVisible();
		bool GuiInjectPointerMove(const OIS::MouseEvent* arg);
		bool GuiInjectPointerDown(const OIS::MouseEvent* arg, OIS::MouseButtonID id);
		bool GuiInjectPointerUp(const OIS::MouseEvent* arg, OIS::MouseButtonID id);
		OgreBites::TrayLocation GetTrayLocation(std::string name);
		void MoveWidgetToTray(std::string name, OgreBites::TrayLocation);
		void RemoveWidgetFromTray(std::string name);
		OgreBites::ParamsPanel* CreateInfoPanel(OgreBites::TrayLocation trayloc, std::string name, float width, std::vector<std::string> infos);
		void SetInfoPanelParam(std::string name, unsigned int index, std::string ParamValue);
		std::string GetInfoPanelParam(std::string name, unsigned int index);
	private:
		OgreBites::InputContext* mInputContext;
		OgreBites::SdkTrayManager* mTrayMgr;
	};
}
#endif //GUISDKTRAY_H
