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
#include "apeManualPassImpl.h"

ape::ManualPassImpl::ManualPassImpl(std::string name, bool isHostCreated) : ape::IManualPass(name), ape::Replica("ManualPass", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mAmbientColor = ape::Color();
	mDiffuseColor = ape::Color();
	mSpecularColor = ape::Color();
	mEmissiveColor = ape::Color();
	mShininess = 0.0f;
	mSceneBlendingType = ape::Pass::SceneBlendingType::INVALID;
	mTextureName = std::string();
	mTexture = ape::TextureWeakPtr();
}

ape::ManualPassImpl::~ManualPassImpl()
{
	
}

void ape::ManualPassImpl::setDiffuseColor(ape::Color diffuse)
{
	mDiffuseColor = diffuse;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_DIFFUSE));
}

void ape::ManualPassImpl::setSpecularColor(ape::Color specular)
{
	mSpecularColor = specular;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_SPECULAR));
}

void ape::ManualPassImpl::setAmbientColor(ape::Color ambient)
{
	mAmbientColor = ambient;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_AMBIENT));
}

void ape::ManualPassImpl::setEmissiveColor(ape::Color emissive)
{
	mEmissiveColor = emissive;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_EMISSIVE));
}

void ape::ManualPassImpl::setShininess(float shininess)
{
	mShininess = shininess;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_SHININESS));
}

void ape::ManualPassImpl::setTexture(ape::TextureWeakPtr texture)
{
	if (auto textureSP = texture.lock())
	{
		mTexture = texture;
		mTextureName = textureSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_TEXTURE));
	}
	else
		mTexture = ape::TextureWeakPtr();
}

void ape::ManualPassImpl::setPassGpuParameters(ape::PassGpuParameters passGpuParameters)
{
	mPassGpuParameters = passGpuParameters;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_GPUPARAMETERS));
}

void ape::ManualPassImpl::setSceneBlending(ape::Pass::SceneBlendingType sceneBlendingType)
{
	mSceneBlendingType = sceneBlendingType;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_SCENEBLENDING));
}

void ape::ManualPassImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::ManualPassImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAmbientColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDiffuseColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSpecularColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mEmissiveColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mShininess);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSceneBlendingType);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mTextureName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::ManualPassImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAmbientColor))
	{
		//APE_LOG_DEBUG("Deserialize mAmbientColor " << mAmbientColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_AMBIENT));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
	{
		//APE_LOG_DEBUG("Deserialize mDiffuseColor " <<  mDiffuseColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_DIFFUSE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
	{
		//APE_LOG_DEBUG("Deserialize mSpecularColor " << mSpecularColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_SPECULAR));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mEmissiveColor))
	{
		//APE_LOG_DEBUG("Deserialize mEmissiveColor " << mEmissiveColor.toString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_EMISSIVE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mShininess))
	{
		//APE_LOG_DEBUG("Deserialize mShininess " << mShininess);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_SHININESS));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSceneBlendingType))
	{
		//APE_LOG_DEBUG("Deserialize mSceneBlendingType " << mSceneBlendingType);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_SCENEBLENDING));
	}
	RakNet::RakString textureName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, textureName))
	{
		mTextureName = textureName.C_String();
		//APE_LOG_DEBUG("Deserialize mTextureName " << mTextureName);
		mTexture = std::static_pointer_cast<ape::Texture>(mpSceneManager->getEntity(mTextureName).lock());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_MANUAL_TEXTURE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
