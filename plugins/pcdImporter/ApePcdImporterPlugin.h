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

#ifndef APE_PCDIMPORTERPLUGIN_H
#define APE_PCDIMPORTERPLUGIN_H

#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include "system/ApeIMainWindow.h"
#include "plugin/ApeIPlugin.h"
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "managers/ApeISystemConfig.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeICamera.h"
#include "sceneelements/ApeIFileGeometry.h"
#include "sceneelements/ApeIFileTexture.h"
#include "sceneelements/ApeIIndexedFaceSetGeometry.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeITextGeometry.h"

#define THIS_PLUGINNAME "ApePcdImporterPlugin"

namespace Ape
{
	class PcdImporterPlugin : public Ape::IPlugin
	{
	public:
		PcdImporterPlugin();

		~PcdImporterPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;


		void eventCallBack(const Ape::Event& event);

		void createNode(int assimpSceneID, aiNode* assimpNode);

		void loadConfig();

		void readFile(std::string fileName);

		void loadScene(const aiScene* assimpScene, int ID);
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreatePcdImporterPlugin()
	{
		return new Ape::PcdImporterPlugin;
	}

	APE_PLUGIN_FUNC void DestroyPcdImporterPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::PcdImporterPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreatePcdImporterPlugin, DestroyPcdImporterPlugin);
		return 0;
	}
}

#endif
