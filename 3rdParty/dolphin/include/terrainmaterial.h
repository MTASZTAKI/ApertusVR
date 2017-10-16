/*This class from ogre forums : http://www.ogre3d.org/forums/viewtopic.php?f=5&t=72455
  Made by Nauk.
  I add one row (setLightmapEnabled) because original not up to date for ogre 1.10 :)
*/

#ifndef TERRAINMATERIAL_H
#define TERRAINMATERIAL_H

#include "OgreTerrainMaterialGenerator.h"


    class TerrainMaterial : public Ogre::TerrainMaterialGenerator
    {
    public:
		
		TerrainMaterial(Ogre::String materialName);
		
		void setMaterialByName(const Ogre::String materialname); 
        
        class Profile : public Ogre::TerrainMaterialGenerator::Profile
        {
        public:
            Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc);
            ~Profile();

            bool isVertexCompressionSupported() const { return false; }

            Ogre::MaterialPtr generate(const Ogre::Terrain* terrain);

            Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain* terrain);

			void setLightmapEnabled(bool enabled);

            Ogre::uint8 getMaxLayers(const Ogre::Terrain* terrain) const;

            void updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);

            void updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);

            void requestOptions(Ogre::Terrain* terrain);       

        };
    protected:			
		Ogre::String mMaterialName;        
    };
    
#endif
