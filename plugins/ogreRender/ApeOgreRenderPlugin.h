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

#ifndef APE_OGRERENDERPLUGIN_H
#define APE_OGRERENDERPLUGIN_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
#include "plugin/apeIPlugin.h"
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeIBoxGeometry.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeICylinderGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeIIndexedFaceSetGeometry.h"
#include "sceneelements/apeIIndexedLineSetGeometry.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIManualPass.h"
#include "sceneelements/apeIManualTexture.h"
#include "sceneelements/apeIPbsPass.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeIPointCloud.h"
#include "sceneelements/apeIRayGeometry.h"
#include "sceneelements/apeISky.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneelements/apeITorusGeometry.h"
#include "sceneelements/apeITubeGeometry.h"
#include "sceneelements/apeIUnitTexture.h"
#include "sceneelements/apeIWater.h"
#include "macros/userInput/apeUserInputMacro.h"
#define APE_DOUBLEQUEUE_UNIQUE
#include "utils/apeDoubleQueue.h"
#include "apeOgreShaderGeneratorResolver.h"
#include "apeOgreMovableText.h"
#include "apeOgrePointCloud.h"
#include "apeOgreConversions.h"
#include "apeOgreRenderPluginConfigs.h"
#include "OgreSceneManager.h"
#include "OgreRoot.h"
#include "OgreConfigFile.h"
#include "OgreLogManager.h"
#include "OgreResourceGroupManager.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreMovableObject.h"
#include "OgreMeshManager.h"
#include "OgreMaterialManager.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"
#include "OgreRenderWindow.h"
#include "OgreManualObject.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreOverlaySystem.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreOverlayContainer.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgreFontManager.h"
#include "OgreFont.h"
#include "OgreRTShaderSystem.h"
#include "OgreShaderGenerator.h"
#include "OgreFrameListener.h"
#include "OgreTextureManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgrePanelOverlayElement.h"
#include "OgreWindowEventUtilities.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbsMaterial.h"
#include "OgreLodConfig.h"
#include "OgreMeshLodGenerator.h"
#include "OgreLodWorkQueueInjectorListener.h"
#include "OgreLodWorkQueueInjector.h"
#include "OgrePixelCountLodStrategy.h"
#include "OgreLodWorkQueueRequest.h"
#include "OgreLodWorkQueueWorker.h"
#include "OgreTextureManager.h"
#include <OgreGLTextureManager.h>
#include <OgreGLRenderSystem.h>
#include <OgreGLTexture.h>
#include "OgreD3D11RenderSystem.h"
#include "OgreD3D11Texture.h"
#include "D3D11.h"
#include "ProceduralStableHeaders.h"
#include "Procedural.h"
#include "Hydrax.h"
#ifdef HYDRAX_NEW
#include "Noise/Perlin/HydraxPerlin.h"
#include "Modules/ProjectedGrid/HydraxProjectedGrid.h"
#else
#include "Noise/Perlin/Perlin.h"
#include "Modules/ProjectedGrid/ProjectedGrid.h"
#endif
#include "SkyX.h"

#define THIS_PLUGINNAME "apeOgreRenderPlugin"

namespace ape
{
	class OgreRenderPlugin : public IPlugin, public Ogre::FrameListener, public Ogre::LodWorkQueueInjectorListener
	{
	public:
		OgreRenderPlugin();

		~OgreRenderPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		bool frameStarted(const Ogre::FrameEvent& evt) override;

		bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

		bool frameEnded(const Ogre::FrameEvent& evt) override;

		bool shouldInject(Ogre::LodWorkQueueRequest* request) override;

		void injectionCompleted(Ogre::LodWorkQueueRequest* request) override;

	private:
		Ogre::Root* mpRoot;

		Ogre::SceneManager* mpOgreSceneManager;

		std::vector<Ogre::Camera*> mOgreCameras;

		Ogre::OverlaySystem* mpOverlaySys;

		Ogre::OverlayManager* mpOverlayMgr;

		Ogre::LodConfig mCurrentlyLoadingMeshEntityLodConfig;
		
		Ogre::Entity* mpCurrentlyLoadingMeshEntity;

		ape::OgreMovableTextFactory* mpOgreMovableTextFactory;

		std::map<std::string, Ogre::RenderWindow*> mRenderWindows;

		Ogre::RTShader::ShaderGenerator* mpShaderGenerator;

		ape::ShaderGeneratorResolver* mpShaderGeneratorResolver;

		Ogre::HlmsManager* mpHlmsPbsManager;

		Ogre::MeshLodGenerator* mpMeshLodGenerator;

		Ogre::MeshSerializer mMeshSerializer;

		Ogre::MaterialSerializer mMaterialSerializer;

		Hydrax::Hydrax *mpHydrax;

		SkyX::SkyX* mpSkyx;

		Ogre::Light* mpSkyxSunlight;

		Ogre::Light* mpSkyxSkylight;

		SkyX::BasicController* mpSkyxBasicController;

		std::map<std::string, Ogre::PbsMaterial*> mPbsMaterials;

		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::DoubleQueue<Event> mEventDoubleQueue;

		ape::OgreRenderPluginConfig mOgreRenderPluginConfig;

		std::vector<ape::ManualTextureWeakPtr> mRttList;

		int mCameraCountFromConfig;

		std::map<std::string, ape::OgrePointCloud*> mOgrePointCloudMeshes;

		ape::UserInputMacro* mpUserInputMacro;

		ape::UserInputMacro::ViewPose mUserInputMacroPose;

		void processEventDoubleQueue();

		void eventCallBack(const ape::Event& event);

		void createAutoGeneratedLodLevelsIfNeeded(std::string geometryName);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateOgreRenderPlugin()
	{
		return new ape::OgreRenderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOgreRenderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::OgreRenderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateOgreRenderPlugin, DestroyOgreRenderPlugin);
		return 0;
	}
}

#endif
