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

#ifndef APE_OGRE21RENDERPLUGIN_H
#define APE_OGRE21RENDERPLUGIN_H

//std includes
#include <iostream>

//Ogre 2.10 header includes
#include "OgreConfigFile.h"
#include "OgreEntity.h"
#include "OgreFrameListener.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreHlmsManager.h"
#include "OgreLogManager.h"
#include "OgreMaterial.h"
#include "OgreManualObject.h"
#include "OgreManualObject2.h"
#include "OgreMesh2.h"
#include "OgreMeshManager2.h"
#include "OgreMovableObject.h"
#include "OgrePixelCountLodStrategy.h"
#include "OgreRenderWindow.h"
#include "OgreResourceGroupManager.h"
#include "OgreRoot.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreSubMesh2.h"
#include "OgreTechnique.h" 
#include "OgreTextureManager.h"
#include "OgreWindowEventUtilities.h"
#include <OgreCamera.h>
#include <OgreViewport.h>
#include "OgreMaterialManager.h"

#include "OgreLodConfig.h"
#include "OgreMeshLodGenerator.h"
#include "OgreLodWorkQueueInjectorListener.h"
#include "OgreLodWorkQueueInjector.h"
#include "OgrePixelCountLodStrategy.h"
#include "OgreLodWorkQueueRequest.h"
#include "OgreLodWorkQueueWorker.h"
#include "OgreItem.h"
#include "OgrePrerequisites.h"

#include "OgreMesh.h"
#include "OgreMeshManager.h"
#include "OgreMeshSerializer.h"
#include <OgreMesh2Serializer.h>
#include "OgreMeshSerializerImpl.h"
#include "OgreMeshManager2.h"

#include "OgreHlmsUnlit.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsCommon.h"
#include "OgreArchiveManager.h"
#include "OgreHlmsPbsDatablock.h"


#include <OgreCompositorManager2.h>

#include <OgreStaticGeometry.h>
#include <OgreMaterialSerializer.h>

#include <OgreD3D11Texture.h>
#include <OgreGL3PlusTexture.h>

#include <OgreCompositorWorkspace.h>

#include "OgreHlmsManager.h"






//ApertusVR includes
#include "managers/apeILogManager.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeIBoxGeometry.h"
#include "sceneelements/apeICylinderGeometry.h"
#include "sceneelements/apeITorusGeometry.h"
#include "sceneelements/apeITubeGeometry.h"
#include "sceneelements/apeIRayGeometry.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeIIndexedFaceSetGeometry.h"
#include "sceneelements/apeIIndexedLineSetGeometry.h"
#include "sceneelements/apeIManualMaterial.h"
#include "plugin/apePluginAPI.h"
#include "plugin/apeIPlugin.h"
#include "managers/apeISceneManager.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIPointCloud.h"
#include "macros/userInput/apeUserInputMacro.h"
#define APE_DOUBLEQUEUE_UNIQUE
#include "utils/apeDoubleQueue.h"
#include "managers/apeIEventManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIUnitTexture.h"
#include "sceneelements/apeIPbsMaterial.h"
#include "sceneelements/apeIManualPass.h"
#include "sceneelements/apeIManualTexture.h"
#include "sceneelements/apeISky.h"
#include "sceneelements/apeIWater.h"


//Own includes
#include "apeOgre21RenderPluginConfigs.h"
#include "apeOgre21Conversions.h"

#define THIS_PLUGINNAME "apeOgre21RenderPlugin"

namespace ape
{
	class Ogre21RenderPlugin : public IPlugin, public Ogre::FrameListener
	{
	public:
		Ogre21RenderPlugin();

		~Ogre21RenderPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		bool frameStarted(const Ogre::FrameEvent& evt) override;

		bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

		bool frameEnded(const Ogre::FrameEvent& evt) override;

		template <class id_t>
		Ogre::IndexBufferPacked* inflateIndexBufferPacked(ape::GeometryIndexedFaceSetParameters parameters, Ogre::VaoManager* vaomgr,
			Ogre::IndexBufferPacked::IndexType typenum, id_t* buffer);

	private:
		Ogre::Root* mpRoot;

		Ogre::SceneManager* mpSceneMgr;

		Ogre::MaterialPtr mSkyBoxMaterial;

		Ogre::CompositorManager2 * mpCompositorManager;

		std::map<std::string, Ogre::Item*> mItemList;

		std::map<std::string, Ogre::MaterialPtr> mMatList;

		std::map<std::string, Ogre::ManualObject*> mManualObjectList;

		std::map<std::string, Ogre::HlmsPbsDatablock*> mPbsDataBlockList;

		std::map<std::string, Ogre::MeshPtr> mMeshPtrList;

		std::vector<Ogre::Camera*> mOgreCameras;

		Ogre::RenderWindow* mpActualRenderwindow;

		Ogre::CompositorWorkspace* mpActualWorkSpace;

		std::map<std::string, Ogre::Light*> mLightList;

		std::map<std::string, Ogre::RenderWindow*> mRenderWindows;

		Ogre::HlmsManager* mpHlmsPbsManager;

		Ogre::MaterialSerializer mMaterialSerializer;

		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::DoubleQueue<Event> mEventDoubleQueue;

		ape::Ogre21RenderPluginConfig mOgreRenderPluginConfig;

		ape::NodeWeakPtr mUserNode;

		ape::UserInputMacro* mpUserInputMacro;

		ape::UserInputMacro::ViewPose mUserInputMacroPose;

		std::vector<ape::ManualTextureWeakPtr> mRttList;

		int mCameraCountFromConfig;

		void processEventDoubleQueue();

		void eventCallBack(const ape::Event& event);

		void registerHlms();

	};

	APE_PLUGIN_FUNC ape::IPlugin* CreateOgreRenderPlugin()
	{
		return new ape::Ogre21RenderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOgreRenderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::Ogre21RenderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateOgreRenderPlugin, DestroyOgreRenderPlugin);
		return 0;
	}
}

#endif