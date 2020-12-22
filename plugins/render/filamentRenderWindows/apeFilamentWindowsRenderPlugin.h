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

#ifndef APE_FILAMENTWINDOWSRENDERPLUGIN_H
#define APE_FILAMENTWINDOWSRENDERPLUGIN_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeIBoxGeometry.h"
#include "apeICamera.h"
#include "apeICloneGeometry.h"
#include "apeIConeGeometry.h"
#include "apeICylinderGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIFileMaterial.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeILight.h"
#include "apeIManualMaterial.h"
#include "apeIManualTexture.h"
#include "apeIPlaneGeometry.h"
#include "apeIPointCloud.h"
#include "apeIRayGeometry.h"
#include "apeISky.h"
#include "apeISphereGeometry.h"
#include "apeITextGeometry.h"
#include "apeITorusGeometry.h"
#include "apeITubeGeometry.h"
#include "apeIFileTexture.h"
#include "apeIUnitTexture.h"
#include "apeIWater.h"
#include "apeUserInputMacro.h"
#define APE_DOUBLEQUEUE_UNIQUE
#include "apeDoubleQueue.h"
#include "apeFilamentWindowsRenderPluginConfigs.h"
#include "filameshio/MeshReader.h"
#include "filament/FilamentAPI.h"
#include "filament/RenderableManager.h"
#include "filament/TransformManager.h"
#include "filament/VertexBuffer.h"
#include "filament/IndexBuffer.h"
#include "filament/LightManager.h"
#include "filament/Engine.h"
#include "filament/Material.h"
#include "filament/Renderer.h"
#include "filament/SwapChain.h"
#include "filament/View.h"
#include "filament/Viewport.h"
#include "filament/Camera.h"
#include "filament/Scene.h"
#include "filament/SwapChain.h"
#include "filament/Renderer.h"
#include "gltfio/AssetLoader.h"
#include "gltfio/FilamentAsset.h"
#include "gltfio/ResourceLoader.h"
#include "gltfio/FilamentInstance.h"
#include "gltfio/MaterialProvider.h"
#include "utils/EntityManager.h"
#include "utils/NameComponentManager.h"
#include "math/mathfwd.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "math/TVecHelpers.h"

#define THIS_PLUGINNAME "apeFilamentWindowsRenderPlugin"

namespace ape
{
	class FilamentWindowsRenderPlugin : public IPlugin
	{
	public:
		FilamentWindowsRenderPlugin();

		~FilamentWindowsRenderPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:
		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::ICoreConfig* mpCoreConfig;

		std::string mUniqueID;

		ape::DoubleQueue<Event> mEventDoubleQueue;

		std::vector<ape::ManualTextureWeakPtr> mRttList;

		ape::UserInputMacro* mpUserInputMacro;

		filament::Engine* mpFilamentEngine;

		gltfio::AssetLoader* mpGltfAssetLoader;

		filament::Renderer* mpFilamentRenderer;

		filament::SwapChain* mpFilamentSwapChain;

		filament::Camera* mpFilamentCamera;

		filament::View* mpFilamentView;

		filament::Scene* mpFilamentScene;

		filament::TransformManager* mpFilamentTransformManager;

		utils::EntityManager* mpFilamentEntityManager;

		std::map<std::string, gltfio::FilamentAsset*> mpFilamentLoadedAssets;

		std::map<std::string, filament::TransformManager::Instance> mpFilamentTransforms;

		filament::LightManager::Builder* mpFilamentLightManagerBuilder;

		utils::Entity mFilamentSunlight;

		utils::NameComponentManager* mpFilamentNameComponentManager;

		gltfio::MaterialProvider* mpFilamentMaterialProvider;

		ape::FilamentWindowsRenderPluginConfig mFilamentWindowsRenderPluginConfig;
        
        void initFilament();
        
        void parseJson();
        

		void processEventDoubleQueue();

		void eventCallBack(const ape::Event& event);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateFilamentWindowsRenderPlugin()
	{
		return new ape::FilamentWindowsRenderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyFilamentWindowsRenderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::FilamentWindowsRenderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateFilamentWindowsRenderPlugin, DestroyFilamentWindowsRenderPlugin);
		return 0;
	}
}

#endif
