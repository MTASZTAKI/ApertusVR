/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "terrain.h"
#include "RenderOgre.h"
#include "sky/sky.h"
#include "TerrainMaterial.h"

Dolphin::Terrain::Terrain() : mTerrainGlobals(0)
, mTerrainGroup(0)
, mTerrainPaging(0)
, mPageManager(0)
, mFly(false)
, mFallVelocity(0)
, mMode(MODE_NORMAL)
, mLayerEdit(1)
, mBrushSizeTerrainSpace(0.02)
, mHeightUpdateCountDown(0)
, mTerrainPos(0, 0, 0)
, mTerrainsImported(false)
{
	// Update terrain at max 20fps
	mHeightUpdateRate = 1.0 / 20.0;
	

}

Dolphin::Terrain::~Terrain()
{
	
}

void Dolphin::Terrain::SetPhysics(Physics* _physics)
{
	physics = _physics;

	Ogre::Terrain* terrain = mTerrainGroup->getTerrain(0, 0);
	float* data = terrain->getHeightData();
	if (data)
	{
		Ogre::Real scale = terrain->getWorldSize() / (terrain->getSize() - 1);
		physics->GenerateTerrainPhysics(
			terrain->getSize(), data,
			terrain->getMinHeight(), terrain->getMaxHeight(),
			terrain->getPosition(), scale);
	}
}


void Dolphin::Terrain::Update(const Ogre::FrameEvent& evt)
{
	// clamp to terrain
	Ogre::Vector3 camPos = RenderOgre::getmCamera()->getPosition();
	Ogre::Ray ray;
	ray.setOrigin(Ogre::Vector3(camPos.x, mTerrainPos.y + 10000, camPos.z));
	ray.setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);

	Ogre::TerrainGroup::RayResult rayResult = mTerrainGroup->rayIntersects(ray);
	Ogre::Real distanceAboveTerrain = 2;
	Ogre::Real fallSpeed = 300;
	Ogre::Real newy = camPos.y;
	if (rayResult.hit)
	{
		if (camPos.y > rayResult.position.y + distanceAboveTerrain)
		{
			mFallVelocity += evt.timeSinceLastFrame * 20;
			mFallVelocity = std::min(mFallVelocity, fallSpeed);
			newy = camPos.y - mFallVelocity * evt.timeSinceLastFrame;
		}

		newy = std::max(rayResult.position.y + distanceAboveTerrain, newy);
		RenderOgre::getmCamera()->setPosition(camPos.x, newy, camPos.z);

	}

	if (mHeightUpdateCountDown > 0)
	{
		mHeightUpdateCountDown -= evt.timeSinceLastFrame;
		if (mHeightUpdateCountDown <= 0)
		{
			mTerrainGroup->update();
			mHeightUpdateCountDown = 0;

		}
	}

	if (!mTerrainGroup->isDerivedDataUpdateInProgress())
	{
		if (mTerrainsImported)
		{
			saveTerrains(true);
			mTerrainsImported = false;
		}
	}


}

void Dolphin::Terrain::saveTerrains(bool onlyIfModified)
{
	mTerrainGroup->saveAllTerrains(onlyIfModified);
}



Ogre::StringVector getRequiredPlugins()
{
	Ogre::StringVector names;
	if (!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsles") && !Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl150"))
		names.push_back("Cg Program Manager");
	return names;
}

void Dolphin::Terrain::defineTerrain(long x, long y, bool flat)
{
	// if a file is available, use it
	// if not, generate file from import

	// Usually in a real project you'll know whether the compact terrain data is
	// available or not; I'm doing it this way to save distribution size

	if (flat)
	{
		mTerrainGroup->defineTerrain(x, y, 0.0f);
	}
	else
	{
		Ogre::String filename = mTerrainGroup->generateFilename(x, y);
		if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
		{
			mTerrainGroup->defineTerrain(x, y);
		}
		else
		{
			Ogre::Image img;
			getTerrainImage(x % 2 != 0, y % 2 != 0, img);
			mTerrainGroup->defineTerrain(x, y, &img);
			mTerrainsImported = true;
		}
	}
}

void Dolphin::Terrain::getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("Terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();
}

