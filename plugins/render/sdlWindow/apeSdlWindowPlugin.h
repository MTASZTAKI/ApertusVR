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

#ifndef APE_SDLWINDOWPLUGIN_H
#define APE_SDLWINDOWPLUGIN_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeSdlWindowPluginConfigs.h"
#include "SDL.h"
#include "SDL_syswm.h"

#define THIS_PLUGINNAME "apeSdlWindowPlugin"

namespace ape
{
	class SdlWindowPlugin : public IPlugin
	{
	public:
		SdlWindowPlugin();

		~SdlWindowPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:
		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::SdlWindowPluginConfig mSdlWindowPluginConfig;

		void eventCallBack(const ape::Event& event);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateOgre21RenderPlugin()
	{
		return new ape::SdlWindowPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOgre21RenderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::SdlWindowPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateOgre21RenderPlugin, DestroyOgre21RenderPlugin);
		return 0;
	}
}

#endif
