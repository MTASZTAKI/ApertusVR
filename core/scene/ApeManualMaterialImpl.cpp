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

#include "ApeManualMaterialImpl.h"

Ape::ManualMaterialImpl::ManualMaterialImpl(std::string name, bool isHostCreated) : Ape::IManualMaterial(name), Ape::Replica("ManualMaterial", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mTexture = Ape::TextureWeakPtr();
	mTextureName = std::string();
	mCullingMode = Ape::Material::CullingMode::CLOCKWISE;
}

Ape::ManualMaterialImpl::~ManualMaterialImpl()
{
	
}

void Ape::ManualMaterialImpl::setDiffuseColor(Ape::Color diffuse)
{
	mDiffuseColor = diffuse;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_DIFFUSE));
}

void Ape::ManualMaterialImpl::setSpecularColor(Ape::Color specular)
{
	mSpecularColor = specular;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_SPECULAR));
}

void Ape::ManualMaterialImpl::setAmbientColor(Ape::Color ambient)
{
	mAmbientColor = ambient;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_AMBIENT));
}

void Ape::ManualMaterialImpl::setEmissiveColor(Ape::Color emissive)
{
	mEmissiveColor = emissive;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_EMISSIVE));
}

void Ape::ManualMaterialImpl::setPassTexture(Ape::TextureWeakPtr texture)
{
	if (auto textureSP = texture.lock())
	{
		mTexture = texture;
		mTextureName = textureSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_TEXTURE));
	}
	else
		mTexture = Ape::TextureWeakPtr();
}

Ape::TextureWeakPtr Ape::ManualMaterialImpl::getPassTexture()
{
	return mTexture;
}

void Ape::ManualMaterialImpl::setPass(Ape::PassWeakPtr pass)
{
	if (auto passSP = pass.lock())
	{
		mPass = pass;
		mPassName = passSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_PASS));
	}
	else
		mPass = Ape::PassWeakPtr();
}

void Ape::ManualMaterialImpl::setCullingMode(Ape::Material::CullingMode cullingMode)
{
	mCullingMode = cullingMode;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE));
}

void Ape::ManualMaterialImpl::setSceneBlending(Ape::Pass::SceneBlendingType sceneBlendingType)
{
	mSceneBlendingType = sceneBlendingType;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING));
}

void Ape::ManualMaterialImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::ManualMaterialImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDiffuseColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSpecularColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAmbientColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mEmissiveColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mPassName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mTextureName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mCullingMode);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSceneBlendingType);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::ManualMaterialImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_DIFFUSE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_SPECULAR));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAmbientColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_AMBIENT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mEmissiveColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_EMISSIVE));
	RakNet::RakString passName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, passName))
	{
		mPassName = passName.C_String();
		mPass = std::static_pointer_cast<Ape::Pass>(mpScene->getEntity(mPassName).lock());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_PASS));
	}
	RakNet::RakString textureName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, textureName))
	{
		mTextureName = textureName.C_String();
		mTexture = std::static_pointer_cast<Ape::Texture>(mpScene->getEntity(mTextureName).lock());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_TEXTURE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mCullingMode))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSceneBlendingType))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


