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

#include "apeLightImpl.h"

ape::LightImpl::LightImpl(std::string name, bool isHost) : ape::ILight(name), ape::Replica("Light", name, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mLightType = ape::Light::Type::INVALID;
	mDiffuseColor = ape::Color();
	mSpecularColor = ape::Color();
	mLightSpotRange = ape::LightSpotRange();
	mLightAttenuation = ape::LightAttenuation();
	mLightDirection = ape::Vector3();
	mParentNode = ape::NodeWeakPtr();
	mParentNodeName = std::string();
}

ape::LightImpl::~LightImpl()
{
	
}

ape::Light::Type ape::LightImpl::getLightType()
{
	return mLightType;
}

ape::Color ape::LightImpl::getDiffuseColor()
{
	return mDiffuseColor;
}

ape::Color ape::LightImpl::getSpecularColor()
{
	return mSpecularColor;
}

ape::LightSpotRange ape::LightImpl::getLightSpotRange()
{
	return mLightSpotRange;
}

ape::LightAttenuation ape::LightImpl::getLightAttenuation()
{
	return mLightAttenuation;
}

ape::Vector3 ape::LightImpl::getLightDirection()
{
	return mLightDirection;
}

ape::NodeWeakPtr ape::LightImpl::getParentNode()
{
	return mParentNode;
}

void ape::LightImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

void ape::LightImpl::setDiffuseColor( Color diffuseColor )
{
	mDiffuseColor = diffuseColor;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_DIFFUSE));
}

void ape::LightImpl::setSpecularColor( Color specularColor )
{
	mSpecularColor = specularColor;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_SPECULAR));
}

void ape::LightImpl::setLightSpotRange(LightSpotRange lightSpotRange)
{
	mLightSpotRange = lightSpotRange;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_SPOTRANGE));
}

void ape::LightImpl::setLightAttenuation(LightAttenuation lightAttenuation)
{
	mLightAttenuation = lightAttenuation;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_ATTENUATION));
}

void ape::LightImpl::setLightDirection( Vector3 lightDirection )
{
	mLightDirection = lightDirection;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_DIRECTION));
}

void ape::LightImpl::setLightType( ape::Light::Type lightType )
{
	mLightType = lightType;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_TYPE));
}

void ape::LightImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::LightImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLightType);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mDiffuseColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mSpecularColor);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLightSpotRange);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLightAttenuation);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLightDirection);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::LightImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightType))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_TYPE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_DIFFUSE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_SPECULAR));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightSpotRange))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_SPOTRANGE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightAttenuation))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_ATTENUATION));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightDirection))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_DIRECTION));
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpSceneManager->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::LIGHT_PARENTNODE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
