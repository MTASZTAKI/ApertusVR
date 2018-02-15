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


#ifndef APE_FOBHEADTRACKINGPLUGIN_H
#define APE_FOBHEADTRACKINGPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeICamera.h"
#include "ApeISystemConfig.h"
#include "ApeIMainWindow.h"
#include "ApeFobHeadTracking.h"
#include "ApeEuler.h"
#include "ApeDoubleQueue.h"
#include "ApeFobHeadTrackingPluginConfigs.h"
#include "ApeMatrix4.h"

#define THIS_PLUGINNAME "ApeFobHeadTrackingPlugin"

class ApeFobHeadTrackingPlugin : public Ape::IPlugin
{
private:
	int mCameraCount;

	Ape::IEventManager* mpEventManager;

	Ape::IScene* mpScene;

	Ape::NodeWeakPtr mUserNode;

	Ape::NodeWeakPtr mCamerasNode;

	Ape::DoubleQueue<Ape::CameraWeakPtr> mCameraDoubleQueue;

	Ape::ISystemConfig* mpSystemConfig;

	Ape::IMainWindow* mpMainWindow;

	Ape::FobHeadTrackingTrackerConfig mTrackerConfig;

	Ape::FobHeadTrackingDisplayConfigList mDisplayConfigList;

	Ape::Vector3 mTrackedViewerPosition;

	Ape::Vector3 mTrackedPrevViewerPosition;

	Ape::Quaternion mTrackedViewerOrientation;

	Ape::Euler mTrackedViewerOrientationYPR;

	void* mpFobTracker;
	
	void eventCallBack(const Ape::Event& event);

	void setCameraConfigByName(std::string cameraName, Ape::CameraWeakPtr cameraWkPtr);

	Ape::Matrix4 calculateCameraProjection(std::string name, Ape::Vector3 displayBottomLeftCorner, Ape::Vector3 displayBottomRightCorner, Ape::Vector3 displayTopLeftCorner,
		Ape::Vector3 trackedViewerPosition, float cameraNearClip, float cameraFarClip);

	Ape::Matrix4 perspectiveOffCenter(float displayDistanceLeft, float displayDistanceRight, float displayDistanceBottom, float displayDistanceTop, float cameraNearClip, float cameraFarClip);

public:
	ApeFobHeadTrackingPlugin();

	~ApeFobHeadTrackingPlugin();
	
	void Init() override;

	void Run() override;

	void Step() override;

	void Stop() override;

	void Suspend() override;

	void Restart() override;
};

APE_PLUGIN_FUNC Ape::IPlugin* CreateApeFobHeadTrackingPlugin()
{
	return new ApeFobHeadTrackingPlugin;
}

APE_PLUGIN_FUNC void DestroyApeFobHeadTrackingPlugin(Ape::IPlugin *plugin)
{
	delete (ApeFobHeadTrackingPlugin*)plugin;
}

APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

APE_PLUGIN_ALLOC()
{
	std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
	ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeFobHeadTrackingPlugin, DestroyApeFobHeadTrackingPlugin);
	return 0;
}

#endif
