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

#ifndef APE_HTCVIVEPLUGIN_H
#define APE_HTCVIVEPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "openvr.h"
#include "openvr_capi.h"
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeITextGeometry.h"
#include "ApeIFileGeometry.h"
#include "ApeIConeGeometry.h"
#include "ApeIFileMaterial.h"
#include "ApeIManualMaterial.h"
#include "ApeIManualPass.h"
#include "ApeIManualTexture.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeICamera.h"
#include "ApeMatrix4.h"
#include "ApeIMainWindow.h"
#include "ApeIManualMaterial.h"
#include "ApeISystemConfig.h"

#define THIS_PLUGINNAME "ApeHtcVivePlugin"

namespace Ape
{
	class ApeHtcVivePlugin : public Ape::IPlugin
	{
	private:
		Ape::IEventManager* mpEventManager;

		Ape::IScene* mpScene;

		Ape::IMainWindow* mpMainWindow;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::CameraWeakPtr mCameraLeft;

		Ape::CameraWeakPtr mCameraRight;

		Ape::NodeWeakPtr mHeadNode;

		Ape::NodeWeakPtr mUserNode;

		Ape::ManualMaterialWeakPtr mUserMaterial;

		Ape::ManualTextureWeakPtr mManualTextureRightEye;
		
		Ape::ManualTextureWeakPtr mManualTextureLeftEye;

		vr::IVRSystem* mpOpenVrSystem;

		vr::Texture_t mOpenVrTextures[2];

		void* mOpenVrRttTextureIDs[2];

		std::array<vr::VRTextureBounds_t, 2> mOpenVrTextureBounds;

		vr::HmdError mOpenVrHmdError;

		vr::TrackedDevicePose_t mOpenVrTrackedPoses[vr::k_unMaxTrackedDeviceCount];

		void eventCallBack(const Ape::Event& event);

		Ape::CameraWeakPtr createCamera(std::string name);

		Ape::Matrix4 conversionFromOpenVR(vr::HmdMatrix34_t ovrMatrix34);

		Ape::Matrix4 conversionFromOpenVR(vr::HmdMatrix44_t ovrMatrix44);

		void submitTextureLeftToOpenVR();

		void submitTextureRightToOpenVR();

	public:
		ApeHtcVivePlugin();

		~ApeHtcVivePlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC Ape::IPlugin* CreateApeHtcVivePlugin()
	{
		return new Ape::ApeHtcVivePlugin;
	}

	APE_PLUGIN_FUNC void DestroyApeHtcVivePlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ApeHtcVivePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeHtcVivePlugin, DestroyApeHtcVivePlugin);
		return 0;
	}
}

#endif
