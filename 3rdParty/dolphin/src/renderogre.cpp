/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "renderogre.h"

#include "inputois.h"
#include "gui.h"
#include "cameramanager.h"
#include "scenecomponents.h"
#include "ocean/ocean.h"
#include "engine.h"


Ogre::Camera* Dolphin::RenderOgre::mCamera = 0;
Ogre::RenderWindow*  Dolphin::RenderOgre::mWindow = 0;
OgreBites::SdkCameraMan*  Dolphin::RenderOgre::mCameraMan = 0;      // basic camera controller
Ogre::Root* Dolphin::RenderOgre::mRoot = 0;
bool  Dolphin::RenderOgre::mShutDown = 0;
Ogre::SceneManager* Dolphin::RenderOgre::mSceneMgr = 0;

//-------------------------------------------------------------------------------------
Dolphin::RenderOgre::RenderOgre()
// mRoot(0),
// mCamera(0),
//:mSceneMgr(0),
//mWindow(0),
:mResourcesCfg(Ogre::BLANKSTRING),
mPluginsCfg(Ogre::BLANKSTRING),
//mCameraMan(0),
mCursorWasVisible(false),
//mShutDown(false),
mOverlaySystem(0),
terrain(0)
{
}
//-------------------------------------------------------------------------------------
Dolphin::RenderOgre::~RenderOgre(void)
{
	if (mCameraMan) delete mCameraMan;
	if (mOverlaySystem) delete mOverlaySystem;
	if (cameramanager) delete cameramanager;
	Dolphin::Gui::GetSingleton().DestroyGui();
	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

void Dolphin::RenderOgre::SetTerrain(Terrain* tr)
{
	terrain = tr;
}

void Dolphin::RenderOgre::SetPhysics(Physics* ph)
{
	physics = ph;
}


Ogre::Root* Dolphin::RenderOgre::GetOgreRoot()
{
	return mRoot;
}

Ogre::RenderWindow* Dolphin::RenderOgre::getmWindow()
{
	return mWindow;
}

OgreBites::SdkCameraMan* Dolphin::RenderOgre::getmCameraMan()
{
	return mCameraMan;
}

Ogre::Camera* Dolphin::RenderOgre::getmCamera()
{
	return mCamera;
}

bool Dolphin::RenderOgre::IsShutDown()
{
	return mShutDown;
}

void Dolphin::RenderOgre::SetShutDown(bool b)
{
	mShutDown = b;
}

Ogre::SceneManager* Dolphin::RenderOgre::GetSceneManager()
{
	return mSceneMgr;
}

bool Dolphin::RenderOgre::Initialize(void)
{
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif

	// construct Ogre::Root
	mRoot = new Ogre::Root(mPluginsCfg);

	//-------------------------------------------------------------------------------------
	// setup resources
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
	//-------------------------------------------------------------------------------------
	// configure
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if (mRoot->showConfigDialog()) //mRoot->restoreConfig() || mRoot->showConfigDialog()
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "Dolphin framework system");
	}
	else
	{
		return false;
	}
	//-------------------------------------------------------------------------------------
	// choose scenemanager
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
	//-------------------------------------------------------------------------------------
	// initialize the OverlaySystem (changed for 1.9)
	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);
	//-------------------------------------------------------------------------------------
	// create camera
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	mCamera->setNearClipDistance(0.1);

	mCamera->setFarClipDistance(30000);
	mCamera->setPosition(0, 60, 60);
	mCamera->lookAt(Ogre::Vector3(0, 60, 0));

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
	mCameraMan->setTopSpeed(8);
	cameramanager = new CameraManager(mCameraMan);		 //for dolphin plugin logic

	//-------------------------------------------------------------------------------------
	// create viewports
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	//-------------------------------------------------------------------------------------
	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//-------------------------------------------------------------------------------------
	// Create any resource listeners (for loading screens)
	// createResourceListener();
	//-------------------------------------------------------------------------------------
	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//-------------------------------------------------------------------------------------
	
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	// Create a light
	mSceneMgr->setSkyBox(true, Ocean::mSkyBoxes[Ocean::mCurrentSkyBox], 99999 * 3, true);

	// Light
	
	/*mLight->setSpecularColour(Ocean::mSunColor[Ocean::mCurrentSkyBox].x,
		Ocean::mSunColor[Ocean::mCurrentSkyBox].y,
		Ocean::mSunColor[Ocean::mCurrentSkyBox].z);*/
	/*Ogre::Light* l = mSceneMgr->createLight("MainLight");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(-1, -1, -1);
	l->setDiffuseColour(Ogre::ColourValue(1., 1., 1., 1.));
	l->setSpecularColour(Ogre::ColourValue(1., 1., 1., 1.));
	l->setPowerScale(0.5);
	l->setCastShadows(true);*/
	//-------------------------------------------------------------------------------------
	//set shadow
	//mSceneMgr->setShadowColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
	//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	//mSceneMgr->setShadowFarDistance(10000);

	//create FrameListener

	/*Ogre::Light* sunlight = RenderOgre::GetSceneManager()->createLight("sunlight");
	sunlight->setType(Ogre::Light::LT_DIRECTIONAL);

	sunlight->setDirection(-1, -1, -1);
	sunlight->setDiffuseColour(Ogre::ColourValue(0.5, 0.5, 0.5, 1.));
	sunlight->setSpecularColour(Ogre::ColourValue(0.5, 0.5, 0.5, 1.));
	sunlight->setCastShadows(false);

	Ogre::Light* sunlight2 = RenderOgre::GetSceneManager()->createLight("sunlight2");
	sunlight2->setType(Ogre::Light::LT_DIRECTIONAL);

	sunlight2->setDirection(1, -1, 1);
	sunlight2->setDiffuseColour(Ogre::ColourValue(0.5, 0.5, 0.5, 1.));
	sunlight2->setSpecularColour(Ogre::ColourValue(0.5, 0.5, 0.5, 1.));*/
	


	Dolphin::InputOis::GetSingleton().InitializeOIS(mWindow);

	Dolphin::Gui::GetSingleton().InitializeGui();

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
	//-------------------------------------------------------------------------------------
	return true;
}

