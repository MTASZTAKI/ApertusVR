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

#ifndef APE_WEBSERVERPLUGIN_H
#define APE_WEBSERVERPLUGIN_H

#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <unordered_set>
#include <vector>
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeIBrowser.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeIIndexedFaceSetGeometry.h"
#include "sceneelements/apeIIndexedLineSetGeometry.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIManualPass.h"
#include "sceneelements/apeIManualTexture.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeIPbsPass.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeIPointCloud.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneelements/apeITubeGeometry.h"
#include "utils/apeInterpolator.h"
#include "crow.h"

#define THIS_PLUGINNAME "apeWebserverPlugin"

namespace ape
{
	class apeWebserverPlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		std::unordered_set<crow::websocket::connection*> mWebSocketUsers;

		void eventCallBack(const ape::Event& event);

	public:
		apeWebserverPlugin();

		~apeWebserverPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeWebserverPlugin()
	{
		return new ape::apeWebserverPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeWebserverPlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeWebserverPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeWebserverPlugin, DestroyapeWebserverPlugin);
		return 0;
	}
}

#endif
