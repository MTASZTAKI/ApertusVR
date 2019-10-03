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

#include "apePluginAPI.h"

/// managers
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeISceneNetwork.h"
#include "apeICoreConfig.h"

/// sceneelements
#include "apeICamera.h"
#include "apeIConeGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIFileMaterial.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeILight.h"
#include "apeIManualMaterial.h"
#include "apeINode.h"
#include "apeIPlaneGeometry.h"
#include "apeIPointCloud.h"
#include "apeISphereGeometry.h"
#include "apeITextGeometry.h"
#include "apeITubeGeometry.h"
#include "apeICloneGeometry.h"
#include "apeIBoxGeometry.h"
#include "apeIRigidBody.h"
#include "apeICylinderGeometry.h"
#include "apeIWater.h"

/// utils, macros
#include "apeInterpolator.h"
#include "apeSceneMakerMacro.h"
#include "apeUserInputMacro.h"


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

		struct AssetConfig
		{
			std::string name;
			std::string shape;
			float height;
			float radius;
			ape::Vector3 dims;
			ape::Vector3 pos;
			ape::Quaternion orient;
			ape::Color color;
		};

		void makeTerrain(std::string name, ape::Vector3 scale);

		void makeGround(std::string name, ape::Vector2 size, float height);

		void makeWater(std::string name, ape::Vector3 size, ape::Vector3 pos);

		void makeBox(std::string name, ape::Vector3 dims, ape::Vector3 pos, ape::Quaternion orient, ape::Color color);
		
		void makeSphere(std::string name, float radius, ape::Vector3 pos, ape::Quaternion orient, ape::Color color);

		void makeCone(std::string name, float radius, float height, ape::Vector3 pos, ape::Quaternion orient, ape::Color color);

		void makeCylinder(std::string name, float radius, float height, ape::Vector3 pos, ape::Quaternion orient, ape::Color color);

		void makeCubeArray();

		void makeSphereArray();

		bool m_water1;

		bool m_water2;

		bool m_ground;

		bool m_terrain;

		bool m_cubes;

		bool m_spheres;

		int m_cubesArraySize[3];

		int m_spheresArraySize[3];

		ape::Vector3 m_cubesInitPos;

		ape::Vector3 m_spheresInitPos;

		std::vector<AssetConfig> m_assets;

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
