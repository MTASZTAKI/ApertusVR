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

#ifndef APE_SCENERECORDERPLUGIN_H
#define APE_SCENERECORDERPLUGIN_H

#include <iostream>
#include <thread>
#include <random>
#include <functional>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include <fstream>
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

#define THIS_PLUGINNAME "ApeSceneRecorderPlugin"

namespace ape
{
	class ApeSceneRecorderPlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpScene;

		ape::ISystemConfig* mpSystemConfig;

		void eventCallBack(const ape::Event& event);

		bool mIsRecorder;

		bool mIsPlayer;

		bool mIsLooping;

		std::string mFileName;

		std::ofstream mFileStreamOut;

		std::ifstream mFileStreamIn;

		std::chrono::milliseconds mLastEventTimeStamp;

		float fck;

		void readEvent();

		void writeEvent(ape::Event event);

		void fireEvent(unsigned int millisceonds, ape::Event event);

	public:
		ApeSceneRecorderPlugin();

		~ApeSceneRecorderPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateApeSceneRecorderPlugin()
	{
		return new ape::ApeSceneRecorderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyApeSceneRecorderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::ApeSceneRecorderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeSceneRecorderPlugin, DestroyApeSceneRecorderPlugin);
		return 0;
	}
}

#endif
