/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_VIEWPOINTMANAGERPLUGIN_H
#define APE_VIEWPOINTMANAGERPLUGIN_H

#include <iostream>
#include <list>
#include <string>
#include <thread>
#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeUserInputMacro.h"

#define THIS_PLUGINNAME "apeViewPointManagerPlugin"

namespace ape
{
	class ViewPointManagerPlugin : public ape::IPlugin
	{
	public:
		ViewPointManagerPlugin();

		~ViewPointManagerPlugin();

		void keyPressedStringEventCallback(const std::string& keyValue);

		void keyReleasedStringEventCallback(const std::string& keyValue);

		void mousePressedStringEventCallback(const std::string& mouseValue);

		void mouseReleasedStringEventCallback(const std::string& mouseValue);

		void mouseMovedEventCallback(const ape::Vector2& mouseValueRel, const ape::Vector2& mouseValueAbs);

		void controllerMovedEventCallback(ape::Vector3 controllerMovedValuePos, ape::Quaternion controllerMovedValueOri, ape::Vector3 controllerMovedValueScl);

		void controllerTouchpadPressedValue(const ape::Vector2& axis);

		void controllerTouchpadReleasedValue(const ape::Vector2& axis);

		void hmdMovedEventCallback(ape::Vector3 hmdMovedValuePos, ape::Quaternion hmdMovedValueOri, ape::Vector3 hmdMovedValueScl);

		void mouseScrolledEventCallback(const int& mouseValue);

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:
		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::IEventManager* mpEventManager;

		std::vector<ape::UserInputMacro::ViewPose> mViewPoses;

		bool mIsBrowserHovered;

		bool mIsKeyboardLockedByBrowser;

		int mViewPosesToggleIndex;

		float mTranslateSpeedFactorKeyboard;

		float mRotateSpeedFactorKeyboard;

		float mTranslateSpeedFactorMouse;

		float mRotateSpeedFactorMouse;

		float mGeneralSpeedFactor;

		bool mIsKeyReleased;

		bool mIsMouseReleased;

		ape::Vector2 mMouseMovedRelValue;

		int mMouseScrolledValue;

		ape::Quaternion mControllerLastOrientation;

		ape::Vector2 mControllerLastTouchAxis;

		bool mIsControllerTouchPressed;

		ape::UserInputMacro* mpUserInputMacro;

		void updateViewPoseByKeyBoard(const std::string& keyValue);

		void updateViewPoseByMouse(const std::string& mouseValue);

		void updateViewPoseByController();

		void updateViewPoseByHmd(ape::Vector3 hmdMovedValuePos, ape::Quaternion hmdMovedValueOri, ape::Vector3 hmdMovedValueScl);

		void toggleViewPoses(bool isInterpolated);

		void eventCallBack(const ape::Event& event);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateViewPointManagerPlugin()
	{
		return new ape::ViewPointManagerPlugin;
	}

	APE_PLUGIN_FUNC void DestroyViewPointManagerPlugin(ape::IPlugin *plugin)
	{
		delete (ape::ViewPointManagerPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateViewPointManagerPlugin, DestroyViewPointManagerPlugin);
		return 0;
	}
}

#endif
