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

#ifndef APE_AUDIPLUGIN_H
#define APE_AUDIPLUGIN_H

#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "managers/ApeISceneSession.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeILight.h"
#include "sceneelements/ApeICamera.h"
#include "managers/ApeISystemConfig.h"
#include "sceneelements/ApeIFileMaterial.h"
#include "sceneelements/ApeIPlaneGeometry.h"
#include "sceneelements/ApeITubeGeometry.h"
#include "sceneelements/ApeIConeGeometry.h"
#include "sceneelements/ApeISphereGeometry.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeIIndexedFaceSetGeometry.h"
#include "sceneelements/ApeIIndexedLineSetGeometry.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeIPbsPass.h"
#include "utils/ApeInterpolator.h"
#include "sceneelements/ApeIFileGeometry.h"
#include "system/ApeIMainWindow.h"
#include "sceneelements/ApeIPointCloud.h"
#include "ApeSceneMakerMacro.h"

#define THIS_PLUGINNAME "ApeAudiPlugin"

namespace ape
{
	class ApeAudiPlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpScene;

		ape::ISystemConfig* mpSystemConfig;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		void eventCallBack(const ape::Event& event);

	public:
		ApeAudiPlugin();

		~ApeAudiPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateApeAudiPlugin()
	{
		return new ape::ApeAudiPlugin;
	}

	APE_PLUGIN_FUNC void DestroyApeAudiPlugin(ape::IPlugin *plugin)
	{
		delete (ape::ApeAudiPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeAudiPlugin, DestroyApeAudiPlugin);
		return 0;
	}
}

#endif
