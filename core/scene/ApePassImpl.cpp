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

#include "ApePassImpl.h"

Ape::PassImpl::PassImpl(std::string name, bool isHostCreated) : Ape::IPass(name), Ape::Replica("Pass", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mAlbedo = Ape::Color();
	mRoughness = 0.0f;
	mLightRoughnessOffset = 0.0f;
	mF0 = Ape::Color();
}

Ape::PassImpl::~PassImpl()
{
	
}

void Ape::PassImpl::setAlbedo(Ape::Color albedo)
{
	mAlbedo = albedo;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_ALBEDO));
}

void Ape::PassImpl::setRoughness(float roughness)
{
	mRoughness = roughness;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_ROUGHNESS));
}

void Ape::PassImpl::setLightRoughnessOffset(float lightRoughnessOffset)
{
	mLightRoughnessOffset = lightRoughnessOffset;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_LIGHTROUGHNESSOFFSET));
}

void Ape::PassImpl::setF0(Ape::Color f0)
{
	mF0 = f0;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_F0));
}


Ape::Color Ape::PassImpl::getAlbedo()
{
	return mAlbedo;
}

float Ape::PassImpl::getRoughness()
{
	return mRoughness;
}

float Ape::PassImpl::getLightRoughnessOffset()
{
	return mLightRoughnessOffset;
}

Ape::Color Ape::PassImpl::getF0()
{
	return mF0;
}

void Ape::PassImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::PassImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAlbedo);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mRoughness);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLightRoughnessOffset);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mF0);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::PassImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAlbedo))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_ALBEDO));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mRoughness))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_ROUGHNESS));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightRoughnessOffset))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_LIGHTROUGHNESSOFFSET));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mF0))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_F0));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


