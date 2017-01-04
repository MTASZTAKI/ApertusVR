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

#include "ApePbsPassImpl.h"

Ape::PbsPassImpl::PbsPassImpl(std::string name, bool isHostCreated) : Ape::IPbsPass(name), Ape::Replica("PbsPass", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mAlbedo = Ape::Color();
	mRoughness = 0.0f;
	mLightRoughnessOffset = 0.0f;
	mF0 = Ape::Color();
}

Ape::PbsPassImpl::~PbsPassImpl()
{
	
}

void Ape::PbsPassImpl::setDiffuseColor(Ape::Color diffuse)
{
	mDiffuseColor = diffuse;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_DIFFUSE));
}

void Ape::PbsPassImpl::setSpecularColor(Ape::Color specular)
{
	mSpecularColor = specular;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_DIFFUSE));
}

void Ape::PbsPassImpl::setAmbientColor(Ape::Color ambient)
{
	mAmbientColor = ambient;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_DIFFUSE));
}

void Ape::PbsPassImpl::setEmissiveColor(Ape::Color emissive)
{
	mEmissiveColor = emissive;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_DIFFUSE));
}

void Ape::PbsPassImpl::setShininess(float shininess)
{
	mShininess = shininess;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_DIFFUSE));
}

void Ape::PbsPassImpl::setAlbedo(Ape::Color albedo)
{
	mAlbedo = albedo;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_ALBEDO));
}

void Ape::PbsPassImpl::setRoughness(float roughness)
{
	mRoughness = roughness;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_ROUGHNESS));
}

void Ape::PbsPassImpl::setLightRoughnessOffset(float lightRoughnessOffset)
{
	mLightRoughnessOffset = lightRoughnessOffset;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_LIGHTROUGHNESSOFFSET));
}

void Ape::PbsPassImpl::setF0(Ape::Color f0)
{
	mF0 = f0;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_F0));
}


Ape::Color Ape::PbsPassImpl::getAlbedo()
{
	return mAlbedo;
}

float Ape::PbsPassImpl::getRoughness()
{
	return mRoughness;
}

float Ape::PbsPassImpl::getLightRoughnessOffset()
{
	return mLightRoughnessOffset;
}

Ape::Color Ape::PbsPassImpl::getF0()
{
	return mF0;
}

void Ape::PbsPassImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::PbsPassImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
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
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::PbsPassImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAmbientColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_AMBIENT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_DIFFUSE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_SPECULAR));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mEmissiveColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_EMISSIVE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mShininess))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_SHININESS));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAlbedo))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_ALBEDO));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mRoughness))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_ROUGHNESS));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightRoughnessOffset))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_LIGHTROUGHNESSOFFSET));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mF0))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::PASS_PBS_F0));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


