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


//ApertusVR includes
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
#include "ApeISystemConfig.h"
#include "ApeIMainWindow.h"
#include "ApeIFileGeometry.h"
#include "ApeIUnitTexture.h"
#include "ApeIPbsPass.h"
#include "ApeIManualPass.h"
#include "ApeIManualTexture.h"
#include "ApeISky.h"
#include "ApeIWater.h"

//Own includes
#include "ApeOgre21RenderPluginConfigs.h"
#include "ApeOgre21Conversions.h"

#define THIS_PLUGINNAME "ApeOgre21RenderPlugin"

namespace Ape
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

		

	private:
		Ogre::Root* mpRoot;

		Ogre::SceneManager* mpSceneMgr;

		std::vector<Ogre::Camera*> mOgreCameras;

		

		std::map<std::string, Ogre::RenderWindow*> mRenderWindows;

		

		Ogre::HlmsManager* mpHlmsPbsManager;

		

		

		Ape::IScene* mpScene;

		Ape::IEventManager* mpEventManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::DoubleQueue<Event> mEventDoubleQueue;

		Ape::Ogre21RenderPluginConfig mOgreRenderPluginConfig;

		Ape::NodeWeakPtr mUserNode;

		int mCameraCountFromConfig;

		void processEventDoubleQueue();

		void eventCallBack(const Ape::Event& event);

		
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateOgre21RenderPlugin()
	{
		return new Ogre21RenderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOgre21RenderPlugin(Ape::IPlugin *plugin)
	{
		delete (Ogre21RenderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateOgre21RenderPlugin, DestroyOgre21RenderPlugin);
		return 0;
	}
}

#endif
