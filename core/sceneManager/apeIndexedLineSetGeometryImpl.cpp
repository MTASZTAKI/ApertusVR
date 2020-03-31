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

#include "apeIndexedLineSetGeometryImpl.h"

ape::IndexedLineSetGeometryImpl::IndexedLineSetGeometryImpl(std::string name, bool replicate, std::string ownerID, bool isHost) : ape::IIndexedLineSetGeometry(name, replicate, ownerID), ape::Replica("IndexedLineSetGeometry", name, ownerID, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mParameters = ape::GeometryIndexedLineSetParameters();
	mCoordinatesSize = 0;
	mIndicesSize = 0;
}

ape::IndexedLineSetGeometryImpl::~IndexedLineSetGeometryImpl()
{
	
}

void ape::IndexedLineSetGeometryImpl::setParameters(ape::GeometryCoordinates coordinates, ape::GeometryIndices indices, ape::Color color)
{
	mCoordinatesSize = static_cast<int>(coordinates.size());
	mIndicesSize = static_cast<int>(indices.size());
	mParameters.coordinates = coordinates;
	mParameters.indices = indices;
	mParameters.color = color;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARAMETERS));
}

ape::GeometryIndexedLineSetParameters ape::IndexedLineSetGeometryImpl::getParameters()
{
	return mParameters;
}

void ape::IndexedLineSetGeometryImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

void ape::IndexedLineSetGeometryImpl::setOwner(std::string ownerID)
{
	mOwnerID = ownerID;
}

std::string ape::IndexedLineSetGeometryImpl::getOwner()
{
	return mOwnerID;
}

void ape::IndexedLineSetGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str())); allocationIdBitstream->Write(RakNet::RakString(mOwnerID.c_str()));
}

RakNet::RM3SerializationResult ape::IndexedLineSetGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	/*if (serializeParameters->whenLastSerialized == 0)
	{*/
		RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
		serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
		mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mCoordinatesSize);
		for (auto item : mParameters.coordinates)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIndicesSize);
		for (auto item : mParameters.indices)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters.color);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));

		mVariableDeltaSerializer.EndSerialize(&serializationContext);
		return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
	/*}
	return RakNet::RM3SR_DO_NOT_SERIALIZE;*/
}

void ape::IndexedLineSetGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	APE_LOG_FUNC_ENTER();
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mCoordinatesSize))
	{
		while (mParameters.coordinates.size() < mCoordinatesSize)
		{
			float item;
			if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, item))
				mParameters.coordinates.push_back(item);
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIndicesSize))
	{
		while (mParameters.indices.size() < mIndicesSize)
		{
			int item;
			if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, item))
				mParameters.indices.push_back(item);
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters.color))
	{
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARAMETERS));
	}
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		if (auto parentNode = mpSceneManager->getNode(parentName.C_String()).lock())
		{
			mParentNode = parentNode;
			mParentNodeName = parentName.C_String();
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE));
		}
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
	APE_LOG_FUNC_LEAVE();
}


