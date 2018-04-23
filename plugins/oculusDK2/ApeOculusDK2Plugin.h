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

#ifndef APE_OCULUSDK2PLUGIN_H
#define APE_OCULUSDK2PLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "OVR.h"
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeITextGeometry.h"
#include "ApeIFileGeometry.h"
#include "ApeIFileMaterial.h"
#include "ApeIManualMaterial.h"
#include "ApeIManualPass.h"
#include "ApeIManualTexture.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeICamera.h"
#include "ApeMatrix4.h"
#include "ApeIMainWindow.h"
#include "ApeISystemConfig.h"

#define THIS_PLUGINNAME "ApeOculusDK2Plugin"

namespace Ape
{
	class ApeOculusDK2Plugin : public Ape::IPlugin
	{
	private:
		Ape::IEventManager* mpEventManager;

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		ovrHmd mpHMD;

		ovrFrameTiming mHMDFrameTiming;

		Ape::CameraWeakPtr mCameraLeft;

		Ape::CameraWeakPtr mCameraRight;

		Ape::NodeWeakPtr mHeadNode;

		Ape::NodeWeakPtr mUserNode;

		void eventCallBack(const Ape::Event& event);

		Ape::Matrix4 conversionFromOVR(ovrMatrix4f ovrMatrix4);

	public:
		ApeOculusDK2Plugin();

		~ApeOculusDK2Plugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC Ape::IPlugin* CreateApeOculusDK2Plugin()
	{
		return new Ape::ApeOculusDK2Plugin;
	}

	APE_PLUGIN_FUNC void DestroyApeOculusDK2Plugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ApeOculusDK2Plugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeOculusDK2Plugin, DestroyApeOculusDK2Plugin);
		return 0;
	}
}

#endif