void Dolphin::Terrain::initBlendMaps(Ogre::Terrain* terrain)
{
	Ogre::LogManager::getSingleton().logMessage("1");
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 40;
	Ogre::Real minHeight1 = 70;
	Ogre::Real fadeDist1 = 15;
	
	float* pBlend1 = blendMap1->getBlendPointer();
	
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
	
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
	
			Ogre::Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
	

		}
	}
	
	blendMap0->dirty();
	blendMap1->dirty();
	//blendMap0->loadImage("blendmap1.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	blendMap0->update();
	blendMap1->update();
	
	// set up a colour map
	/*
	if (!terrain->getGlobalColourMapEnabled())
	{
	terrain->setGlobalColourMapEnabled(true);
	Image colourMap;
	colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	terrain->getGlobalColourMap()->loadImage(colourMap);
	}
	*/
}

void Dolphin::Terrain::configureTerrainDefaults(Ogre::Light* l)
{
	// Configure global
	//mTerrainGlobals->setMaxPixelError(8);
	mTerrainGlobals->setMaxPixelError(1);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(30000);
	//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
	//mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
	//mTerrainGlobals->setLightMapSize(256);

#if OGRE_NO_GLES3_SUPPORT == 1
	// Disable the lightmap for OpenGL ES 2.0. The minimum number of samplers allowed is 8(as opposed to 16 on desktop).
	// Otherwise we will run over the limit by just one. The minimum was raised to 16 in GL ES 3.0.
	if (Ogre::Root::getSingletonPtr()->getRenderSystem()->getName().find("OpenGL ES 2") != Ogre::String::npos)
	{
		Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile =
			static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
		matProfile->setLightmapEnabled(false);
	}
#endif

	// Init custom materialgenerator
	Ogre::TerrainMaterialGeneratorPtr terrainMaterialGenerator;

	// Set Ogre Material  with the name "TerrainMaterial" in constructor
	TerrainMaterial *terrainMaterial = OGRE_NEW TerrainMaterial("TerrainMaterial");
	terrainMaterialGenerator.bind(terrainMaterial);

	mTerrainGlobals->setDefaultMaterialGenerator(terrainMaterialGenerator);

	terrainMaterial->setMaterialByName("Terrain");

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(RenderOgre::GetSceneManager()->getAmbientLight());
	//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
	mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());
	//mTerrainGlobals->
	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = TERRAIN_SIZE;
	defaultimp.worldSize = TERRAIN_WORLD_SIZE;
	//defaultimp.inputScale = 60; //600
	defaultimp.inputScale = 120;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	/*
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
	*/
}

Ogre::MaterialPtr Dolphin::Terrain::buildDepthShadowMaterial(const Ogre::String& textureName)
{
	Ogre::String matName = "DepthShadows/" + textureName;

	Ogre::MaterialPtr ret = Ogre::MaterialManager::getSingleton().getByName(matName);
	if (ret.isNull())
	{
		Ogre::MaterialPtr baseMat = Ogre::MaterialManager::getSingleton().getByName("Ogre/shadow/depth/integrated/pssm");
		ret = baseMat->clone(matName);
		Ogre::Pass* p = ret->getTechnique(0)->getPass(0);
		p->getTextureUnitState("diffuse")->setTextureName(textureName);

		Ogre::Vector4 splitPoints;
		const Ogre::PSSMShadowCameraSetup::SplitPointList& splitPointList =
			static_cast<Ogre::PSSMShadowCameraSetup*>(mPSSMSetup.get())->getSplitPoints();
		for (int i = 0; i < 3; ++i)
		{
			splitPoints[i] = splitPointList[i];
		}
		p->getFragmentProgramParameters()->setNamedConstant("pssmSplitPoints", splitPoints);
	}

	return ret;
}

void Dolphin::Terrain::changeShadows()
{
	//configureShadows(true , true);
	configureShadows(false, false);
}

