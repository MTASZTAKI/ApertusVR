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
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/View.h>
#include <filament/Frustum.h>
#include <filament/Box.h>
#include "gltfio/AssetLoader.h"
#include "gltfio/FilamentAsset.h"
#include "gltfio/ResourceLoader.h"
#include <viewer/AutomationEngine.h>
#include <viewer/AutomationSpec.h>
#include <viewer/SimpleViewer.h>
#include <viewer/Settings.h>
#include <camutils/Manipulator.h>
#include <getopt/getopt.h>
#include "utils/EntityManager.h"
#include "utils/NameComponentManager.h"
#include "math/mathfwd.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include <math/vec4.h>
#include <math/norm.h>
#include <filagui/ImGuiExtensions.h>
#include "apeVLFTImgui.h"
#include "apeVLFTAnimationPlayerPluginConfig.h"
//#include "generated/resources/gltf_viewer.h"
#include "math/TVecHelpers.h"
#include <cmath>

#include "generated/resources/resources.h"

#define THIS_PLUGINNAME "apeFilamentApplicationPlugin"
using namespace filament;
using namespace filament::math;
using namespace filament::viewer;

using namespace gltfio;
using namespace utils;


//Needs scene
//swapshain
//lightmanager
//sunlight
struct InstanceData{
    int index;
    std::string assetName;
    FilamentInstance* mpInstance;
    InstanceData(){
        index = -1;
        assetName = "";
        mpInstance = nullptr;
    }
    InstanceData(int index_, std::string assetName_, FilamentInstance* instance_){
        index = index_;
        assetName = assetName_;
        mpInstance = instance_;
    }
    
};

struct LineVertex{
    float3 pos;
    uint32_t color;
};

struct SpaghettiLines{
    filament::VertexBuffer* lineVertexBuffer;
    filament::IndexBuffer* lineIndexBuffer;
    std::vector<LineVertex> lineVertices;
    std::vector<uint32_t> lineIndices;
    size_t size;
    utils::Entity lineEntity;
    filament::Material* mat;
    SpaghettiLines(){
        size = 0;
        lineVertexBuffer = nullptr;
        lineIndexBuffer = nullptr;
        lineEntity = EntityManager::get().create();
        
    }
    SpaghettiLines(size_t size_,  filament::VertexBuffer* lineVertexBuffer_, filament::IndexBuffer* lineIndexBuffer_, LineVertex* lineVertices_, uint32_t* lineIndices_){
        lineVertexBuffer = lineVertexBuffer_;
        size = size_;
        lineIndexBuffer = lineIndexBuffer_;
        lineVertices.assign(lineVertices_, lineVertices_+size);
        lineIndices.assign(lineIndices_,lineIndices_+size);
        lineEntity = EntityManager::get().create();
    }
};

struct App {
    Engine* engine;
    SimpleViewer* viewer;
    Config config;
    Scene* mpScene;
    View* view;
    filament::Camera* mainCamera;
    
    Settings settings;
    AssetLoader* loader;
    std::map<std::string, FilamentAsset*> asset;
    NameComponentManager* names;
    std::map<std::string,std::vector<FilamentInstance*>> instances;
    std::map<std::string, int> instanceCount;
    std::map<std::string, std::vector<std::string>> geometryNameMap;
    
    std::map<std::string, InstanceData> mpInstancesMap;
    
    LightManager* lightManager;
    Entity sunLight;
    Entity lineEntity;
    
    std::pair<Entity, float> boxEntity;
    std::pair<Entity, std::string> selectedNode;
    std::pair<Entity, std::string> rootOfSelected;
    IndirectLight* indirectLight;
    std::vector<std::pair<Entity, float>> rayIntersectedEntities;
    updateInfo updateinfo;
    Entity parentOfPicked;
    
    filament::math::float3 sunlightColor = filament::Color::toLinear<filament::ACCURATE>({ 0.98, 0.92, 0.89});
    filament::math::float3 sunlightDirection = {0.6, -1.0, -0.8};
    float sunlightIntensity = 100000.0f;
    float IblIntensity = 30000.0f;
    float IblRotation = 0.0f;
    
