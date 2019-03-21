/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <iostream>
#include "ApeManualMaterialImpl.h"

ape::ManualMaterialImpl::ManualMaterialImpl(std::string name, bool isHostCreated) : ape::IManualMaterial(name), ape::Replica("ManualMaterial", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mAmbientColor = ape::Color();
	mDiffuseColor = ape::Color();
	mSpecularColor = ape::Color();
	mEmissiveColor = ape::Color();
	mTexture = ape::TextureWeakPtr();
	mTextureName = std::string();
	mPass = ape::PassWeakPtr();
	mPassName = std::string();
	mCullingMode = ape::Material::CullingMode::INVALID_CM;
	mManualCullingMode = ape::Material::ManualCullingMode::INVALID_MCM;
	mDepthCheckEnabled = false;
	mDepthWriteEnabled = false;
	mDepthBias = ape::Vector2();
	mLightingEnabled = true;
	mZOrder = 0;
	mIsShowOnOverlay = false;
}

ape::ManualMaterialImpl::~ManualMaterialImpl()
{
	
}

void ape::ManualMaterialImpl::setDiffuseColor(ape::Color diffuse)
{
	mDiffuseColor = diffuse;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DIFFUSE));
}

void ape::ManualMaterialImpl::setSpecularColor(ape::Color specular)
{
	mSpecularColor = specular;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_SPECULAR));
}

void ape::ManualMaterialImpl::setAmbientColor(ape::Color ambient)
{
	mAmbientColor = ambient;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_AMBIENT));
}

void ape::ManualMaterialImpl::setEmissiveColor(ape::Color emissive)
{
	mEmissiveColor = emissive;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_EMISSIVE));
}

void ape::ManualMaterialImpl::setPassTexture(ape::TextureWeakPtr texture)
{
	if (auto textureSP = texture.lock())
	{
		mTexture = texture;
		mTextureName = textureSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_TEXTURE));
	}
	else
		mTexture = ape::TextureWeakPtr();
}

ape::TextureWeakPtr ape::ManualMaterialImpl::getPassTexture()
{
	return mTexture;
}

void ape::ManualMaterialImpl::setPass(ape::PassWeakPtr pass)
{
	if (auto passSP = pass.lock())
	{
		mPass = pass;
		mPassName = passSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_PASS));
	}
	else
		mPass = ape::PassWeakPtr();
}

void ape::ManualMaterialImpl::setCullingMode(ape::Material::CullingMode cullingMode)
{
	mCullingMode = cullingMode;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE));
}

void ape::ManualMaterialImpl::setSceneBlending(ape::Pass::SceneBlendingType sceneBlendingType)
{
	mSceneBlendingType = sceneBlendingType;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING));
}

void ape::ManualMaterialImpl::setDepthWriteEnabled(bool enable)
{
	mDepthWriteEnabled = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DEPTHWRITE));
}

void ape::ManualMaterialImpl::setDepthCheckEnabled(bool enable)
{
	mDepthCheckEnabled = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DEPTHCHECK));
}

void ape::ManualMaterialImpl::setLightingEnabled(bool enable)
{
	mLightingEnabled = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_LIGHTING));
}

void ape::ManualMaterialImpl::setManualCullingMode(ape::Material::ManualCullingMode manualcullingMode)
{
	mManualCullingMode = manualcullingMode;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_MANUALCULLINGMODE));
}

void ape::ManualMaterialImpl::setDepthBias(float constantBias, float slopeScaleBias)
{
	mDepthBias.x = constantBias;
	mDepthBias.y = slopeScaleBias;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DEPTHBIAS));
}

void ape::ManualMaterialImpl::showOnOverlay(bool enable, int zOrder)
{
	if (enable)
	{
		mZOrder = zOrder;
		mIsShowOnOverlay = true;
	}
	else
		mIsShowOnOverlay = false;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_OVERLAY));
}

int ape::ManualMaterialImpl::getZOrder()
{
	return mZOrder;
}

void ape::ManualMaterialImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::ManualMaterialImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDiffuseColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSpecularColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAmbientColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mEmissiveColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mPassName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mCullingMode);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mManualCullingMode);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDepthCheckEnabled);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDepthWriteEnabled);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDepthBias);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLightingEnabled);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mTextureName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::ManualMaterialImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString passName;
	RakNet::RakString textureName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
	{
		//APE_LOG_DEBUG("Deserialize mDiffuseColor " << mDiffuseColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DIFFUSE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
	{
		//APE_LOG_DEBUG("Deserialize mSpecularColor " << mSpecularColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_SPECULAR));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAmbientColor))
	{
		//APE_LOG_DEBUG("Deserialize mAmbientColor " << mAmbientColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_AMBIENT));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mEmissiveColor))
	{
		//APE_LOG_DEBUG("Deserialize mEmissiveColor " << mEmissiveColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_EMISSIVE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, passName))
	{
		if (auto entity = mpSceneManager->getEntity(passName.C_String()).lock())
		{
			if (entity->getName() == mName && entity->getType() == mType)
			{
				APE_LOG_DEBUG("Deserialize passName " << passName.C_String());
				mPass = std::static_pointer_cast<ape::Pass>(entity);
				mPassName = passName.C_String();
				mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_PASS));
			}
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mCullingMode))
	{
		//APE_LOG_DEBUG("Deserialize mCullingMode: " << mCullingMode);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSceneBlendingType))
	{
		//APE_LOG_DEBUG("Deserialize mSceneBlendingType: " << mSceneBlendingType);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mManualCullingMode))
	{
		//APE_LOG_DEBUG("Deserialize mManualCullingMode: " << mManualCullingMode);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_MANUALCULLINGMODE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDepthCheckEnabled))
	{
		//APE_LOG_DEBUG("Deserialize mDepthCheckEnabled: " << mDepthCheckEnabled);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DEPTHCHECK));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDepthWriteEnabled))
	{
		//APE_LOG_DEBUG("Deserialize mDepthWriteEnabled: " << mDepthWriteEnabled);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DEPTHWRITE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDepthBias))
	{
		//APE_LOG_DEBUG("Deserialize mDepthBias: " << mDepthBias);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_DEPTHBIAS));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, textureName))
	{
		mTextureName = textureName.C_String();
		//APE_LOG_DEBUG("Deserialize textureName: " << mTextureName);
		mTexture = std::static_pointer_cast<ape::Texture>(mpSceneManager->getEntity(mTextureName).lock());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_MANUAL_TEXTURE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