void Dolphin::Terrain::configureShadows(bool enabled, bool depthShadows)
{
	Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile =
		static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setReceiveDynamicShadowsEnabled(enabled);
#ifdef SHADOWS_IN_LOW_LOD_MATERIAL
	matProfile->setReceiveDynamicShadowsLowLod(true);
#else
	matProfile->setReceiveDynamicShadowsLowLod(false);
#endif

	// Default materials
	/*for (EntityList::iterator i = mHouseList.begin(); i != mHouseList.end(); ++i)
	{
		(*i)->setMaterialName("Examples/TudorHouse");
	}*/

	if (enabled)
	{
		// General scene setup
		RenderOgre::GetSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		RenderOgre::GetSceneManager()->setShadowFarDistance(50000);

		// 3 textures per directional light (PSSM)
		RenderOgre::GetSceneManager()->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);

		if (mPSSMSetup.isNull())
		{
			// shadow camera setup
			Ogre::PSSMShadowCameraSetup* pssmSetup = new Ogre::PSSMShadowCameraSetup();
			pssmSetup->setSplitPadding(RenderOgre::getmCamera()->getNearClipDistance());
			pssmSetup->calculateSplitPoints(3, RenderOgre::getmCamera()->getNearClipDistance(), RenderOgre::GetSceneManager()->getShadowFarDistance());
			pssmSetup->setOptimalAdjustFactor(0, 2);
			pssmSetup->setOptimalAdjustFactor(1, 1);
			pssmSetup->setOptimalAdjustFactor(2, 0.5);

			mPSSMSetup.bind(pssmSetup);
		}
		RenderOgre::GetSceneManager()->setShadowCameraSetup(mPSSMSetup);

		if (depthShadows)
		{
			RenderOgre::GetSceneManager()->setShadowTextureCount(3);
			RenderOgre::GetSceneManager()->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_FLOAT32_R);
			RenderOgre::GetSceneManager()->setShadowTextureConfig(1, 1024, 1024, Ogre::PF_FLOAT32_R);
			RenderOgre::GetSceneManager()->setShadowTextureConfig(2, 1024, 1024, Ogre::PF_FLOAT32_R);
			RenderOgre::GetSceneManager()->setShadowTextureSelfShadow(true);
			RenderOgre::GetSceneManager()->setShadowCasterRenderBackFaces(true);

			Ogre::MaterialPtr houseMat = buildDepthShadowMaterial("fw12b.jpg");
			for (EntityList::iterator i = mHouseList.begin(); i != mHouseList.end(); ++i)
			{
				(*i)->setMaterial(houseMat);
			}
		}
		else
		{
			RenderOgre::GetSceneManager()->setShadowTextureCount(3);
			RenderOgre::GetSceneManager()->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_X8B8G8R8);
			RenderOgre::GetSceneManager()->setShadowTextureConfig(1, 1024, 1024, Ogre::PF_X8B8G8R8);
			RenderOgre::GetSceneManager()->setShadowTextureConfig(2, 1024, 1024, Ogre::PF_X8B8G8R8);
			RenderOgre::GetSceneManager()->setShadowTextureSelfShadow(false);
			RenderOgre::GetSceneManager()->setShadowCasterRenderBackFaces(false);
			RenderOgre::GetSceneManager()->setShadowTextureCasterMaterial(Ogre::BLANKSTRING);
		}

		matProfile->setReceiveDynamicShadowsDepth(depthShadows);
		matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mPSSMSetup.get()));

		//addTextureShadowDebugOverlay(TL_RIGHT, 3);
	}
	else
	{
		RenderOgre::GetSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
	}


}

