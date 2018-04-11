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

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <fstream>

//Ogre 1.10 header includes
#include "OgreConfigFile.h"
#include "OgreEntity.h"
#include "OgreFont.h"
#include "OgreFontManager.h"
#include "OgreFrameListener.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbsMaterial.h"
#include "OgreLodConfig.h"
#include "OgreLodWorkQueueInjectorListener.h"
#include "OgreLodWorkQueueInjector.h"
#include "OgreLodWorkQueueRequest.h"
#include "OgreLodWorkQueueWorker.h"
#include "OgreLogManager.h"
#include "OgreMaterial.h"
#include "OgreManualObject.h"
#include "OgreMaterialManager.h"
#include "OgreMesh.h"
#include "OgreMeshLodGenerator.h"
#include "OgreMeshManager.h"
#include "OgreMovableObject.h"
#include "OgreOverlaySystem.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreOverlayContainer.h"
#include "OgrePanelOverlayElement.h"
#include "OgrePixelCountLodStrategy.h"
#include "OgreRenderWindow.h"
#include "OgreResourceGroupManager.h"
#include "OgreRoot.h"
#include "OgreRTShaderSystem.h"
#include "OgreShaderGenerator.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreSubMesh.h"
#include "OgreTechnique.h" 
#include "OgreTextAreaOverlayElement.h"
#include "OgreTextureManager.h"
#include "OgreWindowEventUtilities.h"
#include "ProceduralStableHeaders.h"
#include "Procedural.h"




//Apertus includes
#include "ApeIFileMaterial.h"
#include "ApeITextGeometry.h"
#include "ApeILight.h"
#include "ApeIFileGeometry.h"
#include "ApeIPlaneGeometry.h"
#include "ApeIBoxGeometry.h"
#include "ApeICylinderGeometry.h"
#include "ApeITorusGeometry.h"
#include "ApeITubeGeometry.h"
#include "ApeIRayGeometry.h"
#include "ApeISphereGeometry.h"
#include "ApeIConeGeometry.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeIIndexedLineSetGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"
#include "ApeIScene.h"
#include "ApeICamera.h"
#include "ApeIPointCloud.h"
#define APE_DOUBLEQUEUE_UNIQUE
#include "ApeDoubleQueue.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeOgreMovableText.h"
#include "ApeOgrePointCloud.h"
#include "ApeOgreConversions.h"
#include "ApeOgreRenderPluginConfigs.h"
#include "ApeISystemConfig.h"
#include "ApeIMainWindow.h"
#include "ApeIFileGeometry.h"
#include "ApeIUnitTexture.h"
#include "ApeIPbsPass.h"
#include "ApeIManualPass.h"
#include "ApeIManualTexture.h"
#include "ApeOgreShaderGeneratorResolver.h"

#define THIS_PLUGINNAME "ApeOgreRenderPlugin"

namespace Ape
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

		Ogre::SceneManager* mpSceneMgr;

		std::vector<Ogre::Camera*> mOgreCameras;

		Ogre::OverlaySystem* mpOverlaySys;

		Ogre::OverlayManager* mpOverlayMgr;

		Ogre::LodConfig mCurrentlyLoadingMeshEntityLodConfig;
		
		Ogre::Entity* mpCurrentlyLoadingMeshEntity;

		Ape::OgreMovableTextFactory* mpOgreMovableTextFactory;

		std::map<std::string, Ogre::RenderWindow*> mRenderWindows;

		Ogre::RTShader::ShaderGenerator* mpShaderGenerator;

		Ape::ShaderGeneratorResolver* mpShaderGeneratorResolver;

		Ogre::HlmsManager* mpHlmsPbsManager;

		Ogre::MeshLodGenerator* mpMeshLodGenerator;

		Ogre::MeshSerializer mMeshSerializer;

		Ogre::MaterialSerializer mMaterialSerializer;

		// Hydrax::Hydrax *mpHydrax;

		// SkyX::SkyX* mpSkyx;

		// Ogre::Light* mpSkyxSunlight;

		// Ogre::Light* mpSkyxSkylight;

		// SkyX::BasicController* mpSkyxBasicController;

		std::map<std::string, Ogre::PbsMaterial*> mPbsMaterials;

		Ape::IScene* mpScene;

		Ape::IEventManager* mpEventManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::DoubleQueue<Event> mEventDoubleQueue;

		Ape::OgreRenderPluginConfig mOgreRenderPluginConfig;

		Ape::NodeWeakPtr mUserNode;

		Ape::NodeWeakPtr mHeadNode;

		int mCameraCountFromConfig;

		std::map<std::string, Ape::OgrePointCloud*> mOgrePointCloudMeshes;

		void processEventDoubleQueue();

		void eventCallBack(const Ape::Event& event);

		void createAutoGeneratedLodLevelsIfNeeded(std::string geometryName);
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateOgreRenderPlugin()
	{
		return new Ape::OgreRenderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOgreRenderPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::OgreRenderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateOgreRenderPlugin, DestroyOgreRenderPlugin);
		return 0;
	}
}

#endif
