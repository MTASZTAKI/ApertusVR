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

#include "ApeSkyImpl.h"

ape::SkyImpl::SkyImpl(std::string name, bool isHostCreated) : ape::ISky(name), ape::Replica("Sky", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mSkyLight = ape::LightWeakPtr();
	mSunLight = ape::LightWeakPtr();
	mTime = ape::ISky::Time();
	mSize = 0.0f;
}

ape::SkyImpl::~SkyImpl()
{
	
}

void ape::SkyImpl::setTime(float startTime, float sunRiseTime, float sunSetTime)
{
	mTime.currentTime = startTime;
	mTime.sunRiseTime = sunRiseTime;
	mTime.sunSetTime = sunSetTime;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_TIME));
}

ape::ISky::Time ape::SkyImpl::getTime()
{
	return mTime;
}

void ape::SkyImpl::setSunLight(ape::LightWeakPtr sunLight)
{
	mSunLight = sunLight;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_SUNLIGHT));
}

ape::LightWeakPtr ape::SkyImpl::getSunLight()
{
	return mSunLight;
}

void ape::SkyImpl::setSkyLight(ape::LightWeakPtr skyLight)
{
	mSkyLight = skyLight;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_SKYLIGHT));
}

ape::LightWeakPtr ape::SkyImpl::getSkyLight()
{
	return mSkyLight;
}

void ape::SkyImpl::setSize(float size)
{
	mSize = size;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_SIZE));
}

float ape::SkyImpl::getSize()
{
	return mSize;
}

void ape::SkyImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::SkyImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSunLight);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSkyLight);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mTime);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSize);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::SkyImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSunLight))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_SUNLIGHT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSkyLight))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_SKYLIGHT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mTime))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_TIME));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSize))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::SKY_SIZE));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
