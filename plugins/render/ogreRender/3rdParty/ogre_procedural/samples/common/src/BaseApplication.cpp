/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
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
#include "ProceduralStableHeaders.h"
#include "BaseApplication.h"
#include "ProceduralPlatform.h"
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
#include "Compositor/OgreCompositorManager2.h"
#endif

using namespace Ogre;
//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
	: mRoot(0)
	, mCamera(0)
	, mSceneMgr(0)
	, mWindow(0)
	, mResourcesCfg("")
	, mPluginsCfg("")
	, mTrayMgr(0)
	, mCameraMan(0)
	, mDetailsPanel(0)
	, mCursorWasVisible(false)
	, mShutDown(false)
	, mNonExclusiveMouse(false)
	, mInputManager(0)
	, mMouse(0)
	, mKeyboard(0)
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
	, mOverlaySystem(0)
#endif
{
}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
	if (mTrayMgr) delete mTrayMgr;
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
	if (mOverlaySystem) delete mOverlaySystem;
#endif
	if (mCameraMan) delete mCameraMan;

	//Remove ourself as a Window listener
	WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if (mRoot->restoreConfig() || mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "Ogre Procedural example");
		mWindow->setDeactivateOnFocusChange(false);

		return true;
	}
	else
	{
		return false;
	}
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
	// Get the SceneManager, in this case a generic one
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC);
#else
	const size_t numThreads = std::max<size_t>(1, Ogre::PlatformInformation::getNumLogicalCores() / 2);
	Ogre::InstancingTheadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
	if(numThreads > 1) Ogre::InstancingTheadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
	mSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, numThreads, threadedCullingMethod);
#endif
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Vector3(0,0,80));
	// Look back along -Z
	mCamera->lookAt(Vector3(0,0,-300));
	mCamera->setNearClipDistance(.5);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller

	mSceneMgr->setShadowFarDistance(100.0);
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowTextureSize(1024);
#endif
	mSceneMgr->setAmbientLight(ColourValue::Black);
	// Setup camera and light
	mCamera->setPosition(0,50,-50);
	mCamera->lookAt(0,0,0);
	// Slow down speed, as the scene is small
	mCameraMan->setTopSpeed(20);

#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	Light* l = mSceneMgr->createLight("myLight");
#else
	Light* l = mSceneMgr->createLight();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(l);
#endif
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(Vector3(0,-1,1).normalisedCopy());
	l->setDiffuseColour(ColourValue(.7f,.5f,.5f));
	l->setSpecularColour(ColourValue::White);

#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	movingLight = mSceneMgr->createLight("movingLight");
	movingLight->setPosition(mCamera->getPosition());
#else
	movingLight = mSceneMgr->createLight();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(movingLight);
	movingLight->getParentSceneNode()->setPosition(mCamera->getPosition());
#endif
	movingLight->setType(Light::LT_POINT);
	movingLight->setDiffuseColour(ColourValue(.5f,.5f,.7f));
	movingLight->setSpecularColour(ColourValue::White);
	movingLight->setCastShadows(false);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	if (mNonExclusiveMouse)
	{
#if defined OIS_WIN32_PLATFORM
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
	}

	mInputManager = OIS::InputManager::createInputSystem( pl );

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);

#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
	OgreBites::InputContext input;
	input.mAccelerometer = NULL;
	input.mKeyboard = mKeyboard;
	input.mMouse = mMouse;
	input.mMultiTouch = NULL;

	mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, input, this);
#else
	mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mMouse, this);
#endif
	mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	mTrayMgr->hideCursor();

	// create a params panel for displaying sample details
	StringVector items;
	items.push_back("cam.pX");
	items.push_back("cam.pY");
	items.push_back("cam.pZ");
	items.push_back("");
	items.push_back("cam.oW");
	items.push_back("cam.oX");
	items.push_back("cam.oY");
	items.push_back("cam.oZ");
	items.push_back("");
	items.push_back("Filtering");
	items.push_back("Poly Mode");

	mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
	mDetailsPanel->setParamValue(9, "Bilinear");
	mDetailsPanel->setParamValue(10, "Solid");
	mDetailsPanel->hide();

	mRoot->addFrameListener(this);
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
	mSceneMgr->addRenderQueueListener(mOverlaySystem);
#endif
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
	mSceneMgr->removeRenderQueueListener(mOverlaySystem);
#endif
}
//-------------------------------------------------------------------------------------
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
void BaseApplication::createViewports(void)
{
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
	    Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}
