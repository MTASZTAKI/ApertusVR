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

#ifndef APE_PHYSICSSIMULATIONPLUGIN_H
#define APE_PHYSICSSIMULATIONPLUGIN_H

/// std lib
#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include "plugin/apePluginAPI.h"

/// managers
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeISceneNetwork.h"
#include "managers/apeICoreConfig.h"

/// sceneelements
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeIIndexedFaceSetGeometry.h"
#include "sceneelements/apeIIndexedLineSetGeometry.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeIPbsPass.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeIPointCloud.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneelements/apeITubeGeometry.h"
#include "sceneelements/apeICloneGeometry.h"
#include "sceneelements/apeIBoxGeometry.h"
#include "sceneelements/apeIRigidBody.h"
#include "sceneElements/apeICylinderGeometry.h"

/// utils, macros
#include "utils/apeInterpolator.h"
#include "macros/sceneMaker/apeSceneMakerMacro.h"
#include "macros/userInput/ApeUserInputMacro.h"

#define THIS_PLUGINNAME "apePhysicsSimulationScenePlugin"

namespace ape
{

	class apePhysicsSimulationScenePlugin : public ape::IPlugin
	{
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		void eventCallBack(const ape::Event& event);

		ape::PointCloudWeakPtr mPointCloud;

		ape::UserInputMacro* mpUserInputMacro;

	public:
		apePhysicsSimulationScenePlugin();

		~apePhysicsSimulationScenePlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:

		void makeTerrain(std::string name, ape::Vector3 scale);

		void makeGround(std::string name, ape::Vector2 size, float height);

		void makeWater(std::string name, ape::Vector2 size, ape::Vector3 pos);

		void makeBox(std::string name, ape::Vector3 dims, ape::Vector3 pos);
		
		void makeSphere(std::string name, float radius, ape::Vector3 pos);

		void makeCone(std::string name, float radius, float height, ape::Vector3 pos);

		void makeCylinder(std::string name, float radius, float height, ape::Vector3 pos);

		bool m_waterEnabled;

		bool m_groundIsWavy;

		std::vector<RigidBodyWeakPtr> m_bodies;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateApePhysicsSimulationPlugin()
	{
		return new ape::apePhysicsSimulationScenePlugin;
	}

	APE_PLUGIN_FUNC void DestroyApePhysicsSimulationPlugin(ape::IPlugin *plugin)
	{
		delete (ape::apePhysicsSimulationScenePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateApePhysicsSimulationPlugin, DestroyApePhysicsSimulationPlugin);
		return 0;
	}

}

#endif
