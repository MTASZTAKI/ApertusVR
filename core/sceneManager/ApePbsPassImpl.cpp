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

#include "ApePbsPassImpl.h"

ape::PbsPassImpl::PbsPassImpl(std::string name, bool isHostCreated) : ape::IPbsPass(name), ape::Replica("PbsPass", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mAlbedo = ape::Color();
	mRoughness = 0.0f;
	mLightRoughnessOffset = 0.0f;
	mF0 = ape::Color();
}

ape::PbsPassImpl::~PbsPassImpl()
{
	
}

void ape::PbsPassImpl::setDiffuseColor(ape::Color diffuse)
{
	mDiffuseColor = diffuse;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_DIFFUSE));
}

void ape::PbsPassImpl::setSpecularColor(ape::Color specular)
{
	mSpecularColor = specular;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_SPECULAR));
}

void ape::PbsPassImpl::setAmbientColor(ape::Color ambient)
{
	mAmbientColor = ambient;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_AMBIENT));
}

void ape::PbsPassImpl::setEmissiveColor(ape::Color emissive)
{
	mEmissiveColor = emissive;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_EMISSIVE));
}

void ape::PbsPassImpl::setShininess(float shininess)
{
	mShininess = shininess;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_SHININESS));
}

void ape::PbsPassImpl::setAlbedo(ape::Color albedo)
{
	mAlbedo = albedo;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_ALBEDO));
}

void ape::PbsPassImpl::setRoughness(float roughness)
{
	mRoughness = roughness;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_ROUGHNESS));
}

void ape::PbsPassImpl::setLightRoughnessOffset(float lightRoughnessOffset)
{
	mLightRoughnessOffset = lightRoughnessOffset;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_LIGHTROUGHNESSOFFSET));
}

void ape::PbsPassImpl::setF0(ape::Color f0)
{
	mF0 = f0;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_F0));
}


ape::Color ape::PbsPassImpl::getAlbedo()
{
	return mAlbedo;
}

float ape::PbsPassImpl::getRoughness()
{
	return mRoughness;
}

float ape::PbsPassImpl::getLightRoughnessOffset()
{
	return mLightRoughnessOffset;
}

ape::Color ape::PbsPassImpl::getF0()
{
	return mF0;
}

void ape::PbsPassImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::PbsPassImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAmbientColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDiffuseColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSpecularColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mEmissiveColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mShininess);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAlbedo);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mRoughness);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLightRoughnessOffset);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mF0);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::PbsPassImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAmbientColor))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_AMBIENT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_DIFFUSE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_SPECULAR));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mEmissiveColor))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_EMISSIVE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mShininess))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_SHININESS));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAlbedo))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_ALBEDO));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mRoughness))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_ROUGHNESS));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightRoughnessOffset))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_LIGHTROUGHNESSOFFSET));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mF0))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::PASS_PBS_F0));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


