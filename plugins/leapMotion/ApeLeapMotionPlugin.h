/*MIT License

Copyright (c) 2016 MTA SZTAKI

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
#include "Leap.h"
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"
#include "ApeISystemConfig.h"
#include "ApeINode.h"
#include "ApeIScene.h"
#include "ApeIMainWindow.h"
#include "ApeIEventManager.h"
#include "ApeICamera.h"
#include "ApeITextGeometry.h"

namespace Ape
{
	class LeapMotionPlugin : public Ape::IPlugin
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


	private:

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		void eventCallBack(const Ape::Event& event);
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateLeapMotionPlugin()
	{
		return new LeapMotionPlugin;
	}

	APE_PLUGIN_FUNC void DestroyLeapMotionPlugin(Ape::IPlugin *plugin)
	{
		delete (LeapMotionPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME("LeapMotionPlugin");

	APE_PLUGIN_ALLOC()
	{
		std::cout << "LeapMotionPlugin_CREATE" << std::endl;
		ApeRegisterPlugin("LeapMotionPlugin", CreateLeapMotionPlugin, DestroyLeapMotionPlugin);
		return 0;
	}
}

#endif
