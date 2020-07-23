#include "Ogre_glTF_materialLoader.hpp"
#include "Ogre_glTF_textureImporter.hpp"
#include "Ogre_glTF_common.hpp"
#include <OgreHlmsPbsDatablock.h>
#include <OgreHlms.h>
#include <OgreHlmsManager.h>
#include <OgreLogManager.h>
#include "Ogre_glTF_internal_utils.hpp"

using namespace Ogre_glTF;

Ogre::Vector3 materialLoader::convertColor(const tinygltf::ColorValue& color)
{
	std::array<Ogre::Real, 4> colorBuffer{};
	internal_utils::container_double_to_real(color, colorBuffer);
	return Ogre::Vector3 { colorBuffer.data() };
}

void materialLoader::setBaseColor(Ogre::HlmsPbsDatablock* block, Ogre::Vector3 color) const
{
	block->setDiffuse(color);
}

void materialLoader::setMetallicValue(Ogre::HlmsPbsDatablock* block, Ogre::Real value) const
{
	block->setMetalness(value);
}

void materialLoader::setRoughnesValue(Ogre::HlmsPbsDatablock* block, Ogre::Real value) const
{
	block->setRoughness(value);
}

void materialLoader::setEmissiveColor(Ogre::HlmsPbsDatablock* block, Ogre::Vector3 color) const
{
	block->setEmissive(color);
}

bool materialLoader::isTextureIndexValid(int value) const
{
	return !(value < 0);
}

void materialLoader::setBaseColorTexture(Ogre::HlmsPbsDatablock* block, int value) const
{
	if(!isTextureIndexValid(value)) return;
	auto texture = textureImporterRef.getTexture(value);
	if(texture)
	{
		//OgreLog("diffuse texture from textureImporter : " + texture->getName());
		block->setTexture(Ogre::PbsTextureTypes::PBSM_DIFFUSE, 0, texture);
	}
}

void materialLoader::setMetalRoughTexture(Ogre::HlmsPbsDatablock* block, int gltfTextureID) const
{
	if(!isTextureIndexValid(gltfTextureID)) return;
	//Ogre cannot use combined metal rough textures. Metal is in the R channel, and rough in the G channel. It seems that the images are loaded as BGR by the libarry
	//R channel is channle 2 (from 0), G channel is 1.

	auto metalTexure = textureImporterRef.generateGreyScaleFromChannel(gltfTextureID, 2);
	auto roughTexure = textureImporterRef.generateGreyScaleFromChannel(gltfTextureID, 1);

	if(metalTexure)
	{
		//OgreLog("metalness greyscale texture extracted by textureImporter : " + metalTexure->getName());
		block->setTexture(Ogre::PBSM_METALLIC, 0, metalTexure);
	}

	if(roughTexure)
	{
		//OgreLog("roughness geyscale texture extracted by textureImporter : " + roughTexure->getName());
		block->setTexture(Ogre::PBSM_ROUGHNESS, 0, roughTexure);
	}
}

void materialLoader::setNormalTexture(Ogre::HlmsPbsDatablock* block, int value) const
{
	if(!isTextureIndexValid(value)) return;
	auto texture = textureImporterRef.getNormalSNORM(value);
	if(texture)
	{
		//OgreLog("normal texture from textureImporter : " + texture->getName());
		block->setTexture(Ogre::PbsTextureTypes::PBSM_NORMAL, 0, texture);
	}
}

void materialLoader::setOcclusionTexture(Ogre::HlmsPbsDatablock* block, int value) const
{
	if(!isTextureIndexValid(value)) return;
	auto texture = textureImporterRef.getTexture(value);
	if(texture)
	{
		//OgreLog("occlusion texture from textureImporter : " + texture->getName());
		//OgreLog("Warning: Ogre doesn't supoort occlusion map in it's HLMS PBS implementation!");
		//block->setTexture(Ogre::PbsTextureTypes::PBSM_, 0, texture);
	}
}

void materialLoader::setEmissiveTexture(Ogre::HlmsPbsDatablock* block, int value) const
{
	if(!isTextureIndexValid(value)) return;
	auto texture = textureImporterRef.getTexture(value);
	if(texture)
	{
		//OgreLog("emissive texture from textureImporter : " + texture->getName());
		block->setTexture(Ogre::PbsTextureTypes::PBSM_EMISSIVE, 0, texture);
	}
}

