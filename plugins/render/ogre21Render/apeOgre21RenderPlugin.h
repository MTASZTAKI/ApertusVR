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
#include "apeOgre21Conversions.h"
#include "apeOgre21RenderPluginConfigs.h"
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
#include "OgreFrameListener.h"
#include "OgreTextureManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgreWindowEventUtilities.h"
#include "OgreHlmsManager.h"
#include "OgrePixelCountLodStrategy.h"
#include "OgreTextureManager.h"
#include "OgreD3D11RenderSystem.h"
#include "OgreD3D11Texture.h"
#include "D3D11.h"
#include "OgreWindowEventUtilities.h"
#include "OgreHlmsUnlit.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsCommon.h"
#include "OgreArchiveManager.h"
#include "OgreHlmsPbsDatablock.h"

#define THIS_PLUGINNAME "apeOgre21RenderPlugin"

namespace ape
{
	class Ogre21RenderPlugin : public IPlugin, public Ogre::FrameListener, public Ogre::WindowEventListener
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

		void windowResized(Ogre::RenderWindow* rw) override;

	private:
		Ogre::Root* mpRoot;

		Ogre::SceneManager* mpOgreSceneManager;

		std::vector<Ogre::Camera*> mOgreCameras;

		std::map<std::string, Ogre::RenderWindow*> mRenderWindows;

		Ogre::HlmsManager* mpHlmsPbsManager;

		Ogre::WindowEventUtilities* mpWindowEventUtilities;

		ape::ISceneManager* mpSceneManager;

		ape::IEventManager* mpEventManager;

		ape::ICoreConfig* mpCoreConfig;

		std::string mUniqueID;

		ape::DoubleQueue<Event> mEventDoubleQueue;

		ape::Ogre21RenderPluginConfig mOgre21RenderPluginConfig;

		std::vector<ape::ManualTextureWeakPtr> mRttList;

		ape::UserInputMacro* mpUserInputMacro;

		void processEventDoubleQueue();

		void eventCallBack(const ape::Event& event);

		void registerHlms();
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateOgre21RenderPlugin()
	{
		return new ape::Ogre21RenderPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOgre21RenderPlugin(ape::IPlugin *plugin)
	{
		delete (ape::Ogre21RenderPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateOgre21RenderPlugin, DestroyOgre21RenderPlugin);
		return 0;
	}
}

#endif