    EntityManager* mpEntityManager;
    TransformManager* mpTransformManager;
    RenderableManager * mpRenderableManager;
    std::map<std::string, TransformManager::Instance> mpTransforms;
    std::map<std::string, FilamentAsset*> mpLoadedAssets;
    
    
    MaterialProvider* materials;
    MaterialSource materialSource = GENERATE_SHADERS;

    gltfio::ResourceLoader* resourceLoader = nullptr;
    bool recomputeAabb = false;

    bool actualSize = false;
    bool firstRun = true;

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
    
    struct AnimationData{
        bool animatedClick;
        bool mouseDown;
        double mouseStartTime;
        
        bool animatedKey;
        std::map<SDL_Scancode,bool> keyDown;
        bool keysDown;
        double keyStartTime;
        int keyCurrentAnimation;
    } animationData;
    
    std::map<std::string, SpaghettiLines> spaghettiLines;
    
    filament::VertexBuffer* boxVertexBuffer;
    filament::IndexBuffer* boxIndexBuffer;
    float3* boxVerts = (float3*) malloc(sizeof(float3) * 8);
    uint32_t* boxInds = (uint32_t*) malloc(sizeof(uint32_t) * 24);
    
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
        
        struct Animation
        {
            animationQuicktype::EventType type;
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
        
        ape::ISceneNetwork* mpSceneNetwork;

		ape::IEventManager* mpEventManager;

		ape::ICoreConfig* mpCoreConfig;

		std::string mUniqueID;

		ape::DoubleQueue<Event> mEventDoubleQueue;
        
        ape::DoubleQueue<Event> mTmpEventDoubleQueue;

		std::vector<ape::ManualTextureWeakPtr> mRttList;

		ape::UserInputMacro* mpUserInputMacro;
        
        ape::FilamentApplicationPluginConfig mFilamentApplicationPluginConfig;
        
        ape::VLFTImgui* mpVlftImgui;
        
        App app;
        
        std::map<std::string, SDL_EventType> mEventMap;
        
        std::map<std::string, SDL_Scancode> mKeyMap;
        
        filament::camutils::Bookmark<float> mCameraBookmark;
        
        filament::camutils::Manipulator<float>* mCamManipulator;
        
        animationQuicktype::Animations mAnimations;
        
        std::thread mAnimationThread;
        
        std::vector<Animation> mParsedAnimations;
        
        int mPlayedAnimations;

        std::vector<std::string> mAnimatedNodeNames;

        std::vector<std::string> mSpaghettiNodeNames;

        std::vector<ape::NodeWeakPtr> mAttachedUsers;
        
        double mStartTime;

        float mTimeToSleepFactor;
        
        int mBookmarkID;
        
        std::string mUserName;
        
        std::vector<unsigned long long> mParsedBookmarkTimes;
        
        unsigned long long mClickedBookmarkTime;
        
        int mChoosedBookmarkedAnimationID;
        
        bool mIsPauseClicked;

        bool mIsStopClicked;

        bool mIsPlayRunning;

        bool mIsStudentsMovementLogging;

        bool mIsAllSpaghettiVisible;
        
        bool mIsStudent;
        
        bool isSelected;
        
        void initFilament();
        
        void parseJson();
        
        void initAnimations();
        
        void initKeyMap();
        
        void playAnimations(double now);
        
        void showSpaghetti(std::string name, bool show);
        
        bool attach2NewAnimationNode(const std::string& parentNodeName, const ape::NodeSharedPtr& node);
        
        void drawSpaghettiSection(const ape::Vector3& startPosition, const ape::NodeSharedPtr& node, std::string& spaghettiSectionName);
        
        std::ifstream::pos_type getFileSize(const char* filename);
        
        bool loadSettings(const char* filename, Settings* out);

        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
        
        const char* DEFAULT_IBL = "default_env";
        
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
