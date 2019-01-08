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

#ifndef APE_MULTIKINECTPLUGIN_H
#define APE_MULTIKINECTPLUGIN_H

#include <iostream>
#include <string>
#include <thread> 
#include <fstream>
#include <strsafe.h>
#include <sstream>
#include <string>
#include <iostream>
#include "ApeInterpolator.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"
#include "ApeISystemConfig.h"
#include "ApeINode.h"
#include "ApeIScene.h"
#include "ApeIMainWindow.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeICamera.h"
#include "ApeITextGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeISphereGeometry.h"
#include "ApeIManualPass.h"
#include "ApeITubeGeometry.h"
#include "ApeIPointCloud.h"
#include "ApeIFileGeometry.h"
#include "libfreenect2\libfreenect2.hpp"
#include "libfreenect2\frame_listener_impl.h"
#include "libfreenect2\registration.h"
#include "libfreenect2\packet_pipeline.h"
#include "libfreenect2\logger.h"

#define THIS_PLUGINNAME "ApeMultiKinectPlugin"

namespace Ape
{
	class MultiKinectPlugin : public Ape::IPlugin
	{
	public:
		MultiKinectPlugin();

		~MultiKinectPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		void eventCallBack(const Ape::Event& event);

	private:
		libfreenect2::Freenect2 freenect2;
		libfreenect2::Freenect2Device *device1;
		libfreenect2::Freenect2Device *device2;
		//libfreenect2::PacketPipeline *pipeline;

		std::string serial;
		std::string serial2;

		libfreenect2::Registration* registration = NULL;

		unsigned char sensorNum = 0;//number of kinects used (1 or 2)

		Ape::PointCloudPoints KPts;//stores the actual point coordinates
		Ape::PointCloudColors KCol;//stores the actual point colors
		Ape::PointCloudPoints KPts2;
		Ape::PointCloudColors KCol2;

		float KPos[3] = { 0.0, 0.0, 0.0 };//Point cloud origin position
		float KRot[4] = { 0.0, 0.0, 0.0, 0.0 };//Point cloud origin quaternion rotaton
		float KPos2[3] = { 0.0, 0.0, 0.0 };
		float KRot2[4] = { 0.0, 0.0, 0.0, 0.0 };
		
		bool pointsGenerated = false;
		const unsigned int width = 512;
		const unsigned int height = 424;

		Ape::NodeWeakPtr RootNode;
		Ape::PointCloudWeakPtr mPointCloud;
		Ape::PointCloudWeakPtr mPointCloud2;

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;
	};

	APE_PLUGIN_FUNC Ape::IPlugin* CreateMultiKinectPlugin()
	{
		return new Ape::MultiKinectPlugin;
	}

	APE_PLUGIN_FUNC void DestroyMultiKinectPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::MultiKinectPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateMultiKinectPlugin, DestroyMultiKinectPlugin);
		return 0;
	}
}
#endif