void Dolphin::Terrain::setupContent()
{
	bool blankTerrain = false;
	//blankTerrain = true;

	if (!Ogre::ResourceGroupManager::getSingleton().resourceGroupExists("Terrain"))
		Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Terrain");

	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

	// Bugfix for D3D11 Render System because of pixel format incompatibility when using
	// vertex compression
	if (Ogre::Root::getSingleton().getRenderSystem()->getName() == "Direct3D11 Rendering Subsystem")
		mTerrainGlobals->setUseVertexCompressionWhenAvailable(false);

	//setupControls();

	//setDragLook(true);

	//mSceneMgr->setFog(FOG_LINEAR, ColourValue(0.7, 0.7, 0.8), 0, 10000, 25000);

	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);

	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(RenderOgre::GetSceneManager(), Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
	mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
	mTerrainGroup->setOrigin(mTerrainPos);
	mTerrainGroup->setResourceGroup("Terrain");
	Ogre::Light* l = RenderOgre::GetSceneManager()->getLight("sunlight");
	configureTerrainDefaults(l);
#ifdef PAGING
	// Paging setup
	mPageManager = OGRE_NEW PageManager();
	// Since we're not loading any pages from .page files, we need a way just
	// to say we've loaded them without them actually being loaded
	mPageManager->setPageProvider(&mDummyPageProvider);
	mPageManager->addCamera(mCamera);
	mTerrainPaging = OGRE_NEW TerrainPaging(mPageManager);
	PagedWorld* world = mPageManager->createWorld();
	mTerrainPaging->createWorldSection(world, mTerrainGroup, 2000, 3000,
		TERRAIN_PAGE_MIN_X, TERRAIN_PAGE_MIN_Y,
		TERRAIN_PAGE_MAX_X, TERRAIN_PAGE_MAX_Y);
#else
	for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
		for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
			defineTerrain(x, y, blankTerrain);
	// sync load since we want everything in place when we start
	mTerrainGroup->loadAllTerrains(true);
#endif
	do
	{
		if (mTerrainsImported)
		{
			saveTerrains(true);
			mTerrainsImported = false;
		}
	} while (mTerrainGroup->isDerivedDataUpdateInProgress());

	if (mTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
		while (ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}

	mTerrainGroup->freeTemporaryResources();
	/*
	// create a few entities on the terrain
	Ogre::Entity* e = RenderOgre::GetSceneManager()->createEntity("tudorhouse.mesh");
	Ogre::Vector3 entPos(mTerrainPos.x + 2043, 0, mTerrainPos.z + 1715);
	Ogre::Quaternion rot;
	entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
	rot.FromAngleAxis(Ogre::Degree(Ogre::Math::RangeRandom(-180, 180)), Ogre::Vector3::UNIT_Y);
	Ogre::SceneNode* sn = RenderOgre::GetSceneManager()->getRootSceneNode()->createChildSceneNode(entPos, rot);
	sn->setScale(Ogre::Vector3(0.12, 0.12, 0.12));
	sn->attachObject(e);
	mHouseList.push_back(e);

	e = RenderOgre::GetSceneManager()->createEntity("tudorhouse.mesh");
	entPos = Ogre::Vector3(mTerrainPos.x + 1850, 0, mTerrainPos.z + 1478);
	entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
	rot.FromAngleAxis(Ogre::Degree(Ogre::Math::RangeRandom(-180, 180)), Ogre::Vector3::UNIT_Y);
	sn = RenderOgre::GetSceneManager()->getRootSceneNode()->createChildSceneNode(entPos, rot);
	sn->setScale(Ogre::Vector3(0.12, 0.12, 0.12));
	sn->attachObject(e);
	mHouseList.push_back(e);

	e = RenderOgre::GetSceneManager()->createEntity("tudorhouse.mesh");
	entPos = Ogre::Vector3(mTerrainPos.x + 1970, 0, mTerrainPos.z + 2180);
	entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
	rot.FromAngleAxis(Ogre::Degree(Ogre::Math::RangeRandom(-180, 180)), Ogre::Vector3::UNIT_Y);
	sn = RenderOgre::GetSceneManager()->getRootSceneNode()->createChildSceneNode(entPos, rot);
	sn->setScale(Ogre::Vector3(0.12, 0.12, 0.12));
	sn->attachObject(e);
	mHouseList.push_back(e);*/
	changeShadows();
}


void Dolphin::Terrain::_shutdown()
{
	if (mTerrainPaging)
	{
		OGRE_DELETE mTerrainPaging;
		mTerrainPaging = 0;
		OGRE_DELETE mPageManager;
		mPageManager = 0;
	}
	else if (mTerrainGroup)
	{
		OGRE_DELETE mTerrainGroup;
		mTerrainGroup = 0;
	}

	if (mTerrainGlobals)
	{
		OGRE_DELETE mTerrainGlobals;
		mTerrainGlobals = 0;
	}

	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("Terrain");

	mHouseList.clear();
}