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

#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include <random>
#include <unordered_set>
#include "crow.h"
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeISceneManager.h"
#include "ApeINode.h"
#include "ApeILight.h"
#include "ApeICamera.h"
#include "ApeISystemConfig.h"
#include "ApeIFileMaterial.h"
#include "ApeIPlaneGeometry.h"
#include "ApeITubeGeometry.h"
#include "ApeIConeGeometry.h"
#include "ApeISphereGeometry.h"
#include "ApeITextGeometry.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeIIndexedLineSetGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeIPbsPass.h"
#include "ApeIManualPass.h"
#include "ApeInterpolator.h"
#include "ApeIFileGeometry.h"
#include "ApeIManualTexture.h"
#include "ApeIManualMaterial.h"
#include "ApeIBrowser.h"
#include "ApeIPointCloud.h"

#define THIS_PLUGINNAME "ApeWebserverPlugin"

namespace Ape
{
	class ApeWebserverPlugin : public Ape::IPlugin
	{
	private:
		Ape::IEventManager* mpEventManager;

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		std::unordered_set<crow::websocket::connection*> mWebSocketUsers;

		void eventCallBack(const Ape::Event& event);

	public:
		ApeWebserverPlugin();

		~ApeWebserverPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC Ape::IPlugin* CreateApeWebserverPlugin()
	{
		return new Ape::ApeWebserverPlugin;
	}

	APE_PLUGIN_FUNC void DestroyApeWebserverPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ApeWebserverPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeWebserverPlugin, DestroyApeWebserverPlugin);
		return 0;
	}
}

#endif
