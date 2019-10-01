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
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeIFileGeometry.h"
#include "apeIFileMaterial.h"
#include "apeILight.h"
#include "apeIManualTexture.h"
#include "apeINode.h"
#include "apeISky.h"
#include "apeITextGeometry.h"
#include "apeIWater.h"

#define THIS_PLUGINNAME "apeSkyWaterTerrainScenePlugin"

namespace ape
{
	class apeSkyWaterTerrainScenePlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		ape::WaterWeakPtr mWater;

		ape::SkyWeakPtr mSky;


		ape::NodeWeakPtr mSkyLightNode;

		ape::LightWeakPtr mSkyLight;

		ape::LightWeakPtr mSunLight;

		std::vector<ape::CameraWeakPtr> mCameras;

		int mExpectedCameraCount;

		void eventCallBack(const ape::Event& event);

		void createSky();

		void createWater();

	public:
		apeSkyWaterTerrainScenePlugin();

		~apeSkyWaterTerrainScenePlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeSkyWaterTerrainScenePlugin()
	{
		return new ape::apeSkyWaterTerrainScenePlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeSkyWaterTerrainScenePlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeSkyWaterTerrainScenePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeSkyWaterTerrainScenePlugin, DestroyapeSkyWaterTerrainScenePlugin);
		return 0;
	}
}

#endif
