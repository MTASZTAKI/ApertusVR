/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef _RENDEROGRE_H_
#define _RENDEROGRE_H_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include <OgreWindowEventUtilities.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include "terrain.h"
#include "physics.h"

namespace Dolphin
{
	class CameraManager;

	class RenderOgre : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener
	{
	public:
		RenderOgre();
		static Ogre::RenderWindow* getmWindow();
		static OgreBites::SdkCameraMan* getmCameraMan();
		static Ogre::Camera* getmCamera();
		static bool RenderOgre::IsShutDown();
		static void SetShutDown(bool b);
		static Ogre::Root* GetOgreRoot();
		static Ogre::SceneManager* GetSceneManager();
		void SetTerrain(Terrain* tr);
		void SetPhysics(Physics* ph);

		virtual ~RenderOgre(void);

		void SetDefaultTextureFiltering(int tfo);
		void SetDefaultAnisotropy(int aniso);
		int GetPolygonMode();
		void SetPolygonMode(int pm);
		void RefreshAllTexture();
		void TakeScreeShot();

		bool Initialize(void);

	protected:
		static Ogre::Root* mRoot;
		static Ogre::Camera* mCamera;
		static Ogre::SceneManager* mSceneMgr;
		static Ogre::RenderWindow* mWindow;
		Ogre::String mResourcesCfg;
		Ogre::String mPluginsCfg;

		Ogre::OverlaySystem *mOverlaySystem;

		// OgreBites
		static OgreBites::SdkCameraMan* mCameraMan;      // basic camera controller
		bool mCursorWasVisible;                   // was cursor visible before dialog appeared
		static bool mShutDown;

		// Ogre::FrameListener
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool frameStarted(const Ogre::FrameEvent& evt);

		// Ogre::WindowEventListener
		virtual void windowResized(Ogre::RenderWindow* rw);
		virtual void windowClosed(Ogre::RenderWindow* rw);
		Ogre::Light* mLight;
		CameraManager* cameramanager;
		Terrain* terrain;
		Physics* physics;
	};
}
#endif //_RENDEROGRE_H_