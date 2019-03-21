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

#ifndef APE_LEAPMOTIONPLUGIN_H
#define APE_LEAPMOTIONPLUGIN_H

#include <iostream>
#include <string>
#include <thread>
#include "system/ApeIMainWindow.h"
#include "plugin/ApeIPlugin.h"
#include "plugin/ApePluginAPI.h"
#include "managers/ApeISystemConfig.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "sceneelements/ApeICamera.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeITextGeometry.h"
#include "Leap.h"
#include "LeapMath.h"

#define THIS_PLUGINNAME "ApeLeapMotionPlugin"

namespace Ape
{
	class LeapMotionPlugin : public Ape::IPlugin, public Leap::Listener 
	{
	public:
		LeapMotionPlugin();

		~LeapMotionPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		void onInit(const Leap::Controller& controller) override;

		void onConnect(const Leap::Controller& controller) override;

		void onDisconnect(const Leap::Controller& controller) override;

		void onExit(const Leap::Controller& controller) override;

		void onFrame(const Leap::Controller& controller) override;

		void onFocusGained(const Leap::Controller& controller) override;

		void onFocusLost(const Leap::Controller& controller) override;

		void onDeviceChange(const Leap::Controller& controller) override;

		void onServiceConnect(const Leap::Controller& controller) override;

		void onServiceDisconnect(const Leap::Controller& controller) override;

	private:

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IEventManager* mpEventManager;

		void eventCallBack(const Ape::Event& event);

		Leap::Controller mLeapController;

		std::vector<std::string> mFingerNames;
		
		std::vector<std::string> mBoneNames;
		
		std::vector<std::string> mStateNames;

		float mPreviousFramePitch;

		float mPreviousFrameYaw;

		float mPreviousFrameRoll;

		bool mHandOrientationFlag;

		Ape::NodeWeakPtr mLeftHandNode;

		Ape::NodeWeakPtr mRightHandNode;
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateLeapMotionPlugin()
	{
		return new Ape::LeapMotionPlugin;
	}

	APE_PLUGIN_FUNC void DestroyLeapMotionPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::LeapMotionPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateLeapMotionPlugin, DestroyLeapMotionPlugin);
		return 0;
	}
}

#endif