#else
void BaseApplication::createCompositor(void)
{
	Ogre::CompositorManager2* pCompositorManager = mRoot->getCompositorManager2();
	const Ogre::IdString workspaceName = "scene workspace";
	pCompositorManager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue::White);
	pCompositorManager->addWorkspace(mSceneMgr, mWindow, mCamera, workspaceName, true);
}
#endif
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
	// Load resource paths from config file
	ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap* settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(
			    archName, typeName, secName);
		}
	}
}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::go(void)
{
#if PROCEDURAL_DEBUG_MODE == 1
	mPluginsCfg = "plugins_d.cfg";
#else
	mPluginsCfg = "plugins.cfg";
#endif
	mResourcesCfg = "resources.cfg";

	if (!setup())
		return;

	mRoot->startRendering();

	// clean up
	destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
	createLogManager();

	mRoot = new Root(mPluginsCfg);
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
	mOverlaySystem = new Ogre::OverlaySystem();
#endif

	setupResources();

	bool carryOn = configure();
	if (!carryOn) return false;

	chooseSceneManager();
	createCamera();
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	createViewports();
#else
	createCompositor();
#endif

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Load resources
	loadResources();

	// Create the scene
	createScene();

	createFrameListener();

	return true;
};
//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const FrameEvent& evt)
{
	if (mWindow->isClosed())
		return false;

	if (mShutDown)
		return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	mTrayMgr->frameRenderingQueued(evt);

	if (!mTrayMgr->isDialogVisible())
	{
		mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera
		if (mDetailsPanel->isVisible())   // if details panel is visible, then update its contents
		{
			mDetailsPanel->setParamValue(0, StringConverter::toString(mCamera->getDerivedPosition().x));
			mDetailsPanel->setParamValue(1, StringConverter::toString(mCamera->getDerivedPosition().y));
			mDetailsPanel->setParamValue(2, StringConverter::toString(mCamera->getDerivedPosition().z));
			mDetailsPanel->setParamValue(4, StringConverter::toString(mCamera->getDerivedOrientation().w));
			mDetailsPanel->setParamValue(5, StringConverter::toString(mCamera->getDerivedOrientation().x));
			mDetailsPanel->setParamValue(6, StringConverter::toString(mCamera->getDerivedOrientation().y));
			mDetailsPanel->setParamValue(7, StringConverter::toString(mCamera->getDerivedOrientation().z));
		}
	}

	return true;
}
//-------------------------------------------------------------------------------------
void BaseApplication::putMesh3(const std::string& meshName, const Vector3& position)
{
	putMeshMat(meshName, "Examples/Road", position, false);
}
//-------------------------------------------------------------------------------------
void BaseApplication::putMesh2(const std::string& meshName, const Vector3& position)
{
	putMeshMat(meshName, "Examples/Rockwall", position, false);
}
//-------------------------------------------------------------------------------------
void BaseApplication::putMesh(const std::string& meshName, const Vector3& position)
{
	putMeshMat(meshName, "Examples/BeachStones", position, true);
}
//-------------------------------------------------------------------------------------
void BaseApplication::putMeshMat(const std::string& meshName, const std::string& matName, const Vector3& position, bool castShadows)
{
	Entity* ent2 = mSceneMgr->createEntity(meshName);
	SceneNode* sn = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	sn->attachObject(ent2);
	sn->setPosition(position);
	ent2->setMaterialName(matName);
	ent2->setCastShadows(castShadows);
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent& arg )
{
	if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

	if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
	{
		mTrayMgr->toggleAdvancedFrameStats();
	}
	else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
	{
		if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
		{
			mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
			mDetailsPanel->show();
		}
		else
		{
			mTrayMgr->removeWidgetFromTray(mDetailsPanel);
			mDetailsPanel->hide();
		}
	}
	else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
	{
		String newVal;
		TextureFilterOptions tfo;
		unsigned int aniso;

		switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
		{
		case 'B':
			newVal = "Trilinear";
			tfo = TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'A':
			newVal = "None";
			tfo = TFO_NONE;
			aniso = 1;
			break;
		default:
			newVal = "Bilinear";
			tfo = TFO_BILINEAR;
			aniso = 1;
		}

		MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		mDetailsPanel->setParamValue(9, newVal);
	}
	else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
	{
		String newVal;
		PolygonMode pm;

		switch (mCamera->getPolygonMode())
		{
		case PM_SOLID:
			newVal = "Wireframe";
			pm = PM_WIREFRAME;
			break;
		case PM_WIREFRAME:
			newVal = "Points";
			pm = PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = PM_SOLID;
		}

		mCamera->setPolygonMode(pm);
		mDetailsPanel->setParamValue(10, newVal);
	}
	else if (arg.key == OIS::KC_F5)  // refresh all textures
	{
		TextureManager::getSingleton().reloadAll();
	}
	else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	else if (arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
	}
	else if (arg.key == OIS::KC_O)   // switch between standard and orthographic projection
	{
		if (mCamera->getProjectionType() == PT_PERSPECTIVE)
		{
			mCamera->setProjectionType(PT_ORTHOGRAPHIC);
			mCamera->setOrthoWindow(100,100);
		}
		else
			mCamera->setProjectionType(PT_PERSPECTIVE);
	}

	mCameraMan->injectKeyDown(arg);
	return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent& arg )
{
	mCameraMan->injectKeyUp(arg);
	return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent& arg )
{
#if OGRE_VERSION >= ((1 << 16) | (10 << 8) | 0) && OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	if (mTrayMgr->injectPointerMove(arg)) return true;
	if (!mNonExclusiveMouse || mMouse->getMouseState().buttonDown(OIS::MB_Left))
		mCameraMan->injectPointerMove(arg);
#else
	if (mTrayMgr->injectMouseMove(arg)) return true;
	if (!mNonExclusiveMouse || mMouse->getMouseState().buttonDown(OIS::MB_Left))
		mCameraMan->injectMouseMove(arg);
#endif
	return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	std::cout<<"mouse presse"<<std::endl;
#if OGRE_VERSION >= ((1 << 16) | (10 << 8) | 0) && OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	if (mTrayMgr->injectPointerDown(arg, id)) return true;
	mCameraMan->injectPointerDown(arg, id);
#else
	if (mTrayMgr->injectMouseDown(arg, id)) return true;
	mCameraMan->injectMouseDown(arg, id);
#endif
	return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	std::cout<<"mouse rele"<<std::endl;
#if OGRE_VERSION >= ((1 << 16) | (10 << 8) | 0) && OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	if (mTrayMgr->injectPointerUp(arg, id)) return true;
	mCameraMan->injectPointerUp(arg, id);
#else
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	mCameraMan->injectMouseUp(arg, id);
#endif
	return true;
}

void BaseApplication::createLogManager(void)
{
}


//Adjust mouse clipping area
void BaseApplication::windowResized(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if ( rw == mWindow )
	{
		if ( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}
