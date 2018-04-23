/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#include <iostream>
#include <string>
#include <thread> 
#include <functional>
#include <ctime>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"
#include "ApeISystemConfig.h"
#include "ApeINode.h"
#include "ApeIScene.h"
#include "ApeIMainWindow.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeICamera.h"
#include "ApeITextGeometry.h"
#include "ApeIFileGeometry.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeIManualPass.h"

#define THIS_PLUGINNAME "ApeAssimpAssetLoaderPlugin"

namespace Ape
{
	class AssimpAssetLoaderPlugin : public Ape::IPlugin
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

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		Assimp::Importer* mpAssimpImporter;

		std::vector<const aiScene*> mAssimpScenes;

		std::vector<std::string> mAssimpAssetFileNames;

		int mObjectCount;

		bool mMergeAndExportMeshes;

		float mSceneUnitScale;

		bool mRegenerateNormals;

		Ape::NodeWeakPtr mRootNode;

		int mAssetCount;

		void eventCallBack(const Ape::Event& event);

		void createNode(int assimpSceneID, aiNode* assimpNode);

		void loadConfig();

		void loadScene(const aiScene* assimpScene, int ID);
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateAssimpAssetLoaderPlugin()
	{
		return new AssimpAssetLoaderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyAssimpAssetLoaderPlugin(Ape::IPlugin *plugin)
	{
		delete (AssimpAssetLoaderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateAssimpAssetLoaderPlugin, DestroyAssimpAssetLoaderPlugin);
		return 0;
	}
}

#endif
