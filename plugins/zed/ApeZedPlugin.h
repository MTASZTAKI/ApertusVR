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

#ifndef APE_ZEDPLUGIN_H
#define APE_ZEDPLUGIN_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include "plugin/apeIPlugin.h"
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIManualPass.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeIPointCloud.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneelements/apeITubeGeometry.h"
#include <sl/Camera.hpp>

#define THIS_PLUGINNAME "apeZedPlugin"

union dataUnionBytesFloat {
	float f;
	char fBuff[sizeof(float)];
};

namespace ape
{
	class ZedPlugin : public ape::IPlugin
	{
	public:
		ZedPlugin();

		~ZedPlugin();

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

		ape::NodeWeakPtr mUserNode;

		void eventCallBack(const ape::Event& event);

		ape::PointCloudWeakPtr mPointCloud;

		sl::Camera mZed;

		int mZedResolutionWidth;

		int mZedResolutionHeight;

		int mPointCloudSize;

		int mPointCloudBoundingSphereRadius;

		sl::Mat mZedPointCloud;
		
		ape::PointCloudPoints mApePointCloudPoints;

		ape::PointCloudColors mApePointCloudColors;

		ape::NodeWeakPtr mApePointCloudNode;

		ape::PointCloudWeakPtr mApePointCloud;
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateZedPlugin()
	{
		return new ape::ZedPlugin;
	}

	APE_PLUGIN_FUNC void DestroyZedPlugin(ape::IPlugin *plugin)
	{
		delete (ape::ZedPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateZedPlugin, DestroyZedPlugin);
		return 0;
	}
}

#endif
