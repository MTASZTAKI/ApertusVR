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

#ifndef APE_TEXASEEPGLUGIN_H
#define APE_TEXASEEPGLUGIN_H

#include <chrono>
#include <deque>
#include <iostream>
#include <math.h>
#include <memory>
#include <thread>
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "datatypes/apeEuler.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeITextGeometry.h"
#include "macros/userInput/apeUserInputMacro.h"
#include "GameManager.h"

#define THIS_PLUGINNAME "apeTexasEEGPlugin"

namespace ape
{
	class apeTexasEEGPlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		void eventCallBack(const ape::Event& event);

		ape::ICoreConfig* mpCoreConfig;

		int mBubblesActivateNextCount;

		int mScore;

		ape::EntityWeakPtr mStatusText;

		ape::EntityWeakPtr mScoreText;

		TexasEEG::GameManager* mGameManager;

		TexasEEG::BubbleManager* mBubbleManager;

		ape::UserInputMacro* mpapeUserInputMacro;

	public:
		apeTexasEEGPlugin();

		~apeTexasEEGPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		void UpdateScore(int score);
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeTexasEEGPlugin()
	{
		return new ape::apeTexasEEGPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeTexasEEGPlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeTexasEEGPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeTexasEEGPlugin, DestroyapeTexasEEGPlugin);
		return 0;
	}
}

#endif
