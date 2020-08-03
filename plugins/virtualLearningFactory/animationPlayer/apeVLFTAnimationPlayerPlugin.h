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

#ifndef APE_VLFTANIMATIONPLAYERPLUGIN_H
#define APE_VLFTANIMATIONPLAYERPLUGIN_H

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <functional>
#include <fstream>
#include "apePluginAPI.h"
#include "apeILogManager.h"
#include "apeIEventManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeICloneGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIPlaneGeometry.h"
#include "apeIFileMaterial.h"
#include "apeIRayGeometry.h"
#include "apeIBrowser.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeINode.h"
#include "apeICamera.h"
#include "apeIManualTexture.h"
#include "apeIFileTexture.h"
#include "apeIManualMaterial.h"
#include "apeUserInputMacro.h"
#include "apeSceneMakerMacro.h"
#include "apeEuler.h"
#include "apeVLFTAnimationPlayerPluginConfig.h"

#define THIS_PLUGINNAME "apeVLFTAnimationPlayerPlugin"

namespace ape
{
	class VLFTAnimationPlayerPlugin : public ape::IPlugin
	{
	public:
		struct Animation
		{
			quicktype::EventType type;
			std::string nodeName;
			std::string parentNodeName;
			unsigned long long time;
			ape::Vector3 position;
			ape::Quaternion orientation;
			std::string modelName;
			ape::Vector3 translate;
			ape::Degree rotationAngle;
			ape::Vector3 rotationAxis;
			std::string url;
			std::string descr;
			std::string fileName;
			bool trail;
		};
	private:
		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::UserInputMacro* mpUserInputMacro;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		ape::ICoreConfig* mpCoreConfig;

		quicktype::Animations mAnimations;

		std::thread mAnimationThread;

		std::vector<Animation> mParsedAnimations;

		std::vector<std::string> mAnimatedNodeNames;

		std::vector<std::string> mSpaghettiNodeNames;

		std::vector<ape::NodeWeakPtr> mAttachedUsers;

		std::vector<unsigned long long> mParsedBookmarkTimes;

		int mBookmarkID;

		ape::NodeWeakPtr mClickedNode;

		float mTimeToSleepFactor;

		bool mIsPauseClicked;

		bool mIsStopClicked;

		bool mIsPlayRunning;

		bool mIsStudentsMovementLogging;

		bool mIsAllSpaghettiVisible;

		std::vector<ape::NodeWeakPtr> mStudents;

		std::ofstream mStudentsMovementLoggingFile;

		PROCESS_INFORMATION mScreenCastProcessInfo;

		DWORD mScreenCastProcessExitCode;

		unsigned long long mClickedBookmarkTime;

		int mChoosedBookmarkedAnimationID;

	public:
		VLFTAnimationPlayerPlugin();

		~VLFTAnimationPlayerPlugin();

		void playAnimation();

		void showSpaghetti(std::string name, bool show);

		void showAllSpaghetti(bool show);

		void drawSpaghettiSection(const ape::Vector3& startPosition, const ape::NodeSharedPtr& node, std::string& spaghettiSectionName);

		bool attach2NewAnimationNode(const std::string& parentNodeName, const ape::NodeSharedPtr& node);

		void startPlayAnimationThread();

		void screenCast();

		void sendAnimationTimeDuringSleep(unsigned long long startTime, unsigned long long stopTime);

		void eventCallBack(const ape::Event& event);
		
		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeVLFTAnimationPlayerPlugin()
	{
		return new ape::VLFTAnimationPlayerPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeVLFTAnimationPlayerPlugin(ape::IPlugin *plugin)
	{
		delete (ape::VLFTAnimationPlayerPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeVLFTAnimationPlayerPlugin, DestroyapeVLFTAnimationPlayerPlugin);
		return 0;
	}
}

#endif
