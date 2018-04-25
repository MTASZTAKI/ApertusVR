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

#include "ApeLightImpl.h"

Ape::LightImpl::LightImpl(std::string name, bool isHostCreated) : Ape::ILight(name), Ape::Replica("Light", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mLightType = Ape::Light::Type::INVALID;
	mDiffuseColor = Ape::Color();
	mSpecularColor = Ape::Color();
	mLightSpotRange = Ape::LightSpotRange();
	mLightAttenuation = Ape::LightAttenuation();
	mLightDirection = Ape::Vector3();
	mParentNode = Ape::NodeWeakPtr();
	mParentNodeName = std::string();
}

Ape::LightImpl::~LightImpl()
{
	
}

Ape::Light::Type Ape::LightImpl::getLightType()
{
	return mLightType;
}

Ape::Color Ape::LightImpl::getDiffuseColor()
{
	return mDiffuseColor;
}

Ape::Color Ape::LightImpl::getSpecularColor()
{
	return mSpecularColor;
}

Ape::LightSpotRange Ape::LightImpl::getLightSpotRange()
{
	return mLightSpotRange;
}

Ape::LightAttenuation Ape::LightImpl::getLightAttenuation()
{
	return mLightAttenuation;
}

Ape::Vector3 Ape::LightImpl::getLightDirection()
{
	return mLightDirection;
}

Ape::NodeWeakPtr Ape::LightImpl::getParentNode()
{
	return mParentNode;
}

void Ape::LightImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

void Ape::LightImpl::setDiffuseColor( Color diffuseColor )
{
	mDiffuseColor = diffuseColor;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_DIFFUSE));
}

void Ape::LightImpl::setSpecularColor( Color specularColor )
{
	mSpecularColor = specularColor;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_SPECULAR));
}

void Ape::LightImpl::setLightSpotRange(LightSpotRange lightSpotRange)
{
	mLightSpotRange = lightSpotRange;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_SPOTRANGE));
}

void Ape::LightImpl::setLightAttenuation(LightAttenuation lightAttenuation)
{
	mLightAttenuation = lightAttenuation;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_ATTENUATION));
}

void Ape::LightImpl::setLightDirection( Vector3 lightDirection )
{
	mLightDirection = lightDirection;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_DIRECTION));
}

void Ape::LightImpl::setLightType( Ape::Light::Type lightType )
{
	mLightType = lightType;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_TYPE));
}

void Ape::LightImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::LightImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
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
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::LightImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightType))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_TYPE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mDiffuseColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_DIFFUSE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mSpecularColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_SPECULAR));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightSpotRange))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_SPOTRANGE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightAttenuation))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_ATTENUATION));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLightDirection))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_DIRECTION));
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpScene->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::LIGHT_PARENTNODE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
