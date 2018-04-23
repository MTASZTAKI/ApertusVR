/*MIT License

Copyright (c) 2016 MTA SZTAKI

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
#include "ApeManualPassImpl.h"

Ape::ManualPassImpl::ManualPassImpl(std::string name, bool isHostCreated) : Ape::IManualPass(name), Ape::Replica("ManualPass", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mAmbientColor = Ape::Color();
	mDiffuseColor = Ape::Color();
	mSpecularColor = Ape::Color();
	mEmissiveColor = Ape::Color();
	mShininess = 0.0f;
	mSceneBlendingType = Ape::Pass::SceneBlendingType::INVALID;
	mTextureName = std::string();
	mTexture = Ape::TextureWeakPtr();
}

Ape::ManualPassImpl::~ManualPassImpl()
{
	
}

void Ape::ManualPassImpl::setDiffuseColor(Ape::Color diffuse)
{
	mDiffuseColor = diffuse;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_DIFFUSE));
}

void Ape::ManualPassImpl::setSpecularColor(Ape::Color specular)
{
	mSpecularColor = specular;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_SPECULAR));
}

void Ape::ManualPassImpl::setAmbientColor(Ape::Color ambient)
{
	mAmbientColor = ambient;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_AMBIENT));
}

void Ape::ManualPassImpl::setEmissiveColor(Ape::Color emissive)
{
	mEmissiveColor = emissive;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_EMISSIVE));
}

void Ape::ManualPassImpl::setShininess(float shininess)
{
	mShininess = shininess;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_SHININESS));
}

void Ape::ManualPassImpl::setTexture(Ape::TextureWeakPtr texture)
{
	if (auto textureSP = texture.lock())
	{
		mTexture = texture;
		mTextureName = textureSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_TEXTURE));
	}
	else
		mTexture = Ape::TextureWeakPtr();
}

void Ape::ManualPassImpl::setPassGpuParameters(Ape::PassGpuParameters passGpuParameters)
{
	mPassGpuParameters = passGpuParameters;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_GPUPARAMETERS));
}

void Ape::ManualPassImpl::setSceneBlending(Ape::Pass::SceneBlendingType sceneBlendingType)
{
	mSceneBlendingType = sceneBlendingType;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_SCENEBLENDING));
}

void Ape::ManualPassImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::ManualPassImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
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
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::ManualPassImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAmbientColor))
	{
		//LOG(LOG_TYPE_DEBUG, "Deserialize mAmbientColor " << mAmbientColor.toString());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_AMBIENT));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
	{
		//LOG(LOG_TYPE_DEBUG, "Deserialize mDiffuseColor " <<  mDiffuseColor.toString());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_DIFFUSE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
	{
		//LOG(LOG_TYPE_DEBUG, "Deserialize mSpecularColor " << mSpecularColor.toString());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_SPECULAR));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mEmissiveColor))
	{
		//LOG(LOG_TYPE_DEBUG, "Deserialize mEmissiveColor " << mEmissiveColor.toString());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_EMISSIVE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mShininess))
	{
		//LOG(LOG_TYPE_DEBUG, "Deserialize mShininess " << mShininess);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_SHININESS));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSceneBlendingType))
	{
		//LOG(LOG_TYPE_DEBUG, "Deserialize mSceneBlendingType " << mSceneBlendingType);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_SCENEBLENDING));
	}
	RakNet::RakString textureName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, textureName))
	{
		mTextureName = textureName.C_String();
		//LOG(LOG_TYPE_DEBUG, "Deserialize mTextureName " << mTextureName);
		mTexture = std::static_pointer_cast<Ape::Texture>(mpScene->getEntity(mTextureName).lock());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_MANUAL_TEXTURE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
