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

#ifndef APE_FILAMENTAPPLICATIONPLUGIN_H
#define APE_FILAMENTAPPLICATIONPLUGIN_H

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
#include "apeFilamentApplicationPluginConfigs.h"
#include <filamentapp/Config.h>
#include <filamentapp/FilamentApp.h>
#include <filamentapp/IBL.h>
#include <filament/Camera.h>
#include <filament/ColorGrading.h>
#include <filament/Engine.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/Renderer.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/TransformManager.h>
#include <filament/VertexBuffer.h>
#include <filament/View.h>
#include "gltfio/AssetLoader.h"
#include "gltfio/FilamentAsset.h"
#include "gltfio/ResourceLoader.h"
#include <viewer/AutomationEngine.h>
#include <viewer/AutomationSpec.h>
#include <viewer/SimpleViewer.h>
#include <camutils/Manipulator.h>
#include <getopt/getopt.h>
#include "utils/EntityManager.h"
#include "utils/NameComponentManager.h"
#include "math/mathfwd.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include <math/vec4.h>
#include <math/norm.h>
#include <imgui.h>
#include <filagui/ImGuiExtensions.h>
//#include "generated/resources/gltf_viewer.h"
#include "math/TVecHelpers.h"

#define THIS_PLUGINNAME "apeFilamentApplicationPlugin"
using namespace filament;
using namespace filament::math;
using namespace filament::viewer;

using namespace gltfio;
using namespace utils;

struct App {
    Engine* engine;
    SimpleViewer* viewer;
    Config config;
    filament::Camera* mainCamera;

    AssetLoader* loader;
    FilamentAsset* asset = nullptr;
    NameComponentManager* names;

    MaterialProvider* materials;
    MaterialSource materialSource = GENERATE_SHADERS;

    gltfio::ResourceLoader* resourceLoader = nullptr;
    bool recomputeAabb = false;

    bool actualSize = false;

    struct ViewOptions {
        float cameraAperture = 16.0f;
        float cameraSpeed = 125.0f;
        float cameraISO = 100.0f;
        float groundShadowStrength = 0.75f;
        bool groundPlaneEnabled = false;
        bool skyboxEnabled = true;
        sRGBColor backgroundColor = { 0.0f };
    } viewOptions;

    struct Scene {
        Entity groundPlane;
        VertexBuffer* groundVertexBuffer;
        IndexBuffer* groundIndexBuffer;
        Material* groundMaterial;
    } scene;

    // zero-initialized so that the first time through is always dirty.
    ColorGradingSettings lastColorGradingOptions = { 0 };

    ColorGrading* colorGrading = nullptr;

    float rangePlot[1024 * 3];
    float curvePlot[1024 * 3];

    // 0 is the default "free camera". Additional cameras come from the gltf file.
    int currentCamera = 0;

    std::string messageBoxText;
    std::string settingsFile;
    std::string batchFile;

    AutomationSpec* automationSpec = nullptr;
    AutomationEngine* automationEngine = nullptr;
};
namespace ape
{
	class FilamentApplicationPlugin : public IPlugin
	{
	public:
		FilamentApplicationPlugin();

		~FilamentApplicationPlugin();

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
        
        ape::FilamentApplicationPluginConfig mFilamentApplicationPluginConfig;
        
        void initFilament();
        
        void parseJson();
        
        
        std::ifstream::pos_type getFileSize(const char* filename);
        
        bool loadSettings(const char* filename, Settings* out);
        
        void computeRangePlot(App& app, float* rangePlot);
        
        void rangePlotSeriesStart(int series);
        
        void rangePlotSeriesEnd(int series);
        
        float getRangePlotValue(int series, void* data, int index);
        
        float3 curves(float3 v, float3 shadowGamma, float3 midPoint, float3 highlightScale);
        
        void computeCurvePlot(App& app, float* curvePlot);
        
        LinearColor inverseTonemapSRGB(sRGBColor x);
        
        void colorGradingUI(App& app);
        
        void pushSliderColors(float hue);
        
        void popSliderColors() { ImGui::PopStyleColor(4); }

        const char* DEFAULT_IBL = "default_env";
        
        void tooltipFloat(float value);
        
		void processEventDoubleQueue();

		void eventCallBack(const ape::Event& event);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateFilamentApplicationPlugin()
	{
		return new ape::FilamentApplicationPlugin;
	}

	APE_PLUGIN_FUNC void DestroyFilamentApplicationPlugin(ape::IPlugin *plugin)
	{
		delete (ape::FilamentApplicationPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateFilamentApplicationPlugin, DestroyFilamentApplicationPlugin);
		return 0;
	}
}

#endif
