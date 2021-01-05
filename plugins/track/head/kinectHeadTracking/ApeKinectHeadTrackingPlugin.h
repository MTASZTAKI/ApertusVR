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
#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeICamera.h"
#include "apeIFileGeometry.h"
#include "apeIManualMaterial.h"
#include "apeINode.h"
#include "apeIPointCloud.h"
#include "apeISphereGeometry.h"
#include "apeITextGeometry.h"
#include "apeITubeGeometry.h"
#include "apeDoubleQueue.h"
#include "apeInterpolator.h"
#include "apeHeadTrackingConfigs.h"
#include "apeEuler.h"
#include "apeUserInputMacro.h"

#define THIS_PLUGINNAME "apeKinectHeadTrackingPlugin"

namespace ape
{
	class KinectHeadTrackingPlugin : public ape::IPlugin
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

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::IEventManager* mpEventManager;

		ape::DoubleQueue<ape::CameraWeakPtr> mCameraDoubleQueue;

		ape::HeadTrackerConfig mTrackerConfig;

		ape::HeadTrackerDisplayConfigList mDisplayConfigList;

		ape::Vector3 mTrackedViewerPosition;

		ape::Vector3 mTrackedPrevViewerPosition;

		ape::Quaternion mTrackedViewerOrientation;

		ape::Euler mTrackedViewerOrientationYPR;

		float mNearClip;

		float mFarClip;

		float mC;

		float mD;

		int mCameraCount;

		ape::UserInputMacro* mpapeUserInputMacro;

		void eventCallBack(const ape::Event& event);

		void setCameraConfigByName(std::string cameraName, ape::CameraWeakPtr cameraWkPtr);

		ape::Matrix4 calculateCameraProjection(ape::HeadTrackerDisplayConfig& displayConfig, ape::Vector3& trackedEyePosition);

		ape::Matrix4 perspectiveOffCenter(float& displayDistanceLeft, float& displayDistanceRight, float& displayDistanceBottom, float& displayDistanceTop);

		HRESULT InitializeDefaultSensor();

		void getHeadPositionFromBodyData(IBody* pBody);

		void getBodyDataFromSensor(IMultiSourceFrame * pframe);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateKinectHeadTrackingPlugin()
	{
		return new ape::KinectHeadTrackingPlugin;
	}

	APE_PLUGIN_FUNC void DestroyKinectHeadTrackingPlugin(ape::IPlugin *plugin)
	{
		delete (ape::KinectHeadTrackingPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateKinectHeadTrackingPlugin, DestroyKinectHeadTrackingPlugin);
		return 0;
	}
}

#endif
