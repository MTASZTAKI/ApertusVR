/*
--------------------------------------------------------------------------------
This source file is part of Hydrax.
Visit ---

Copyright (C) 2008 Xavier Verguín González <xavierverguin@hotmail.com>
                       <xavyiy@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Contributors:
  Jose Luis Cercós Pita <jlcercos@alumnos.upm.es>
--------------------------------------------------------------------------------
*/

#pragma warning(disable:4355)

#include <iostream>
#include <Hydrax.h>

namespace Hydrax
{

  Hydrax::Hydrax(Ogre::SceneManager *sm, Ogre::Viewport *v)
  {
    // Initialisation
    mSceneManager = sm;
    mViewportList.insert(v);

    mCreated = false;
    mVisible = true;
    mPolygonMode = Ogre::PM_SOLID;
    mPosition = Ogre::Vector3(0, 0, 0);
    mPlanesError = 0;
    mFullReflectionDistance = 99999997952.0;
    mGlobalTransparency = 0.05;
    mWaterColor = Ogre::Vector3(0, 0.1, 0.172);
    mNormalDistortion = 0.09;
    mSunPosition = Ogre::Vector3(5000, 3000, 1);
    mSunStrength = 1.75;
    mSunArea = 150;
    mSunColor = Ogre::Vector3(1, 0.75, 0.25);
    mFoamMaxDistance = 75000000.0;
    mFoamScale = 0.0075;
    mFoamStart = 0;
    mFoamTransparency = 1;
    mDepthLimit = 0;
    mDistLimit = 0;
    mSmoothPower = 30.0f;
    mCausticsScale = 10.0f;
    mCausticsPower = 1.0f;
    mCausticsEnd = 0.55f;
    mGodRaysExposure = Ogre::Vector3(0.76, 2.46, 2.29);
    mGodRaysIntensity = 0.015;
    mUnderwaterCameraSwitchDelta = 1.25f;
    mCurrentFrameUnderwater = false;
    mMesh = new Mesh(this);
    mMaterialManager = new MaterialManager(this);
    mRttManager = new RttManager(this);
    mTextureManager = new TextureManager(this);
    mGodRaysManager = new GodRaysManager(this);
    mDecalsManager = new DecalsManager(this);
    mGPUNormalMapManager = new GPUNormalMapManager(this);
    mCfgFileManager = new CfgFileManager(this);
    mModule = 0;
    mComponents = HYDRAX_COMPONENTS_NONE;
    mTimeMultiplier = 1.0f;
    mTimeSinceLastFrame = 0.0f;
    rttVisibilityMask = 0xFFFFFFFF;
	mCamera = v->getCamera(); //mSceneManager->createCamera(Ogre::String("Hydrax/Camera"));

   /* Ogre::Camera* cam = v->getCamera();
    if (cam)
    {
      mCamera->setPosition(cam->getDerivedPosition());
      mCamera->setOrientation(cam->getDerivedOrientation());
      mCamera->setProjectionType(cam->getProjectionType());
      mCamera->setAspectRatio(cam->getAspectRatio());
      mCamera->setNearClipDistance(cam->getNearClipDistance());
      mCamera->setFarClipDistance(cam->getFarClipDistance());
      mCamera->setUseRenderingDistance(cam->getUseRenderingDistance());
      mCamera->setFOVy(cam->getFOVy());
      mCamera->setFocalLength(cam->getFocalLength());
    }*/

    mDeviceLost = false;
	//mRttManager->setAutoUpdate(false);
    HydraxLOG("Hydrax created.");
  }

  Hydrax::~Hydrax()
  {
    remove();

    if (mModule)
      delete mModule;

    delete mTextureManager;
    delete mMaterialManager;
    delete mGPUNormalMapManager;
    delete mDecalsManager;
    delete mGodRaysManager;
    delete mRttManager;
    delete mCfgFileManager;
    delete mMesh;
    mSceneManager->destroyCamera(mCamera->getName());

    HydraxLOG("Hydrax object removed.");
  }

