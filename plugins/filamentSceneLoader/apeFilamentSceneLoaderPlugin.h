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

#ifndef APE_FILAMENTSCENELOADERPLUGIN_H
#define APE_FILAMENTSCENELOADERPLUGIN_H

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
#include "apeIFileGeometry.h"
#include "apeIPlaneGeometry.h"
#include "apeIFileMaterial.h"
#include "apeICloneGeometry.h"
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
#include "apeFilamentSceneLoaderPluginConfig.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#define THIS_PLUGINNAME "apeFilamentSceneLoaderPlugin"

namespace ape
{
	class FilamentSceneLoaderPlugin : public ape::IPlugin
	{
	private:
		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::UserInputMacro* mpApeUserInputMacro;

		ape::SceneMakerMacro* mpSceneMakerMacro;

		ape::ICoreConfig* mpCoreConfig;

		quicktype::Scene mScene;

		FILE* mApeFilamentSceneLoaderPluginConfigFile;

		std::map<std::string,tinygltf::Model> mGltfModel;

		std::vector<std::string> mApeNodes;

		std::vector<std::string> mApeEntities;

		std::vector<std::string> mModelPaths;

		bool mStopCalled;

		void parseGltfModel(std::string filePath);

		void parseRepresentations();

		void parseModelsAndNodes();

		void parsePlacementRelTo();

		void parseModelsIDs();

		void parseVisibleNodes();

		void createResourceList();

		void parseModels();

		void cloneGeometry(ape::FileGeometrySharedPtr fileGeometry, std::string id, ape::NodeSharedPtr parentNode);

		std::map<std::string, ape::Vector3> mFileGeometryNamesScales;

		std::multimap<std::string, std::string> mModelsIDs;

		std::string findGeometryNameByModelName(std::string modelName);

		void setInitialState();

	public:
		FilamentSceneLoaderPlugin();

		~FilamentSceneLoaderPlugin();

		void eventCallBack(const ape::Event& event);
		
		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateapeFilamentSceneLoaderPlugin()
	{
		return new ape::FilamentSceneLoaderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyapeFilamentSceneLoaderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::FilamentSceneLoaderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateapeFilamentSceneLoaderPlugin, DestroyapeFilamentSceneLoaderPlugin);
		return 0;
	}
}

#endif
