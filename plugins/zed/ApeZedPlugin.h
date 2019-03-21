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
#include "system/ApeIMainWindow.h"
#include "plugin/ApeIPlugin.h"
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "managers/ApeISystemConfig.h"
#include "sceneelements/ApeICamera.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeIManualPass.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeIPointCloud.h"
#include "sceneelements/ApeISphereGeometry.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeITubeGeometry.h"
#include <sl/Camera.hpp>

#define THIS_PLUGINNAME "ApeZedPlugin"

union dataUnionBytesFloat {
	float f;
	char fBuff[sizeof(float)];
};

namespace Ape
{
	class ZedPlugin : public Ape::IPlugin
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
		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		void eventCallBack(const Ape::Event& event);

		Ape::PointCloudWeakPtr mPointCloud;

		sl::Camera mZed;

		int mZedResolutionWidth;

		int mZedResolutionHeight;

		int mPointCloudSize;

		int mPointCloudBoundingSphereRadius;

		sl::Mat mZedPointCloud;
		
		Ape::PointCloudPoints mApePointCloudPoints;

		Ape::PointCloudColors mApePointCloudColors;

		Ape::NodeWeakPtr mApePointCloudNode;

		Ape::PointCloudWeakPtr mApePointCloud;
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateZedPlugin()
	{
		return new Ape::ZedPlugin;
	}

	APE_PLUGIN_FUNC void DestroyZedPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ZedPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateZedPlugin, DestroyZedPlugin);
		return 0;
	}
}

#endif
