/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
	  |___/
	  Tutorial Framework
	  http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreConfigFile.h>
#include <OGRE/OgreWindowEventUtilities.h>
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlaySystem.h>
#endif

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <OGRE/OgreVector3.h>

class BaseApplication : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
{
public:
	BaseApplication(void);
	virtual ~BaseApplication(void);

	virtual void go(void);

protected:
	virtual bool setup();
	virtual bool configure(void);
	virtual void chooseSceneManager(void);
	virtual void createCamera(void);
	virtual void createFrameListener(void);
	virtual void createScene(void) = 0; // Override me!
	virtual void destroyScene(void);
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	virtual void createViewports(void);
#else
	virtual void createCompositor(void);
#endif
	virtual void setupResources(void);
	virtual void loadResources(void);
	virtual void createLogManager(void);

	// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent& arg );
	virtual bool keyReleased( const OIS::KeyEvent& arg );
	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent& arg );
	virtual bool mousePressed( const OIS::MouseEvent& arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id );

	//Adjust mouse clipping area
	virtual void windowResized(Ogre::RenderWindow* rw);
	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(Ogre::RenderWindow* rw);

	virtual void windowFocusChange(Ogre::RenderWindow* rw)
	{
		std::cout<<"focus change"<<std::endl;
	}

	void putMeshMat(const std::string& meshName, const std::string& matName, const Ogre::Vector3& position = Ogre::Vector3::ZERO, bool castShadows=true);
	void putMesh(const std::string& meshName, const Ogre::Vector3& position = Ogre::Vector3::ZERO);
	void putMesh2(const std::string& meshName, const Ogre::Vector3& position = Ogre::Vector3::ZERO);
	void putMesh3(const std::string& meshName, const Ogre::Vector3& position = Ogre::Vector3::ZERO);

	Ogre::Root* mRoot;
	Ogre::Camera* mCamera;
	Ogre::Light* movingLight;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;

	// OgreBites
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
	Ogre::OverlaySystem* mOverlaySystem;       // Overlay system
#endif
	OgreBites::SdkTrayManager* mTrayMgr;
	OgreBites::SdkCameraMan* mCameraMan;       // basic camera controller
	OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel
	bool mCursorWasVisible;                    // was cursor visible before dialog appeared
	bool mShutDown;

	//OIS Input devices
	bool mNonExclusiveMouse;
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
};

#endif // #ifndef __BaseApplication_h_