  void Hydrax::create()
  {
    if (!mModule)
    {
      HydraxLOG("Module isn't set, skipping...");
      return;
    }

    if (mCreated)
    {
      HydraxLOG("Hydrax is alredy created, skipping...");
      return;
    }

    HydraxLOG("Creating module...");
    mModule->create();
    if (mModule->getNormalMode() == MaterialManager::NM_RTT)
    {
      if (!mModule->getNoise()->createGPUNormalMapResources(mGPUNormalMapManager))
      {
        HydraxLOG(mModule->getNoise()->getName() + " doesn't support GPU Normal map generation.");
      }
    }
    else
    {
      if (mModule->getNoise()->areGPUNormalMapResourcesCreated())
      {
        mModule->getNoise()->removeGPUNormalMapResources(mGPUNormalMapManager);
      }
    }
    HydraxLOG("Module created.");

    HydraxLOG("Initializating RTT Manager...");
    mRttManager->initialize(RttManager::RTT_REFLECTION);
    mRttManager->initialize(RttManager::RTT_REFRACTION);
    if (isComponent(HYDRAX_COMPONENT_DEPTH))
    {
      mRttManager->initialize(RttManager::RTT_DEPTH);
    }
    HydraxLOG("RTT manager initialized.");

    HydraxLOG("Registring device restored listener...");
    mDeviceListener.mHydrax = this;
    Ogre::Root::getSingleton().getRenderSystem()->addListener(&mDeviceListener);
    HydraxLOG("Device restored listener registred.");

    HydraxLOG("Creating materials...");
    mMaterialManager->createMaterials(mComponents, mModule->getNormalMode());
    mMesh->setMaterialName(mMaterialManager->getMaterial(MaterialManager::MAT_WATER)->getName());
    HydraxLOG("Materials created.");

    if (isComponent(HYDRAX_COMPONENT_UNDERWATER_GODRAYS))
    {
      HydraxLOG("Creating god rays...");
      mGodRaysManager->create(mComponents);
      HydraxLOG("God rays created.");
    }

    HydraxLOG("Creating water mesh...");
    mMesh->setOptions(mModule->getMeshOptions());
    mMesh->create();
    HydraxLOG("Water mesh created.");

    mCreated = true;

    // Hide if !mVisible
    _checkVisible();

    // Registering listeners
    ViewportList::iterator iViewportSearched = mViewportList.begin();
    while (iViewportSearched != mViewportList.end())
    {
	  mMaterialManager->addCompositor((*iViewportSearched));
      (*iViewportSearched)->getTarget()->insertListener(this, 0);
      iViewportSearched++;
    }

  }

  void Hydrax::remove()
  {
    if (!mCreated)
    {
      return;
    }

    ViewportList::iterator iViewportSearched = mViewportList.begin();
    while (iViewportSearched != mViewportList.end())
    {
      (*iViewportSearched)->getTarget()->removeListener(this);
      iViewportSearched++;
    }

    Ogre::Root::getSingleton().getRenderSystem()->removeListener(&mDeviceListener);
    mMesh->remove();
    mDecalsManager->removeAll();
    mMaterialManager->removeMaterials();
    mRttManager->removeAll();
    mGodRaysManager->remove();
    mModule->remove();
    mTextureManager->remove();
    mCreated = false;
  }

  void Hydrax::setVisible(const bool& Visible)
  {
    mVisible = Visible;
    _checkVisible();
  }

