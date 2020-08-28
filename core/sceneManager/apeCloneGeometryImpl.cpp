#include "apeCloneGeometryImpl.h"
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

ape::CloneGeometryImpl::CloneGeometryImpl(std::string name, bool replicate, std::string ownerID, bool isHost)
	: ape::ICloneGeometry(name, replicate, ownerID), ape::Replica("CloneGeometry", name, ownerID, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mSourceGeometryName = std::string();
	mParentNodeName = std::string();
}

ape::CloneGeometryImpl::~CloneGeometryImpl()
{
}

void ape::CloneGeometryImpl::setSourceGeometry(ape::GeometryWeakPtr sourceGeometryWeak)
{
	/*printf("SET SOURCE GEOMETRY\n");*/
	if (auto sourceGeometryShared = sourceGeometryWeak.lock())
	{
		mSourceGeometry = sourceGeometryWeak;
		mSourceGeometryName = sourceGeometryShared->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY));
	}
	else
		mSourceGeometry = ape::GeometryWeakPtr();
}

void ape::CloneGeometryImpl::setSourceGeometryGroupName(std::string sourceGeometryName)
{
	/*printf("SET SOURCE GEOMETRY GROUP NAME\n");*/
	mSourceGeometryGroupName = sourceGeometryName;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME));
}

void ape::CloneGeometryImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeShared = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeShared->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_CLONE_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

ape::GeometryWeakPtr ape::CloneGeometryImpl::getSourceGeometry()
{
	return mSourceGeometry;
}

std::string ape::CloneGeometryImpl::getSourceGeometryName()
{
	return mSourceGeometryName;
}

std::string ape::CloneGeometryImpl::getSourceGeometryGroupName()
{
	return mSourceGeometryGroupName;
}

void ape::CloneGeometryImpl::setOwner(std::string ownerID)
{
	mOwnerID = ownerID;
}

std::string ape::CloneGeometryImpl::getOwner()
{
	return mOwnerID;
}

void ape::CloneGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str())); allocationIdBitstream->Write(RakNet::RakString(mOwnerID.c_str()));
}

RakNet::RM3SerializationResult ape::CloneGeometryImpl::Serialize(RakNet::SerializeParameters * serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mSourceGeometryName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mSourceGeometryGroupName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::CloneGeometryImpl::Deserialize(RakNet::DeserializeParameters * deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString sourceGeometryName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, sourceGeometryName))
	{
		if (auto sourceGeometry = std::static_pointer_cast<ape::Geometry>(mpSceneManager->getEntity(sourceGeometryName.C_String()).lock()))
		{
			mSourceGeometry = sourceGeometry;
			mSourceGeometryName = sourceGeometry->getName();
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY));
		}
	}
	RakNet::RakString sourceGeometryGroupName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, sourceGeometryGroupName))
	{
		mSourceGeometryGroupName = sourceGeometryGroupName.C_String();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME));
	}
	RakNet::RakString parentNodeName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentNodeName))
	{
		mParentNodeName = parentNodeName.C_String();
		mParentNode = mpSceneManager->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_CLONE_PARENTNODE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
