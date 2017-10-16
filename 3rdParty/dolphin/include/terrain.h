/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef TERRAIN_H
#define TERRAIN_H

#include "physics.h"

#include "ogre.h"
#include "OgrePageManager.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"

namespace Dolphin
{
	//#define PAGING

	#define TERRAIN_PAGE_MIN_X 0
	#define TERRAIN_PAGE_MIN_Y 0
	#define TERRAIN_PAGE_MAX_X 0
	#define TERRAIN_PAGE_MAX_Y 0

	#define TERRAIN_FILE_PREFIX Ogre::String("testTerrain")
	#define TERRAIN_FILE_SUFFIX Ogre::String("dat")
	#define TERRAIN_WORLD_SIZE 1200.0f
	#define TERRAIN_SIZE 513

	class Terrain
	{
	public:
		Terrain();
		virtual ~Terrain();
		
		void setupContent();
		void Update(const Ogre::FrameEvent& evt);
		void SetPhysics(Physics* _physics);
	private:
		Ogre::TerrainGlobalOptions* mTerrainGlobals;
		Ogre::TerrainGroup* mTerrainGroup;
		bool mPaging;
		Ogre::TerrainPaging* mTerrainPaging;
		Ogre::PageManager* mPageManager;
		bool mFly;
		Ogre::Real mFallVelocity;
		enum Mode
		{
			MODE_NORMAL = 0,
			MODE_EDIT_HEIGHT = 1,
			MODE_EDIT_BLEND = 2,
			MODE_COUNT = 3
		};
		enum ShadowMode
		{
			SHADOWS_NONE = 0,
			SHADOWS_COLOUR = 1,
			SHADOWS_DEPTH = 2,
			SHADOWS_COUNT = 3
		};
		Mode mMode;
		ShadowMode mShadowMode;
		Ogre::uint8 mLayerEdit;
		Ogre::Real mBrushSizeTerrainSpace;
		Ogre::SceneNode* mEditNode;
		Ogre::Entity* mEditMarker;
		Ogre::Real mHeightUpdateCountDown;
		Ogre::Real mHeightUpdateRate;
		Ogre::Vector3 mTerrainPos;
		bool mTerrainsImported;
		Ogre::ShadowCameraSetupPtr mPSSMSetup;

		typedef std::list<Ogre::Entity*> EntityList;
		EntityList mHouseList;

		/// This class just pretends to provide prcedural page content to avoid page loading
		class DummyPageProvider : public Ogre::PageProvider
		{
		public:
			bool prepareProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
			bool loadProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
			bool unloadProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
			bool unprepareProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
		};
		DummyPageProvider mDummyPageProvider;
		void defineTerrain(long x, long y, bool flat = false);
		void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);
		void initBlendMaps(Ogre::Terrain* terrain);
		void configureTerrainDefaults(Ogre::Light* l);
		Ogre::MaterialPtr buildDepthShadowMaterial(const Ogre::String& textureName);
		void changeShadows();
		void configureShadows(bool enabled, bool depthShadows);
		void saveTerrains(bool onlyIfModified);
		void setupView();
		
		void _shutdown();

		Physics* physics;


	};
}
#endif //TERRAIN_H
