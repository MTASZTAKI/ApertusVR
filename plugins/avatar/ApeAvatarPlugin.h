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

#ifndef APE_AVATARPLUGIN_H
#define APE_AVATARPLUGIN_H

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIManualPass.h"
#include "sceneelements/apeIManualTexture.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeITextGeometry.h"
#include "utils/apeInterpolator.h"

#define THIS_PLUGINNAME "apeAvatarPlugin"

namespace ape
{
	class apeAvatarPlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		void eventCallBack(const ape::Event& event);

		ape::ICoreConfig* mpCoreConfig;

		ape::NodeWeakPtr mLeftHandNode;

		ape::NodeWeakPtr mRightHandNode;

	public:
		apeAvatarPlugin();

		~apeAvatarPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeAvatarPlugin()
	{
		return new ape::apeAvatarPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeAvatarPlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeAvatarPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeAvatarPlugin, DestroyapeAvatarPlugin);
		return 0;
	}
}

#endif
