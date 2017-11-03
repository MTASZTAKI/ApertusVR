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

#include "ApeUnitTextureImpl.h"

Ape::UnitTextureImpl::UnitTextureImpl(std::string name, bool isHostCreated) : Ape::IUnitTexture(name), Ape::Replica("UnitTexture", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mScroll = Ape::Vector2();
	mAddressingMode = Ape::Texture::AddressingMode::AM_NONE;
	mFiltering = Ape::IUnitTexture::Filtering();
	mParameters = Ape::IUnitTexture::Parameters();
}

Ape::UnitTextureImpl::~UnitTextureImpl()
{
	
}

void Ape::UnitTextureImpl::setParameters(Ape::MaterialWeakPtr material, std::string fileName)
{
	mParameters.material = material;
	mParameters.fileName = fileName;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_PARAMETERS));
}

Ape::IUnitTexture::Parameters Ape::UnitTextureImpl::getParameters()
{
	return mParameters;
}

void Ape::UnitTextureImpl::setTextureScroll(float u, float v)
{
	mScroll.x = u;
	mScroll.y = v;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_SCROLL));
}

Ape::Vector2 Ape::UnitTextureImpl::getTextureScroll()
{
	return mScroll;
}

void Ape::UnitTextureImpl::setTextureAddressingMode(Ape::Texture::AddressingMode addressingMode)
{
	mAddressingMode = addressingMode;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_ADDRESSING));
}

Ape::Texture::AddressingMode Ape::UnitTextureImpl::getTextureAddressingMode()
{
	return mAddressingMode;
}

void Ape::UnitTextureImpl::setTextureFiltering(Ape::Texture::Filtering minFilter, Ape::Texture::Filtering magFilter, Ape::Texture::Filtering mipFilter)
{
	mFiltering.minFilter = minFilter;
	mFiltering.magFilter = magFilter;
	mFiltering.mipFilter = mipFilter;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_FILTERING));
}

Ape::IUnitTexture::Filtering Ape::UnitTextureImpl::getTextureFiltering()
{
	return mFiltering;
}

void Ape::UnitTextureImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::UnitTextureImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mScroll);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAddressingMode);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mFiltering);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::UnitTextureImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_PARAMETERS));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mScroll))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_SCROLL));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAddressingMode))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_ADDRESSING));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mFiltering))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_UNIT_FILTERING));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


