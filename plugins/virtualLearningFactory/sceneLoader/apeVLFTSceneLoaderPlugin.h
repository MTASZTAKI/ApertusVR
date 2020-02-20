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

#ifndef APE_VLFTSCENELOADERPLUGIN_H
#define APE_VLFTSCENELOADERPLUGIN_H

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <functional>
#include <fstream>
#include "apePluginAPI.h"
#include "apeILogManager.h"
#include "apeIEventManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeIFileGeometry.h"
#include "apeIPlaneGeometry.h"
#include "apeIFileMaterial.h"
#include "apeIRayGeometry.h"
#include "apeIBrowser.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeINode.h"
#include "apeICamera.h"
#include "apeIManualTexture.h"
#include "apeIFileTexture.h"
#include "apeIManualMaterial.h"
#include "apeUserInputMacro.h"
#include "apeSceneMakerMacro.h"
#include "apeEuler.h"
#include "apeVLFTSceneLoaderPluginConfig.h"

#define THIS_PLUGINNAME "apeVLFTSceneLoaderPlugin"

namespace ape
{
	class VLFTSceneLoaderPlugin : public ape::IPlugin
	{
	private:
		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::UserInputMacro* mpApeUserInputMacro;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		ape::ICoreConfig* mpCoreConfig;

		quicktype::Scene mScene;

	public:
		VLFTSceneLoaderPlugin();

		~VLFTSceneLoaderPlugin();

		void eventCallBack(const ape::Event& event);
		
		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeVLFTSceneLoaderPlugin()
	{
		return new ape::VLFTSceneLoaderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeVLFTSceneLoaderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::VLFTSceneLoaderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeVLFTSceneLoaderPlugin, DestroyapeVLFTSceneLoaderPlugin);
		return 0;
	}
}

#endif
