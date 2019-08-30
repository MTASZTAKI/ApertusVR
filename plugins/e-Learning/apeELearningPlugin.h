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

#ifndef APE_ELEARNINGPLUGIN_H
#define APE_ELEARNINGPLUGIN_H

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "plugin/apePluginAPI.h"
#include "managers/apeILogManager.h"
#include "managers/apeIEventManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIManualTexture.h"
#include "sceneelements/apeIFileTexture.h"
#include "sceneelements/apeIManualMaterial.h"
#include "macros/userInput/apeUserInputMacro.h"
#include "macros/sceneMaker/apeSceneMakerMacro.h"
#include "apeELearningPluginConfig.h"

#define THIS_PLUGINNAME "apeELearningPlugin"

namespace ape
{
	class apeELearningPlugin : public ape::IPlugin
	{
	private:
		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::UserInputMacro* mpApeUserInputMacro;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		ape::ICoreConfig* mpCoreConfig;

		std::map<std::string, quicktype::Hotspot> mNodeNamesHotSpots;

		std::vector<quicktype::Room> mRooms;

		std::map<std::string, std::string> mGameURLResourcePath;

		ape::Vector3 mUserDeadZone;

		ape::FileGeometryWeakPtr mSphereGeometryLeft;

		ape::FileGeometryWeakPtr mSphereGeometryRight;

		ape::Vector2 mMouseMovedValueAbs;

		int mMouseScrolledValue;

		int mCurrentRoomID;

		ape::UserInputMacro::OverlayBrowserCursor mOverlayBrowserCursor;

	public:
		apeELearningPlugin();

		~apeELearningPlugin();

		ape::FileGeometryWeakPtr createSphere(std::string cameraName, std::string sphereNodeName, std::string meshName, unsigned int visibility);

		void createRoomTextures();

		void createHotSpots();

		void createOverlayBrowser();

		void loadNextRoom();

		void loadHotSpots();

		void loadRoomTextures();

		void resetUserNodePose();

		void resetHeadNodePose();

		void eventCallBack(const ape::Event& event);

		void keyPressedStringEventCallback(const std::string& keyValue);

		void mousePressedStringEventCallback(const std::string& keyValue);

		void mouseReleasedStringEventCallback(const std::string& keyValue);

		void mouseMovedCallback(const ape::Vector2& mouseMovedValueRel, const ape::Vector2& mouseMovedValueAbs);

		void mouseScrolledCallback(const int& mouseScrolledValue);

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeELearningPlugin()
	{
		return new ape::apeELearningPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeELearningPlugin(ape::IPlugin *plugin)
	{
		delete (ape::apeELearningPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeELearningPlugin, DestroyapeELearningPlugin);
		return 0;
	}
}

#endif