void materialLoader::setAlphaMode(Ogre::HlmsPbsDatablock* block, const std::string& mode) const
{
	if(mode == "BLEND")
	{
		auto blendBlock = *block->getBlendblock();
		blendBlock.setBlendType(Ogre::SBT_TRANSPARENT_ALPHA);
		block->setBlendblock(blendBlock);
	}
	else if(mode =="MASK")
	{
		block->setAlphaTest( Ogre::CMPF_GREATER_EQUAL );
	}
}

void materialLoader::setAlphaCutoff(Ogre::HlmsPbsDatablock* block, Ogre::Real value) const
{
	block->setAlphaTestThreshold(value);
}

materialLoader::materialLoader(tinygltf::Model& input, textureImporter& textureInterface) :
 textureImporterRef { textureInterface },
 model { input }
{
}

Ogre::HlmsDatablock* materialLoader::getDatablock(size_t index) const
{
	OgreLog("Loading material...");
	auto HlmsPbs			 = static_cast<Ogre::HlmsPbs*>(Ogre::Root::getSingleton().getHlmsManager()->getHlms(Ogre::HlmsTypes::HLMS_PBS));
	const auto material		 = model.materials[index];

	auto datablock = static_cast<Ogre::HlmsPbsDatablock*>(HlmsPbs->getDatablock(Ogre::IdString(material.name)));
	if(datablock)
	{
		//OgreLog("Found HlmsPbsDatablock " + material.name + " in Ogre::HlmsPbs");
		return datablock;
	}
	datablock = static_cast<Ogre::HlmsPbsDatablock*>(HlmsPbs->createDatablock(Ogre::IdString(material.name),
																			  material.name,
																			  Ogre::HlmsMacroblock {},
																			  Ogre::HlmsBlendblock {},
																			  Ogre::HlmsParamVec {}));
	datablock->setWorkflow(Ogre::HlmsPbsDatablock::Workflows::MetallicWorkflow);

	//TODO refactor these almost exact peices of code
	//OgreLog("values");
	for(const auto& content : material.values)
	{
		//OgreLog(content.first);
		if(content.first == "baseColorTexture")
			setBaseColorTexture(datablock, content.second.TextureIndex());

		if(content.first == "metallicRoughnessTexture")
			setMetalRoughTexture(datablock, content.second.TextureIndex());

		if (content.first == "baseColorFactor")
		{
			setBaseColor(datablock, convertColor(content.second.ColorFactor()));

			// Need to set the alpha channel separately
			float alpha = content.second.number_array[3];
			auto transparentMode = (alpha == 1) ? Ogre::HlmsPbsDatablock::None : Ogre::HlmsPbsDatablock::Transparent;
			datablock->setTransparency(alpha, transparentMode);
		}

		if(content.first == "metallicFactor")
			setMetallicValue(datablock, static_cast<float>(content.second.Factor()));

		if(content.first == "roughnessFactor")
			setRoughnesValue(datablock, static_cast<float>(content.second.Factor()));
	}

	//OgreLog("additionalValues");
	for(const auto& content : material.additionalValues)
	{
		//OgreLog(content.first);
		if(content.first == "normalTexture")
			setNormalTexture(datablock, content.second.TextureIndex());

		//if (content.first == "occlusionTexture")
		//	setOcclusionTexture(datablock, getTextureIndex(content));

		if(content.first == "emissiveTexture")
			setEmissiveTexture(datablock, content.second.TextureIndex());

		if(content.first == "emissiveFactor")
			setEmissiveColor(datablock, convertColor(content.second.ColorFactor()));

		if(content.first == "alphaMode") 
			setAlphaMode(datablock, content.second.string_value);

		if(content.first == "alphaCutoff") 
			setAlphaCutoff(datablock, static_cast<Ogre::Real>(content.second.number_value));
	}

	//	OgreLog("extCommonValues");
	//	for(const auto& content : material.extCommonValues)
	//		OgreLog(content.first);

	//	OgreLog("extPBRValues");
	//	for(const auto& content : material.extPBRValues)
	//		OgreLog(content.first);

	return datablock;
}

size_t materialLoader::getDatablockCount() const //todo this could use some refactoring. This information is actually fetched like, twice.
{
	const auto mainMeshIndex = (model.defaultScene != 0 ? model.nodes[model.scenes[model.defaultScene].nodes.front()].mesh : 0);
	const auto& mesh = model.meshes[mainMeshIndex];
	return mesh.primitives.size();
}