  void Hydrax::_checkVisible()
  {
    if (!mCreated)
    {
      return;
    }

    if (!mVisible)
    {
      // Stop RTTs:
      mRttManager->removeAll();

      // Hide hydrax mesh
      mMesh->getSceneNode()->setVisible(false);

      // Stop compositor (MaterialManager::setCompositorEnable(...) checks if underwater compositor exists)
      mMaterialManager->setCompositorEnable(MaterialManager::COMP_UNDERWATER, false);
    }
    else
    {
      // Start reflection and refraction RTTs:
      mRttManager->initialize(RttManager::RTT_REFLECTION);
      mRttManager->initialize(RttManager::RTT_REFRACTION);

      // Start depth rtt if needed:
      if (isComponent(HYDRAX_COMPONENT_DEPTH))
      {
        mRttManager->initialize(RttManager::RTT_DEPTH);
      }

      // Start GPU normals rtt if needed:
      if (mModule->getNormalMode() == MaterialManager::NM_RTT)
      {
        mGPUNormalMapManager->create();
      }

      // Set over-water material and check for underwater:
      Ogre::MaterialPtr mat = mMaterialManager->getMaterial(MaterialManager::MAT_WATER);
      if (!mat.isNull())
        mMesh->setMaterialName(mat->getName());

      mMaterialManager->reload(MaterialManager::MAT_WATER);

      ViewportList::iterator iViewportSearched = mViewportList.begin();
      while (iViewportSearched != mViewportList.end())
      {
        Ogre::Camera* cam = (*iViewportSearched)->getCamera();
        if (cam)
          _checkUnderwater(cam, 0);

        iViewportSearched++;
      }      
      mMesh->getSceneNode()->setVisible(true);
    }
  }

  void Hydrax::registerViewport(Ogre::Viewport* viewport)
  {
    ViewportList::iterator iViewportSearched = mViewportList.find(viewport);
    if (iViewportSearched == mViewportList.end())
    {
      mViewportList.insert(viewport);

      if (mCreated)
      {
       /* Ogre::Camera* cam = viewport->getCamera();
        if (cam)
        {
          mCamera->setPosition(cam->getDerivedPosition());
          mCamera->setOrientation(cam->getDerivedOrientation());
          mCamera->setProjectionType(cam->getProjectionType());
          mCamera->setAspectRatio(cam->getAspectRatio());
          mCamera->setNearClipDistance(cam->getNearClipDistance());
          mCamera->setFarClipDistance(cam->getFarClipDistance());
          mCamera->setUseRenderingDistance(cam->getUseRenderingDistance());
          mCamera->setFOVy(cam->getFOVy());
          mCamera->setFocalLength(cam->getFocalLength());*/

          mMaterialManager->addCompositor(viewport);
          viewport->getTarget()->insertListener(this, 0);

        /*  _checkUnderwater(cam, 0);
        }*/
      }
    }
  }

  void Hydrax::unregisterViewport(Ogre::Viewport* viewport)
  {
    ViewportList::iterator iViewportSearched = mViewportList.find(viewport);
    if (iViewportSearched != mViewportList.end())
    {
      if (mCreated)
      {
        viewport->getTarget()->removeListener(this);
        mMaterialManager->removeCompositor(viewport);;
      }
      mViewportList.erase(iViewportSearched);
    }
  }

  void Hydrax::DeviceListener::eventOccurred(const Ogre::String& eventName, const Ogre::NameValuePairList *parameters)
  {
    // If needed...
    if (eventName == "DeviceLost")
    {
      mHydrax->mDeviceLost = true;
    }
    else if (eventName == "DeviceRestored")
    {
      mHydrax->mDeviceLost = false;
    }
  }

