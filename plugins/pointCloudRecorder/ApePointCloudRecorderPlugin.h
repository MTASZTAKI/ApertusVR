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

#ifndef APE_POINTCLOUDRECORDERPLUGIN_H
#define APE_POINTCLOUDRECORDERPLUGIN_H

#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include <fstream>
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeIScene.h"
#include "ApeISceneSession.h"
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
#include "ApeInterpolator.h"
#include "ApeIFileGeometry.h"
#include "ApeIMainWindow.h"
#include "ApeIPointCloud.h"

#define THIS_PLUGINNAME "ApePointCloudRecorderPlugin"

namespace Ape
{
	class ApePointCloudRecorderPlugin : public Ape::IPlugin
	{
	private:
		Ape::IEventManager* mpEventManager;

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		void eventCallBack(const Ape::Event& event);

		Ape::NodeWeakPtr mUserNode;

		Ape::PointCloudWeakPtr mPointCloud;

		std::string mRecordedPointCloudName;

		bool mIsRecorder;

		bool mIsPlayer;

		bool mIsLooping;

		std::string mFileName;

		std::ofstream mFileStreamOut;

		std::ifstream mFileStreamIn;

		Ape::Vector3 mPointCloudPosition;

		Ape::Quaternion mPointCloudOrinetation;

		Ape::NodeWeakPtr mPointCloudNode;

		long mPointCloudSize;

		Ape::PointCloudPoints mCurrentPoints;

		Ape::PointCloudColors mCurrentColors;

		void readFrame();

		void writeFrame();

	public:
		ApePointCloudRecorderPlugin();

		~ApePointCloudRecorderPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC Ape::IPlugin* CreateApePointCloudRecorderPlugin()
	{
		return new Ape::ApePointCloudRecorderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyApePointCloudRecorderPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ApePointCloudRecorderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApePointCloudRecorderPlugin, DestroyApePointCloudRecorderPlugin);
		return 0;
	}
}

#endif
