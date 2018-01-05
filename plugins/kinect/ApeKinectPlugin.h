/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#ifndef APE_KINECTPLUGIN_H
#define APE_KINECTPLUGIN_H

#include <iostream>
#include <string>
#include <thread> 
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"
#include "ApeISystemConfig.h"
#include "ApeINode.h"
#include "ApeIScene.h"
#include "ApeIMainWindow.h"
#include "ApeIEventManager.h"
#include "ApeICamera.h"
#include "ApeITextGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeISphereGeometry.h"
#include "ApeIManualPass.h"
#include <ApeITubeGeometry.h>
#include <ApeIPointCloud.h>

#define THIS_PLUGINNAME "ApeKinectPlugin"

namespace Ape
{
	class KinectPlugin : public Ape::IPlugin
	{
		static const int        cDepthWidth = 512;
		static const int        cDepthHeight = 424;

	public:
		KinectPlugin();

		~KinectPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		/// <summary>
		/// Gets the different frames from the sensor
		/// </summary>
		void Update();

		void GetBodyData(IMultiSourceFrame* pframe);
		void GetRGBData(IMultiSourceFrame* pframe);
		void GetDepthData(IMultiSourceFrame* pframe);

		/// <summary>
		/// Initializes the default Kinect sensor
		/// </summary>
		/// <returns>indicates success or failure</returns>
		HRESULT Ape::KinectPlugin::InitializeDefaultSensor();

		/// <summary>
		/// Handle new body data
		/// <param name="nTime">timestamp of frame</param>
		/// <param name="nBodyCount">body data count</param>
		/// <param name="ppBodies">body data in frame</param>
		/// </summary>
		void Ape::KinectPlugin::ProcessBody(int nBodyCount, IBody** ppBodies);

	private:
		BOOLEAN Operatorfound[BODY_COUNT] = { false,false,false,false,false,false };

		std::vector<Ape::NodeWeakPtr> _0Body;
		std::vector<Ape::NodeWeakPtr> _1Body;
		std::vector<Ape::NodeWeakPtr> _2Body;
		std::vector<Ape::NodeWeakPtr> _3Body;
		std::vector<Ape::NodeWeakPtr> _4Body;
		std::vector<Ape::NodeWeakPtr> _5Body;

		float body[BODY_COUNT][JointType_Count][3];//stores the detected joint coordinates

		// Current Kinect
		IKinectSensor*          m_pKinectSensor;
		ICoordinateMapper*      m_pCoordinateMapper;

		IMultiSourceFrame* pFrame;
		IMultiSourceFrameReader* reader;   // Kinect data source

		const int colorwidth = 1920;
		const int colorheight = 1080;

		int size = 651264; //number of coordinates in point cloud
		int CloudSize;
		bool pointsGenerated = false;
		double pointratio = 1;

		Ape::PointCloudPoints KPts;//stores the actual point coordinates
		Ape::PointCloudColors KCol;//stores the actual point colors

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		void eventCallBack(const Ape::Event& event);

		Ape::NodeWeakPtr mLeftHandNode;

		Ape::NodeWeakPtr mRightHandNode;

		Ape::PointCloudWeakPtr mPointCloud;
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateKinectPlugin()
	{
		return new KinectPlugin;
	}

	APE_PLUGIN_FUNC void DestroyKinectPlugin(Ape::IPlugin *plugin)
	{
		delete (KinectPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateKinectPlugin, DestroyKinectPlugin);
		return 0;
	}
}

#endif
