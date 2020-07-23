#pragma once

#include "tiny_gltf.h"
#include <unordered_map>
#include <OgreTexture.h>

namespace Ogre_glTF
{

	///Import textures described in glTF into Ogre
	class textureImporter
	{
		///List of the loaded basic textures
		std::unordered_map<int, Ogre::TexturePtr> loadedTextures;

		///Static counter to make unique texture name. Incremented by constructor
		static size_t id;

		///Reference to the tinygltf
		tinygltf::Model& model;

		///Load a single texture
		/// \param texture reference to the texture that we are loading
		void loadTexture(const tinygltf::Texture& texture);

		///Checks that is hardware gamma enabled
		bool isHardwareGammaEnabled() const;

	public:
		///Construct the texture importer object. Inrement the id counter
		/// \param input reference to the model that we are loading
		textureImporter(tinygltf::Model& input);

		///Load all the textures in the model
		void loadTextures();

		///Get the loaded texture that corespound to the given index
		/// \param glTFTextureSourceID index of a texture in the gltf file
		Ogre::TexturePtr getTexture(int glTFTextureSourceID);

		///Get the texture that corespound to the given index, but as a greyscale one containing only
		///the information of the given channel. It seems that the order of channel on loaded textures
		///is BGR
		/// \param gltfTextureSourceID index of a texture in the gltf file
		/// \param channel index of a channel. Starts from zero
		Ogre::TexturePtr generateGreyScaleFromChannel(int gltfTextureSourceID, int channel);

		///Get the normal texture in a compatible format
		/// \param gltfTextureSourceID index of a texture in the gltf file
		Ogre::TexturePtr getNormalSNORM(int gltfTextureSourceID);
	};
}