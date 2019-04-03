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

#ifndef APE_LEGOPLUGIN_H
#define APE_LEGOPLUGIN_H

#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeIIndexedFaceSetGeometry.h"
#include "sceneelements/apeIIndexedLineSetGeometry.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeIPbsPass.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneelements/apeITubeGeometry.h"
#include "utils/apeInterpolator.h"
#include "macros/sceneMaker/apeSceneMakerMacro.h"

#define THIS_PLUGINNAME "apeLegoPlugin"

namespace ape
{
	class apeLegoPlugin : public ape::IPlugin
	{
	private:

		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		std::vector<std::string> mModelFileNames;

		std::vector<ape::Vector3> mInterpolatePositions;

		void eventCallBack(const ape::Event& event);

		void blowModel();

	public:
		apeLegoPlugin();

		~apeLegoPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeLegoPlugin()
	{
		return new ape::apeLegoPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeLegoPlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeLegoPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeLegoPlugin, DestroyapeLegoPlugin);
		return 0;
	}
}

#endif
