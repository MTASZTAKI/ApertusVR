#include "TerrainMaterial.h"

#include "Ogre.h"
#include <OgreTerrain.h>


    TerrainMaterial::TerrainMaterial(Ogre::String materialname)
    {
		mMaterialName = materialname;
        mProfiles.push_back(OGRE_NEW Profile(this, "OgreMaterial", "Profile for rendering Ogre standard material"));
        setActiveProfile("OgreMaterial");
    }
    
    void TerrainMaterial::setMaterialByName(const Ogre::String materialname) {
		mMaterialName = materialname;
    };

    // -----------------------------------------------------------------------------------------------------------------------

    TerrainMaterial::Profile::Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc)
        : Ogre::TerrainMaterialGenerator::Profile(parent, name, desc)        
    {
    };

    TerrainMaterial::Profile::~Profile()
    {
    };

	void TerrainMaterial::Profile::setLightmapEnabled(bool enabled)
	{

	}

    Ogre::MaterialPtr TerrainMaterial::Profile::generate(const Ogre::Terrain* terrain)
    {
        const Ogre::String& matName = terrain->getMaterialName();        
        
        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
        if (!mat.isNull())
            Ogre::MaterialManager::getSingleton().remove(matName);
         
  		// Set Ogre material 
        mat = Ogre::MaterialManager::getSingleton().getByName(((TerrainMaterial*)getParent())->mMaterialName);

		// Get default pass
		Ogre::Pass *p = mat->getTechnique(0)->getPass(0);	

		// Add terrain's global normalmap to renderpass so the fragment program can find it.
		Ogre::TextureUnitState *tu = p->createTextureUnitState(matName+"/nm");
	
		Ogre::TexturePtr nmtx = terrain->getTerrainNormalMap();
        tu->_setTexturePtr(nmtx);
      	
		return mat;
    };
    
    Ogre::MaterialPtr TerrainMaterial::Profile::generateForCompositeMap(const Ogre::Terrain* terrain)
    {
		return terrain->_getCompositeMapMaterial();
    };

    Ogre::uint8 TerrainMaterial::Profile::getMaxLayers(const Ogre::Terrain* terrain) const
    {
        return 0;
    };

    void TerrainMaterial::Profile::updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
    {
    };

    void TerrainMaterial::Profile::updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
    {
    };

    void TerrainMaterial::Profile::requestOptions(Ogre::Terrain* terrain)
    {
        terrain->_setMorphRequired(true);
        terrain->_setNormalMapRequired(true); // enable global normal map
        terrain->_setLightMapRequired(false);
        terrain->_setCompositeMapRequired(false);
    };

