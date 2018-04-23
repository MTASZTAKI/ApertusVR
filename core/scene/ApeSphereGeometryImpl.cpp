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

#include <iostream>
#include "ApeSphereGeometryImpl.h"

Ape::SphereGeometryImpl::SphereGeometryImpl(std::string name, bool isHostCreated) : Ape::ISphereGeometry(name), Ape::Replica("SphereGeometry", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mParameters = Ape::GeometrySphereParameters();
	mMaterial = Ape::MaterialWeakPtr();
	mMaterialName = std::string();
}

Ape::SphereGeometryImpl::~SphereGeometryImpl()
{
	
}

void Ape::SphereGeometryImpl::setParameters(float radius, Ape::Vector2 tile)
{
	mParameters.radius = radius;
	mParameters.tile = tile;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS));
}

Ape::GeometrySphereParameters Ape::SphereGeometryImpl::getParameters()
{
	return mParameters;
}

void Ape::SphereGeometryImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

void Ape::SphereGeometryImpl::setMaterial(Ape::MaterialWeakPtr material)
{
	if (auto materialSP = material.lock())
	{
		mMaterial = material;
		mMaterialName = materialSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_SPHERE_MATERIAL));
	}
	else
		mMaterial = Ape::MaterialWeakPtr();
}

Ape::MaterialWeakPtr Ape::SphereGeometryImpl::getMaterial()
{
	return mMaterial;
}

void Ape::SphereGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::SphereGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mMaterialName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::SphereGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS));
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpScene->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE));
	}
	RakNet::RakString materialName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, materialName))
	{
		if (auto material = std::static_pointer_cast<Ape::Material>(mpScene->getEntity(materialName.C_String()).lock()))
		{
			//LOG(LOG_TYPE_DEBUG, "Deserialize materialName " << materialName.C_String());
			mMaterial = material;
			mMaterialName = material->getName();
			mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_SPHERE_MATERIAL));
		}
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


