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

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "datatypes/apeMatrix4.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeIIndexedFaceSetGeometry.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIManualPass.h"
#include "sceneelements/apeIManualTexture.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeITextGeometry.h"
#include "macros/userInput/apeUserInputMacro.h"
#include "openvr.h"
#include "openvr_capi.h"


#define THIS_PLUGINNAME "apeHtcVivePlugin"

namespace ape
{
	class apeHtcVivePlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::CameraWeakPtr mCamera;

		ape::CameraWeakPtr mCameraLeft;

		ape::CameraWeakPtr mCameraRight;

		ape::ManualTextureWeakPtr mManualTextureRightEye;
		
		ape::ManualTextureWeakPtr mManualTextureLeftEye;

		vr::IVRSystem* mpOpenVrSystem;

		vr::Texture_t mOpenVrTextures[2];

		void* mOpenVrRttTextureIDs[2];

		std::array<vr::VRTextureBounds_t, 2> mOpenVrTextureBounds;

		vr::HmdError mOpenVrHmdError;

		vr::TrackedDevicePose_t mOpenVrTrackedPoses[vr::k_unMaxTrackedDeviceCount];

		void eventCallBack(const ape::Event& event);

		ape::Matrix4 conversionFromOpenVR(vr::HmdMatrix34_t ovrMatrix34);

		ape::Matrix4 conversionFromOpenVR(vr::HmdMatrix44_t ovrMatrix44);

		void submitTextureLeftToOpenVR();

		void submitTextureRightToOpenVR();

		ape::UserInputMacro* mpApeUserInputMacro;

		ape::UserInputMacro::ViewPose mUserInputMacroPose;

	public:
		apeHtcVivePlugin();

		~apeHtcVivePlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeHtcVivePlugin()
	{
		return new ape::apeHtcVivePlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeHtcVivePlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeHtcVivePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeHtcVivePlugin, DestroyapeHtcVivePlugin);
		return 0;
	}
}

#endif
