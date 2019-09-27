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

#ifndef APE_360STEREOIMAGEPLUGIN_H
#define APE_360STEREOIMAGEPLUGIN_H

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "plugin/apePluginAPI.h"
#include "managers/apeILogManager.h"
#include "managers/apeIEventManager.h"
#include "managers/apeISceneManager.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIManualTexture.h"
#include "macros/userInput/apeUserInputMacro.h"

#define THIS_PLUGINNAME "ape360StereoImagePlugin"

namespace ape
{
	class ape360StereoImagePlugin : public ape::IPlugin
	{
	private:
		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::UserInputMacro* mpApeUserInputMacro;

	public:
		ape360StereoImagePlugin();

		~ape360StereoImagePlugin();

		void createSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility);

		void eventCallBack(const ape::Event& event);

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* Createape360StereoImagePlugin()
	{
		return new ape::ape360StereoImagePlugin;
	}

	APE_PLUGIN_FUNC void Destroyape360StereoImagePlugin(ape::IPlugin *plugin)
	{
		delete (ape::ape360StereoImagePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, Createape360StereoImagePlugin, Destroyape360StereoImagePlugin);
		return 0;
	}
}

#endif