bool Dolphin::RenderOgre::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mWindow->isClosed())
		return false;

	if (mShutDown)
		return false;

	//Need to capture/update each device
	Dolphin::InputOis::GetSingleton().CaptureKeyboard();
	Dolphin::InputOis::GetSingleton().CaptureMouse();
	
	if (terrain)
		terrain->Update(evt);
	
	Dolphin::Gui::GetSingleton().UpdateGui(evt);
	//physics->mDebugDrawer->Update();
	mCameraMan->frameRenderingQueued(evt);

	return true;
}


bool Dolphin::RenderOgre::frameStarted(const Ogre::FrameEvent& evt)
{
	Ogre::RaySceneQuery *raySceneQuery =
		mSceneMgr->
		createRayQuery(Ogre::Ray(mCamera->getPosition() + Ogre::Vector3(0, 1000000, 0),
		Ogre::Vector3::NEGATIVE_UNIT_Y));
	Ogre::RaySceneQueryResult& qryResult = raySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator i = qryResult.begin();
	if (i != qryResult.end() && i->worldFragment)
	{
		if (mCamera->getPosition().y < i->worldFragment->singleIntersection.y + 30)
		{
			mCamera->
				setPosition(mCamera->getPosition().x,
				i->worldFragment->singleIntersection.y + 30,
				mCamera->getPosition().z);
		}
	}

	delete raySceneQuery;
	Engine::GetSky()->UpdateLights();
	Engine::GetOcean()->Update();
	static_cast<OceanSceneComponent*>(Engine::GetSceneComponents()->back())->GetHydrax()->update(evt.timeSinceLastFrame);
	
	static_cast<SkySceneComponent*>(Engine::GetSceneComponents()->at(Engine::GetSceneComponents()->size() - 2))->GetSkyX()->update(evt.timeSinceLastFrame);

	return true;
}

void Dolphin::RenderOgre::SetDefaultTextureFiltering(int tfo)
{
	Ogre::TextureFilterOptions otfo = static_cast<Ogre::TextureFilterOptions>(tfo);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(otfo);

}
void Dolphin::RenderOgre::SetDefaultAnisotropy(int aniso)
{
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
}

int Dolphin::RenderOgre::GetPolygonMode()
{
	return mCamera->getPolygonMode();
}

void Dolphin::RenderOgre::SetPolygonMode(int pm)
{
	Ogre::PolygonMode opm = static_cast<Ogre::PolygonMode>(pm);
	mCamera->setPolygonMode(opm);
}

void Dolphin::RenderOgre::RefreshAllTexture()
{
	Ogre::TextureManager::getSingleton().reloadAll();
}

void Dolphin::RenderOgre::TakeScreeShot()
{
	mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
}

//Adjust mouse clipping area
void Dolphin::RenderOgre::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = Dolphin::InputOis::GetSingleton().GetMouse()->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void Dolphin::RenderOgre::windowClosed(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if (rw == mWindow)
	{
		Dolphin::InputOis::GetSingleton().DestroyInputs();
		
	}
}