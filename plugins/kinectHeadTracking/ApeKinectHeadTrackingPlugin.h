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

#ifndef APE_KINECTHEADTRACKINGPLUGIN_H
#define APE_KINECTHEADTRACKINGPLUGIN_H

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
#include "utils/ApeDoubleQueue.h"
#include "utils/ApeInterpolator.h"
#include "utils/ApeHeadTrackingConfigs.h"
#include "datatypes/ApeEuler.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

#define THIS_PLUGINNAME "ApeKinectHeadTrackingPlugin"

namespace Ape
{
	class KinectHeadTrackingPlugin : public Ape::IPlugin
	{
	public:
		KinectHeadTrackingPlugin();

		~KinectHeadTrackingPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:
		IKinectSensor* mpKinectSensor;

		ICoordinateMapper* mpCoordinateMapper;

		IMultiSourceFrameReader* mpKinectReader;   

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		Ape::ManualMaterialWeakPtr mUserMaterial;

		Ape::NodeWeakPtr mHeadNode;

		Ape::DoubleQueue<Ape::CameraWeakPtr> mCameraDoubleQueue;

		Ape::HeadTrackerConfig mTrackerConfig;

		Ape::HeadTrackerDisplayConfigList mDisplayConfigList;

		Ape::Vector3 mTrackedViewerPosition;

		Ape::Vector3 mTrackedPrevViewerPosition;

		Ape::Quaternion mTrackedViewerOrientation;

		Ape::Euler mTrackedViewerOrientationYPR;

		float mNearClip;

		float mFarClip;

		float mC;

		float mD;

		int mCameraCount;

		void eventCallBack(const Ape::Event& event);

		void setCameraConfigByName(std::string cameraName, Ape::CameraWeakPtr cameraWkPtr);

		Ape::Matrix4 calculateCameraProjection(Ape::HeadTrackerDisplayConfig& displayConfig, Ape::Vector3& trackedEyePosition);

		Ape::Matrix4 perspectiveOffCenter(float& displayDistanceLeft, float& displayDistanceRight, float& displayDistanceBottom, float& displayDistanceTop);

		HRESULT InitializeDefaultSensor();

		void getHeadPositionFromBodyData(IBody* pBody);

		void getBodyDataFromSensor(IMultiSourceFrame * pframe);
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateKinectHeadTrackingPlugin()
	{
		return new Ape::KinectHeadTrackingPlugin;
	}

	APE_PLUGIN_FUNC void DestroyKinectHeadTrackingPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::KinectHeadTrackingPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateKinectHeadTrackingPlugin, DestroyKinectHeadTrackingPlugin);
		return 0;
	}
}

#endif
