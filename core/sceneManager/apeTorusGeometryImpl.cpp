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

#include "apeTorusGeometryImpl.h"

ape::TorusGeometryImpl::TorusGeometryImpl(std::string name, bool replicate, std::string ownerID, bool isHost) : ape::ITorusGeometry(name, replicate, ownerID), ape::Replica("TorusGeometry", name, ownerID, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mParameters = ape::GeometryTorusParameters();
	mMaterial = ape::MaterialWeakPtr();
	mMaterialName = std::string();
}

ape::TorusGeometryImpl::~TorusGeometryImpl()
{
	
}

void ape::TorusGeometryImpl::setParameters(float radius, float sectionRadius, ape::Vector2 tile)
{
	mParameters.radius = radius;
	mParameters.sectionRadius = sectionRadius;
	mParameters.tile = tile;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TORUS_PARAMETERS));
}

ape::GeometryTorusParameters ape::TorusGeometryImpl::getParameters()
{
	return mParameters;
}

void ape::TorusGeometryImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TORUS_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

void ape::TorusGeometryImpl::setMaterial(ape::MaterialWeakPtr material)
{
	if (auto materialSP = material.lock())
	{
		mMaterial = material;
		mMaterialName = materialSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TORUS_MATERIAL));
	}
	else
		mMaterial = ape::MaterialWeakPtr();
}

ape::MaterialWeakPtr ape::TorusGeometryImpl::getMaterial()
{
	return mMaterial;
}

void ape::TorusGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::TorusGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mMaterialName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::TorusGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TORUS_PARAMETERS));
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpSceneManager->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TORUS_PARENTNODE));
	}
	RakNet::RakString materialName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, materialName))
	{
		if (auto material = std::static_pointer_cast<ape::Material>(mpSceneManager->getEntity(materialName.C_String()).lock()))
		{
			mMaterial = material;
			mMaterialName = material->getName();
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TORUS_MATERIAL));
		}
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


