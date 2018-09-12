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

#ifndef APE_KINECTPLUGIN_H
#define APE_KINECTPLUGIN_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <strsafe.h>
#include <thread>
#include "stdafx.h"
#include "resource.h"
#include "system/ApeIMainWindow.h"
#include "plugin/ApeIPlugin.h"
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "managers/ApeISystemConfig.h"
#include "sceneelements/ApeICamera.h"
#include "sceneelements/ApeIFileGeometry.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeIManualPass.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeIPointCloud.h"
#include "sceneelements/ApeISphereGeometry.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeITubeGeometry.h"
#include "utils/ApeInterpolator.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

#define THIS_PLUGINNAME "ApeKinectPlugin"

//#define operatortest

namespace Ape
{
	enum ScanningState {
		WAITING,
		FIRST_DONE,
		SECOND_DONE
	};

	class KinectPlugin : public Ape::IPlugin
	{
		static const int        cDepthWidth = 512;
		static const int        cDepthHeight = 424;
		static const int		cBodyCount = 6;

	public:
		KinectPlugin();

		~KinectPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		void Update();

		void GetBodyData(IMultiSourceFrame* pframe);
		void GetBodyIndexes(IMultiSourceFrame* pframe);
		void GetDepthData(IMultiSourceFrame* pframe);
		void GetRGBData(IMultiSourceFrame* pframe);

		HRESULT InitializeDefaultSensor();

		void GetOperator();
		void GetOperatorPts(); 
		void GetOperatorColrs();
		float GetDistance(std::vector<float> joint, std::vector<float> point);

		void ScanOperator();
		bool CheckAngles(int index);
		float AngleBetween(Vector3 p1, Vector3 p2);
		bool CheckDifference(float data, float ref, float perc);
		bool CheckMaxDifference(float data, float ref, float maxdev);

		void ProcessBody(int nBodyCount, IBody** ppBodies);

	private:
		BOOLEAN Operatorfound[BODY_COUNT] = { false,false,false,false,false,false };
		Ape::NodeWeakPtr RootNode;
		std::vector<Ape::NodeWeakPtr> _0Body;
		std::vector<Ape::NodeWeakPtr> _1Body;
		std::vector<Ape::NodeWeakPtr> _2Body;
		std::vector<Ape::NodeWeakPtr> _3Body;
		std::vector<Ape::NodeWeakPtr> _4Body;
		std::vector<Ape::NodeWeakPtr> _5Body;

		float body[cBodyCount][JointType_Count][3];//stores the detected joint coordinates

		// Current Kinect
		IKinectSensor*          m_pKinectSensor;
		ICoordinateMapper*      m_pCoordinateMapper;

		IMultiSourceFrameReader* reader;   // Kinect data source

		const int colorwidth = 1920;
		const int colorheight = 1080;

		const int size = 651264; //number of coordinates in point cloud
		unsigned int CloudSize;
		bool pointsGenerated = false;
		double pointratio = 1;

		Ape::PointCloudPoints KPts;//stores the actual point coordinates
		Ape::PointCloudColors KCol;//stores the actual point colors

		Ape::PointCloudPoints OperatorPoints;
		Ape::PointCloudColors OperatorColors;

		Ape::PointCloudPoints ScannedPoints;
		Ape::PointCloudColors ScannedColors;

		float KPos[3] = { 0.0, 0.0, 0.0 };//Point cloud origin position
		float KRot[4] = { 0.0, 0.0, 0.0, 0.0 };//Point cloud origin quaternion rotaton

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		Ape::NodeWeakPtr mClothNode;

		void eventCallBack(const Ape::Event& event);

		Ape::PointCloudWeakPtr mPointCloud;
		Ape::PointCloudWeakPtr mOperatorPointCloud;

		bool _1Detected = false;
		bool operatorPointsGenerated = false;
		bool halfscan = false;
		std::vector<int> indexes;

		unsigned int framecount = 0;
		unsigned int scanframe = 0;

		bool showSkeleton=false;
		bool backgroundRemoval = false;
		bool maxFPS = false;
		bool _3dScan = false;

		ScanningState sstate = WAITING;
		Degree turnang = 175.0f;
		Matrix3 turnaraound = Matrix3(cos(turnang.toRadian()), 0.0f, sin(turnang.toRadian()),
			0.0f, 1.0f, 0.0f,
			-sin(turnang.toRadian()), 0.0f, cos(turnang.toRadian()));//rotate around Y-axis
		Vector3 spoint1;
		Vector3 spoint2;
		Vector3 anchor;
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateKinectPlugin()
	{
		return new Ape::KinectPlugin;
	}

	APE_PLUGIN_FUNC void DestroyKinectPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::KinectPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateKinectPlugin, DestroyKinectPlugin);
		return 0;
	}
}

#endif
