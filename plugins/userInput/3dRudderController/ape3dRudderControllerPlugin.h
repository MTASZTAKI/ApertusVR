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

#ifndef APE_3DRUDDERCONTROLLERPLUGIN_H
#define APE_3DRUDDERCONTROLLERPLUGIN_H

#include <iostream>
#include <list>
#include <string>
#include <thread>
#define _3DRUDDER_SDK_STATIC
#include "3DRudderSDK.h"
#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeUserInputMacro.h"

#define THIS_PLUGINNAME "ape3dRudderControllerPlugin"

class CEvent : public ns3dRudder::IEvent
{
	void OnConnect(uint32_t nDeviceNumber)
	{
		APE_LOG_DEBUG("OnConnect: " << nDeviceNumber);
	}
	void OnDisconnect(uint32_t nDeviceNumber)
	{
		APE_LOG_DEBUG("OnDisconnect: " << nDeviceNumber);
	}
};

namespace ape
{
	class ape3dRudderControllerPlugin : public ape::IPlugin
	{
	public:
		ape3dRudderControllerPlugin();

		~ape3dRudderControllerPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:

		ape::ISceneManager* mpSceneManager;

		ape::UserInputMacro* mpUserInputMacro;

		ape::ICoreConfig* mpCoreConfig;

		ape::IEventManager* mpEventManager;

		ns3dRudder::CSdk* mpSdk;

		CEvent mEvent;

		ns3dRudder::ErrorCode mErrCodeLoad;

		ns3dRudder::AxesParamDefault mAxesParamDefault;

		ns3dRudder::AxesParamNormalizedLinear mAxesParamNormalizedLinear;

		ns3dRudder::ErrorCode mErrCodeGetAxes;

		ape::Vector3 mUserDeadZone;

		void eventCallBack(const ape::Event& event);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* Create3dRudderControllerPlugin()
	{
		return new ape::ape3dRudderControllerPlugin;
	}

	APE_PLUGIN_FUNC void Destroy3dRudderControllerPlugin(ape::IPlugin *plugin)
	{
		delete (ape::ape3dRudderControllerPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, Create3dRudderControllerPlugin, Destroy3dRudderControllerPlugin);
		return 0;
	}
}

#endif
