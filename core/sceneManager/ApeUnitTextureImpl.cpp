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

#include "ApeUnitTextureImpl.h"

ape::UnitTextureImpl::UnitTextureImpl(std::string name, bool isHostCreated) : ape::IUnitTexture(name), ape::Replica("UnitTexture", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mScroll = ape::Vector2();
	mAddressingMode = ape::Texture::AddressingMode::AM_NONE;
	mFiltering = ape::IUnitTexture::Filtering();
	mParameters = ape::IUnitTexture::Parameters();
}

ape::UnitTextureImpl::~UnitTextureImpl()
{
	
}

void ape::UnitTextureImpl::setParameters(ape::MaterialWeakPtr material, std::string fileName)
{
	mParameters.material = material;
	mParameters.fileName = fileName;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_PARAMETERS));
}

ape::IUnitTexture::Parameters ape::UnitTextureImpl::getParameters()
{
	return mParameters;
}

void ape::UnitTextureImpl::setTextureScroll(float u, float v)
{
	mScroll.x = u;
	mScroll.y = v;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_SCROLL));
}

ape::Vector2 ape::UnitTextureImpl::getTextureScroll()
{
	return mScroll;
}

void ape::UnitTextureImpl::setTextureAddressingMode(ape::Texture::AddressingMode addressingMode)
{
	mAddressingMode = addressingMode;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_ADDRESSING));
}

ape::Texture::AddressingMode ape::UnitTextureImpl::getTextureAddressingMode()
{
	return mAddressingMode;
}

void ape::UnitTextureImpl::setTextureFiltering(ape::Texture::Filtering minFilter, ape::Texture::Filtering magFilter, ape::Texture::Filtering mipFilter)
{
	mFiltering.minFilter = minFilter;
	mFiltering.magFilter = magFilter;
	mFiltering.mipFilter = mipFilter;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_FILTERING));
}

ape::IUnitTexture::Filtering ape::UnitTextureImpl::getTextureFiltering()
{
	return mFiltering;
}

void ape::UnitTextureImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::UnitTextureImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mScroll);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAddressingMode);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mFiltering);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::UnitTextureImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_PARAMETERS));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mScroll))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_SCROLL));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAddressingMode))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_ADDRESSING));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mFiltering))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_UNIT_FILTERING));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


