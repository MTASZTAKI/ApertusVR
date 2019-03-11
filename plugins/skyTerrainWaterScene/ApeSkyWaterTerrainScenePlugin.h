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

#ifndef APE_SKYWATERTERRAINLUGIN_H
#define APE_SKYWATERTERRAINLUGIN_H

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "system/ApeIMainWindow.h"
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "sceneelements/ApeIFileGeometry.h"
#include "sceneelements/ApeIFileMaterial.h"
#include "sceneelements/ApeILight.h"
#include "sceneelements/ApeIManualTexture.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeISky.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeIWater.h"

#define THIS_PLUGINNAME "ApeSkyWaterTerrainScenePlugin"

namespace Ape
{
	class ApeSkyWaterTerrainScenePlugin : public Ape::IPlugin
	{
	private:
		Ape::IEventManager* mpEventManager;

		Ape::ISceneManager* mpSceneManager;

		Ape::WaterWeakPtr mWater;

		Ape::SkyWeakPtr mSky;


		Ape::NodeWeakPtr mSkyLightNode;

		Ape::LightWeakPtr mSkyLight;

		Ape::LightWeakPtr mSunLight;

		std::vector<Ape::CameraWeakPtr> mCameras;

		int mExpectedCameraCount;

		void eventCallBack(const Ape::Event& event);

		void createSky();

		void createWater();

	public:
		ApeSkyWaterTerrainScenePlugin();

		~ApeSkyWaterTerrainScenePlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC Ape::IPlugin* CreateApeSkyWaterTerrainScenePlugin()
	{
		return new Ape::ApeSkyWaterTerrainScenePlugin;
	}

	APE_PLUGIN_FUNC void DestroyApeSkyWaterTerrainScenePlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ApeSkyWaterTerrainScenePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeSkyWaterTerrainScenePlugin, DestroyApeSkyWaterTerrainScenePlugin);
		return 0;
	}
}

#endif
