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

#ifndef APE_VLFTUIMANAGERPLUGIN_H
#define APE_VLFTUIMANAGERPLUGIN_H

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
#include "apeSceneMakerMacro.h"

#define THIS_PLUGINNAME "apeVLFTUIManagerPlugin"

namespace ape
{
	class VLFTUIManagerPlugin : public ape::IPlugin
	{
	public:
		VLFTUIManagerPlugin();

		~VLFTUIManagerPlugin();

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

		ape::UserInputMacro* mpUserInputMacro;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		ape::UserInputMacro::OverlayBrowserCursor mOverlayBrowserCursor;

		ape::Vector2 mMouseMovedValueAbs;

		int mMouseScrolledValue;

		void parseNodeJsPluginConfig();

		int mServerPort;

		void keyPressedStringEventCallback(const std::string& keyValue);

		void mousePressedStringEventCallback(const std::string& keyValue);

		void mouseReleasedStringEventCallback(const std::string& keyValue);

		void mouseMovedCallback(const ape::Vector2& mouseMovedValueRel, const ape::Vector2& mouseMovedValueAbs);

		void eventCallBack(const ape::Event& event);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateVLFTUIManagerPlugin()
	{
		return new ape::VLFTUIManagerPlugin;
	}

	APE_PLUGIN_FUNC void DestroyVLFTUIManagerPlugin(ape::IPlugin *plugin)
	{
		delete (ape::VLFTUIManagerPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateVLFTUIManagerPlugin, DestroyVLFTUIManagerPlugin);
		return 0;
	}
}

#endif
