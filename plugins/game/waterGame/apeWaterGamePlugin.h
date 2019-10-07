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

#ifndef APE_WaterGamePLUGIN_H
#define APE_WaterGamePLUGIN_H

#include <chrono>
#include <deque>
#include <iostream>
#include <math.h>
#include <memory>
#include <thread>
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeEuler.h"
#include "apeICamera.h"
#include "apeIFileGeometry.h"
#include "apeIFileMaterial.h"
#include "apeILight.h"
#include "apeINode.h"
#include "apeITextGeometry.h"
#include "apeUserInputMacro.h"
#include "GameManager.h"
#include "apeIRigidBody.h"

#define THIS_PLUGINNAME "apeWaterGamePlugin"

namespace ape
{

	class apeWaterGamePlugin : public ape::IPlugin
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

		ape::NodeWeakPtr mUserBodyNode;

		WaterGame::GameManager* mGameManager;

		WaterGame::BubbleManager* mBubbleManager;

		ape::UserInputMacro* mpapeUserInputMacro;

		ape::UserInputMacro::ViewPose mUserInputMacroPose;

	public:
		apeWaterGamePlugin();

		~apeWaterGamePlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		void UpdateScore(int score);
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateApeWaterGamePlugin()
	{
		return new ape::apeWaterGamePlugin;
	}

	APE_PLUGIN_FUNC void DestroyApeWaterGamePlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeWaterGamePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateApeWaterGamePlugin, DestroyApeWaterGamePlugin);
		return 0;
	}

}

#endif
