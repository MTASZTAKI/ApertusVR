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

Ape::SkyImpl::SkyImpl(std::string name, bool isHostCreated) : Ape::ISky(name), Ape::Replica("Sky", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mSkyLight = Ape::LightWeakPtr();
	mSunLight = Ape::LightWeakPtr();
	mTime = Ape::ISky::Time();
	mSize = 0.0f;
}

Ape::SkyImpl::~SkyImpl()
{
	
}

void Ape::SkyImpl::setTime(float startTime, float sunRiseTime, float sunSetTime)
{
	mTime.currentTime = startTime;
	mTime.sunRiseTime = sunRiseTime;
	mTime.sunSetTime = sunSetTime;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_TIME));
}

Ape::ISky::Time Ape::SkyImpl::getTime()
{
	return mTime;
}

void Ape::SkyImpl::setSunLight(Ape::LightWeakPtr sunLight)
{
	mSunLight = sunLight;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_SUNLIGHT));
}

Ape::LightWeakPtr Ape::SkyImpl::getSunLight()
{
	return mSunLight;
}

void Ape::SkyImpl::setSkyLight(Ape::LightWeakPtr skyLight)
{
	mSkyLight = skyLight;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_SKYLIGHT));
}

Ape::LightWeakPtr Ape::SkyImpl::getSkyLight()
{
	return mSkyLight;
}

void Ape::SkyImpl::setSize(float size)
{
	mSize = size;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_SIZE));
}

float Ape::SkyImpl::getSize()
{
	return mSize;
}

void Ape::SkyImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::SkyImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSunLight);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSkyLight);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mTime);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSize);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::SkyImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSunLight))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_SUNLIGHT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSkyLight))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_SKYLIGHT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mTime))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_TIME));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSize))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SKY_SIZE));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