  void Hydrax::setPolygonMode(const Ogre::PolygonMode& PM)
  {
    if (!mCreated)
    {
      return;
    }

    mMaterialManager->getMaterial(MaterialManager::MAT_WATER)->getTechnique(0)->getPass(0)->setPolygonMode(PM);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->getMaterial(MaterialManager::MAT_UNDERWATER)->getTechnique(0)->getPass(0)->setPolygonMode(PM);
    }
    mPolygonMode = PM;
  }

  void Hydrax::setComponents(const HydraxComponent &Components)
  {
    mComponents = Components;

    //adjust components dependencies
    if (isComponent(HYDRAX_COMPONENT_SMOOTH) && !isComponent(HYDRAX_COMPONENT_DEPTH))
      mComponents = static_cast<HydraxComponent>(mComponents & ~(HYDRAX_COMPONENT_SMOOTH));

    if (isComponent(HYDRAX_COMPONENT_CAUSTICS) && !isComponent(HYDRAX_COMPONENT_DEPTH))
      mComponents = static_cast<HydraxComponent>(mComponents & ~(HYDRAX_COMPONENT_CAUSTICS));

    if (isComponent(HYDRAX_COMPONENT_UNDERWATER_GODRAYS) && (!isComponent(HYDRAX_COMPONENT_DEPTH) || !isComponent(HYDRAX_COMPONENT_SUN) || !isComponent(HYDRAX_COMPONENT_UNDERWATER)))
      mComponents = static_cast<HydraxComponent>(mComponents & ~(HYDRAX_COMPONENT_UNDERWATER_GODRAYS));

    if (isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS) && !isComponent(HYDRAX_COMPONENT_UNDERWATER))
      mComponents = static_cast<HydraxComponent>(mComponents & ~(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS));

    // do not continue if not enable
    if (!mCreated || !mModule)
      return;

    //reset Material
    mDecalsManager->removeAll();
    mMaterialManager->removeMaterials();
    mRttManager->removeAll();
    mTextureManager->remove();
    mGPUNormalMapManager->remove();

    if (!isComponent(HYDRAX_COMPONENT_UNDERWATER))
      mCurrentFrameUnderwater = false;

    if (isComponent(HYDRAX_COMPONENT_UNDERWATER_GODRAYS))
    {
      mGodRaysManager->create(mComponents);
    }
    else
    {
      mGodRaysManager->remove();
    }

    //add base RTT
    mRttManager->initialize(RttManager::RTT_REFLECTION);
    mRttManager->initialize(RttManager::RTT_REFRACTION);

    // Check for Rtt's
    /*if (isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS))
    {
        mRttManager->initialize(RttManager::RTT_REFLECTION);
    }*/

    if (isComponent(HYDRAX_COMPONENT_DEPTH))
    {
      mRttManager->initialize(RttManager::RTT_DEPTH);

      if (isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS) && mCurrentFrameUnderwater)
      {
        mRttManager->initialize(RttManager::RTT_DEPTH_REFLECTION);
      }
    }

    if (!isComponent(HYDRAX_COMPONENT_UNDERWATER) && mCurrentFrameUnderwater)
    {
      mRttManager->getTexture(RttManager::RTT_REFRACTION)->getBuffer()->getRenderTarget()->getViewport(0)->setSkiesEnabled(false);
      mRttManager->getTexture(RttManager::RTT_REFLECTION)->getBuffer()->getRenderTarget()->getViewport(0)->setSkiesEnabled(true);
    }

    mMesh->setMaterialName("BaseWhiteNoLighting");
    mMaterialManager->createMaterials(mComponents, mModule->getNormalMode());

    Ogre::MaterialPtr mat = mMaterialManager->getMaterial(mCurrentFrameUnderwater ? MaterialManager::MAT_UNDERWATER : MaterialManager::MAT_WATER);
    if (!mat.isNull())
      mMesh->setMaterialName(mat->getName());

    if (mCurrentFrameUnderwater)
      mMaterialManager->setCompositorEnable(MaterialManager::COMP_UNDERWATER, true);
  }

  void Hydrax::setModule(Module::Module* Module, const bool& DeleteOldModule)
  {
    if (mModule)
    {
      if (mModule->getNormalMode() != Module->getNormalMode())
      {
        mMaterialManager->createMaterials(mComponents, Module->getNormalMode());
        mMesh->setMaterialName(mMaterialManager->getMaterial(MaterialManager::MAT_WATER)->getName());
      }

      if (mModule->getNormalMode() == MaterialManager::NM_RTT && mModule->isCreated() && mModule->getNoise()->areGPUNormalMapResourcesCreated())
      {
        mModule->getNoise()->removeGPUNormalMapResources(mGPUNormalMapManager);
      }

      if (DeleteOldModule)
      {
        delete mModule;
      }
      else
      {
        mModule->remove();
      }

      // Due to modules can change -internally- scene nodes position,
      // just reset them to the original position.
      setPosition(mPosition);
    }

    mModule = Module;
    if (mCreated)
    {
      if (!mModule->isCreated())
      {
        mModule->create();
        if (mModule->getNormalMode() == MaterialManager::NM_RTT)
        {
          if (!mModule->getNoise()->createGPUNormalMapResources(mGPUNormalMapManager))
          {
            HydraxLOG(mModule->getNoise()->getName() + " doesn't support GPU Normal map generation.");
          }
        }
      }
      else
      {
        if (mModule->getNormalMode() == MaterialManager::NM_RTT)
        {
          if (!mModule->getNoise()->areGPUNormalMapResourcesCreated())
          {
            if (!mModule->getNoise()->createGPUNormalMapResources(mGPUNormalMapManager))
            {
              HydraxLOG(mModule->getNoise()->getName() + " doesn't support GPU Normal map generation.");
            }
          }
        }
        else
        {
          if (mModule->getNoise()->areGPUNormalMapResourcesCreated())
          {
            mModule->getNoise()->removeGPUNormalMapResources(mGPUNormalMapManager);
          }
        }
      }

      HydraxLOG("Updating water mesh...");
      Ogre::String MaterialNameTmp = mMesh->getMaterialName();

      HydraxLOG("Deleting water mesh...");
      mMesh->remove();
      HydraxLOG("Water mesh deleted.");

      HydraxLOG("Creating water mesh...");
      mMesh->setOptions(mModule->getMeshOptions());
      mMesh->setMaterialName(MaterialNameTmp);
      mMesh->create();
      setPosition(mPosition);
      HydraxLOG("Water mesh created.");

      HydraxLOG("Module set.");
    }
  }

  bool Hydrax::isComponent(const HydraxComponent &Component)
  {
    if (mComponents & Component)
    {
      return true;
    }

    if (Component == HYDRAX_COMPONENTS_NONE && mComponents == HYDRAX_COMPONENTS_NONE)
    {
      return true;
    }

    if (Component == HYDRAX_COMPONENTS_ALL && mComponents == HYDRAX_COMPONENTS_ALL)
    {
      return true;
    }

    return false;
  }

  void Hydrax::_checkUnderwater(const Ogre::Camera* cam, const Ogre::Real& timeSinceLastFrame)
  {
    if (!isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mCurrentFrameUnderwater = false;
      return;
    }

    // If the camera is under the current water x/z position
    if (getHeigth(cam->getDerivedPosition()) > cam->getDerivedPosition().y - mUnderwaterCameraSwitchDelta)
    {
      mCurrentFrameUnderwater = true;

      Ogre::MaterialPtr mat = mMaterialManager->getMaterial(MaterialManager::MAT_UNDERWATER);
      if (!mat.isNull() && mMesh->getMaterialName() != mat->getName())
      {
        mRttManager->getTexture(RttManager::RTT_REFRACTION)->getBuffer()->getRenderTarget()->getViewport(0)->setSkiesEnabled(true);
        if (isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS))
        {
          mRttManager->getTexture(RttManager::RTT_REFLECTION)->getBuffer()->getRenderTarget()->getViewport(0)->setSkiesEnabled(false);
          if (isComponent(HYDRAX_COMPONENT_DEPTH))
          {
            mRttManager->initialize(RttManager::RTT_DEPTH_REFLECTION);
          }
        }
        else
        {
          mRttManager->remove(RttManager::RTT_REFLECTION);
        }

        if (isComponent(HYDRAX_COMPONENT_DEPTH) && isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS))
        {
          mRttManager->initialize(RttManager::RTT_DEPTH_REFLECTION);
        }

        mMaterialManager->reload(MaterialManager::MAT_UNDERWATER);
        mMaterialManager->setCompositorEnable(MaterialManager::COMP_UNDERWATER, true);
        mMesh->setMaterialName(mat->getName());
      }

      // Update god rays
      if (isComponent(HYDRAX_COMPONENT_UNDERWATER_GODRAYS))
      {
        mGodRaysManager->update(timeSinceLastFrame);
      }
    }
    else
    {
      mCurrentFrameUnderwater = false;

      Ogre::MaterialPtr mat = mMaterialManager->getMaterial(MaterialManager::MAT_WATER);
      if (!mat.isNull() && mMesh->getMaterialName() != mat->getName())
      {
        // We asume that RefractionRtt/ReflectionRtt are initialized
        mRttManager->getTexture(RttManager::RTT_REFRACTION)->getBuffer()->getRenderTarget()->getViewport(0)->setSkiesEnabled(false);
        if (!isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS))
        {
          mRttManager->initialize(RttManager::RTT_REFLECTION);
          mMaterialManager->reload(MaterialManager::MAT_WATER);
        }

        mRttManager->getTexture(RttManager::RTT_REFLECTION)->getBuffer()->getRenderTarget()->getViewport(0)->setSkiesEnabled(true);
        if (isComponent(HYDRAX_COMPONENT_DEPTH) && isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS))
        {
          mRttManager->remove(RttManager::RTT_DEPTH_REFLECTION);
        }

        mMaterialManager->setCompositorEnable(MaterialManager::COMP_UNDERWATER, false);
        mMesh->setMaterialName(mat->getName());
      }
    }
  }

  void Hydrax::setPosition(const Ogre::Vector3 &Position)
  {
    mPosition = Position;

    // For world-space -> object-space conversion
    setSunPosition(mSunPosition);

    if (!mCreated)
      return;

    if (isComponent(HYDRAX_COMPONENT_DEPTH))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_VERTEX, MaterialManager::MAT_DEPTH, "uPlaneYPos", Position.y);
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_VERTEX, MaterialManager::MAT_DEPTH_TRANS, "uPlaneYPos", Position.y);
    }

    mMesh->getSceneNode()->setPosition(Position.x - mMesh->getSize().Width / 2, Position.y, Position.z - mMesh->getSize().Height / 2);
    mRttManager->getPlanesSceneNode()->setPosition(Position);
  }

  void Hydrax::rotate(const Ogre::Quaternion &q)
  {
    if (!mCreated)
    {
      HydraxLOG("Hydrax::rotate(...) must be called after Hydrax::create(), skipping...");
      return;
    }

    mMesh->getSceneNode()->rotate(q);
    mRttManager->getPlanesSceneNode()->rotate(q);

    // For world-space -> object-space conversion
    setSunPosition(mSunPosition);
  }

  void Hydrax::setPlanesError(const Ogre::Real &PlanesError)
  {
    mPlanesError = PlanesError;
  }

  void Hydrax::_setStrength(const Ogre::Real &Strength)
  {
    if (isComponent(HYDRAX_COMPONENT_FOAM))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uFoamRange", Strength);
    }

    mDecalsManager->_setWaterStrength(Strength);
  }

  void Hydrax::setFullReflectionDistance(const Ogre::Real &FullReflectionDistance)
  {
    mFullReflectionDistance = FullReflectionDistance;

    if (!mCreated)
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uFullReflectionDistance", FullReflectionDistance);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uFullReflectionDistance", FullReflectionDistance);
    }
  }

  void Hydrax::setGlobalTransparency(const Ogre::Real &GlobalTransparency)
  {
    mGlobalTransparency = GlobalTransparency;

    if (!mCreated)
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uGlobalTransparency", GlobalTransparency);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uGlobalTransparency", GlobalTransparency);
    }
  }

  void Hydrax::setWaterColor(const Ogre::Vector3 &WaterColor)
  {
    mWaterColor = WaterColor;

    if (!mCreated)
      return;

    Ogre::TexturePtr tex;
    Ogre::ColourValue WC = Ogre::ColourValue(WaterColor.x, WaterColor.y, WaterColor.z);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS) || !_isCurrentFrameUnderwater())
    {
      tex = mRttManager->getTexture(RttManager::RTT_REFLECTION);
      if (!tex.isNull())
      {
        tex->getBuffer()->getRenderTarget()->getViewport(0)->setBackgroundColour(WC);
        tex.setNull();
      }
    }
    tex = mRttManager->getTexture(RttManager::RTT_REFRACTION);
    if (!tex.isNull())
    {
      tex->getBuffer()->getRenderTarget()->getViewport(0)->setBackgroundColour(WC);
      tex.setNull();
    }

    if (isComponent(HYDRAX_COMPONENT_DEPTH))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uWaterColor", WaterColor);
    }

    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uWaterColor", WaterColor);

      //mMaterialManager->getCompositor(MaterialManager::COMP_UNDERWATER)->
      //	getTechnique(0)->getTargetPass(0)->getPass(0)->setClearColour(WC);

      /* Active creation/destruction
      if (getHeigth(mCamera->getDerivedPosition()) > mCamera->getDerivedPosition().y-1.25f)
      {
        if (mMaterialManager->isCompositorEnable(MaterialManager::COMP_UNDERWATER))
        {
          mMaterialManager->setCompositorEnable(MaterialManager::COMP_UNDERWATER, false);
          mMaterialManager->setCompositorEnable(MaterialManager::COMP_UNDERWATER, true);
        }
      }
      */
    }
  }


  void Hydrax::setNormalDistortion(const Ogre::Real &NormalDistortion)
  {
    mNormalDistortion = NormalDistortion;

    if (!mCreated)
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uNormalDistortion", NormalDistortion);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uNormalDistortion", NormalDistortion);
    }
  }

  void Hydrax::setSunPosition(const Ogre::Vector3 &SunPosition)
  {
    mSunPosition = SunPosition;

    if (!isComponent(HYDRAX_COMPONENT_SUN) || !isCreated())
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uSunPosition", mMesh->getObjectSpacePosition(SunPosition));
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uSunPosition", mMesh->getObjectSpacePosition(SunPosition));
    }
  }

  void Hydrax::setSunStrength(const Ogre::Real &SunStrength)
  {
    mSunStrength = SunStrength;

    if (!isComponent(HYDRAX_COMPONENT_SUN))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uSunStrength", SunStrength);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uSunStrength", SunStrength);
    }
  }

  void Hydrax::setSunArea(const Ogre::Real &SunArea)
  {
    mSunArea = SunArea;

    if (!isComponent(HYDRAX_COMPONENT_SUN))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uSunArea", SunArea);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uSunArea", SunArea);
    }
  }

  void Hydrax::setSunColor(const Ogre::Vector3 &SunColor)
  {
    mSunColor = SunColor;

    if (!isComponent(HYDRAX_COMPONENT_SUN))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uSunColor", SunColor);
    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uSunColor", SunColor);
    }
  }

  void Hydrax::setFoamMaxDistance(const Ogre::Real &FoamMaxDistance)
  {
    mFoamMaxDistance = FoamMaxDistance;

    if (!isComponent(HYDRAX_COMPONENT_FOAM))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uFoamMaxDistance", FoamMaxDistance);
  }

  void Hydrax::setFoamScale(const Ogre::Real &FoamScale)
  {
    mFoamScale = FoamScale;

    if (!isComponent(HYDRAX_COMPONENT_FOAM))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uFoamScale", FoamScale);
  }

  void Hydrax::setFoamStart(const Ogre::Real &FoamStart)
  {
    mFoamStart = FoamStart;

    if (!isComponent(HYDRAX_COMPONENT_FOAM))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uFoamStart", FoamStart);
  }

  void Hydrax::setFoamTransparency(const Ogre::Real &FoamTransparency)
  {
    mFoamTransparency = FoamTransparency;

    if (!isComponent(HYDRAX_COMPONENT_FOAM))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uFoamTransparency", FoamTransparency);
  }

  void Hydrax::setDepthLimit(const Ogre::Real &DepthLimit)
  {
    mDepthLimit = DepthLimit;
    if (mDepthLimit <= 0)
    {
      mDepthLimit = 1;
    }

    if (!isComponent(HYDRAX_COMPONENT_DEPTH))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH, "uDepthLimit", 1 / mDepthLimit);
    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH_TRANS, "uDepthLimit", 1 / mDepthLimit);
  }

  void Hydrax::setDistLimit(const Ogre::Real &DistLimit)
  {
    mDistLimit = DistLimit;
    if (mDistLimit <= 0)
    {
      mDistLimit = 1;
    }

    if (!isComponent(HYDRAX_COMPONENT_DEPTH))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH, "uDistLimit", 1 / mDistLimit);
    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH_TRANS, "uDistLimit", 1 / mDistLimit);
  }

  void Hydrax::setSmoothPower(const Ogre::Real &SmoothPower)
  {
    mSmoothPower = SmoothPower;

    if (!isComponent(HYDRAX_COMPONENT_SMOOTH))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uSmoothPower", SmoothPower);
  }

  void Hydrax::setCausticsScale(const Ogre::Real &CausticsScale)
  {
    mCausticsScale = CausticsScale;

    if (!isComponent(HYDRAX_COMPONENT_CAUSTICS))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH, "uCausticsScale", CausticsScale);
    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH_TRANS, "uCausticsScale", CausticsScale);
  }

  void Hydrax::setCausticsPower(const Ogre::Real &CausticsPower)
  {
    mCausticsPower = CausticsPower;

    if (!isComponent(HYDRAX_COMPONENT_CAUSTICS))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_WATER, "uCausticsPower", CausticsPower);

    if (isComponent(HYDRAX_COMPONENT_UNDERWATER))
    {
      if (isComponent(HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS))
      {
        mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER, "uCausticsPower", CausticsPower);
      }

      mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_UNDERWATER_COMPOSITOR, "uCausticsPower", CausticsPower);
    }
  }

  void Hydrax::setCausticsEnd(const Ogre::Real &CausticsEnd)
  {
    mCausticsEnd = CausticsEnd;

    if (!isComponent(HYDRAX_COMPONENT_CAUSTICS))
      return;

    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH, "uCausticsEnd", 1.0f - CausticsEnd);
    mMaterialManager->setGpuProgramParameter(MaterialManager::GPUP_FRAGMENT, MaterialManager::MAT_DEPTH_TRANS, "uCausticsEnd", 1.0f - CausticsEnd);
  }

  void Hydrax::update(const Ogre::Real& timeSinceLastFrame)
  {
    mTimeSinceLastFrame = timeSinceLastFrame;
  }

  void Hydrax::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
  {
    if (mCreated && mModule && mVisible)
    {
      //get the first camera
      Ogre::Camera* cam = evt.source->getViewport(0)->getCamera();
      if (!cam)
        return;

      //mCamera->synchroniseBaseSettingsWith(cam);
      //mCamera->setPosition(cam->getDerivedPosition());
      //mCamera->setOrientation(cam->getDerivedOrientation());

      // Update Hydrax
      mModule->update(mTimeSinceLastFrame * mTimeMultiplier, cam);
      mDecalsManager->update(cam);
      
      _checkUnderwater(cam, mTimeSinceLastFrame * mTimeMultiplier);
      mTimeSinceLastFrame = 0;

      if (!mRttManager->isAutoUpdated())
      {
        mRttManager->updateAll();
      }
    }
  }

  void Hydrax::setTimeMultiplier(const Ogre::Real& timeMultiplier)
  {
    mTimeMultiplier = timeMultiplier;
  }

  const Ogre::Real& Hydrax::getTimeMultiplier() const
  {
    return mTimeMultiplier;
  }

  void Hydrax::setRttVisibilityMask(const Ogre::uint32& visibilityMask)
  {
    rttVisibilityMask = visibilityMask;
  }

  const Ogre::uint32 Hydrax::getRttVisibilityMask() const
  {
    return rttVisibilityMask;
  }
}
