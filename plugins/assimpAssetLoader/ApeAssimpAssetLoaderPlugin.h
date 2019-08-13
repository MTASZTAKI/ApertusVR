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

#ifndef APE_ASSIMPASSETLOADERPLUGIN_H
#define APE_ASSIMPASSETLOADERPLUGIN_H

#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include "plugin/apeIPlugin.h"
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileTexture.h"
#include "sceneelements/apeIIndexedFaceSetGeometry.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneElements/apeIRigidBody.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define THIS_PLUGINNAME "apeAssimpAssetLoaderPlugin"

namespace ape
{
	struct AssetConfig
	{
		std::string file;
		ape::Vector3 scale;
		ape::Vector3 position;
		ape::Quaternion orientation;
		bool mergeAndExportMeshes;
		bool regenerateNormals;
		std::string rootNodeName;
	};

	struct PhysicsConfig
	{
		float mass;
		float friction;
		float rollingFriction;
		float spinningFriction;
		float linearDamping;
		float angularDamping;
		float restitution;
		bool bouyancyEnable;
	};

	class AssimpAssetLoaderPlugin : public ape::IPlugin
	{
	public:
		AssimpAssetLoaderPlugin();

		~AssimpAssetLoaderPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::IEventManager* mpEventManager;

		Assimp::Importer* mpAssimpImporter;

		std::vector<const aiScene*> mAssimpScenes;

		std::vector<AssetConfig> mAssimpAssetConfigs;

		std::string mUniqueID;

		std::vector<PhysicsConfig> mPhysicsConfigs;

		int mObjectCount;

		int mAssetCount;

		std::mutex mMutex;

		void eventCallBack(const ape::Event& event);

		void createNode(int assimpSceneID, aiNode* assimpNode);

		void loadConfig();

		void readFile(std::string fileName);

		void loadScene(const aiScene* assimpScene, int ID);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateAssimpAssetLoaderPlugin()
	{
		return new ape::AssimpAssetLoaderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyAssimpAssetLoaderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::AssimpAssetLoaderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateAssimpAssetLoaderPlugin, DestroyAssimpAssetLoaderPlugin);
		return 0;
	}
}

#endif
