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

#include <iostream>
#include "apeIndexedFaceSetGeometryImpl.h"

ape::IndexedFaceSetGeometryImpl::IndexedFaceSetGeometryImpl(std::string name, bool replicate, std::string ownerID, bool isHost) : ape::IIndexedFaceSetGeometry(name, replicate, ownerID), ape::Replica("IndexedFaceSetGeometry", name, ownerID, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mParameters = ape::GeometryIndexedFaceSetParameters();
	mCoordinatesSize = 0;
	mIndicesSize = 0;
	mNormalsSize = 0;
	mColorsSize = 0;
	mTextureCoordinatesSize = 0;
}

ape::IndexedFaceSetGeometryImpl::~IndexedFaceSetGeometryImpl()
{
	
}

void ape::IndexedFaceSetGeometryImpl::setParameters(std::string groupName, ape::GeometryCoordinates coordinates, ape::GeometryIndices indices, ape::GeometryNormals normals, bool generateNormals, ape::GeometryColors colors, ape::GeometryTextureCoordinates textureCoordinates, ape::MaterialWeakPtr material)
{
	mCoordinatesSize = static_cast<int>(coordinates.size());
	mIndicesSize = static_cast<int>(indices.size());
	mNormalsSize = static_cast<int>(normals.size());
	mColorsSize = static_cast<int>(colors.size());
	mTextureCoordinatesSize = static_cast<int>(textureCoordinates.size());
	mParameters = ape::GeometryIndexedFaceSetParameters(groupName, coordinates, indices, normals, generateNormals, colors, textureCoordinates, material);
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS));
}

ape::GeometryIndexedFaceSetParameters ape::IndexedFaceSetGeometryImpl::getParameters()
{
	return mParameters;
}

void ape::IndexedFaceSetGeometryImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

ape::MaterialWeakPtr ape::IndexedFaceSetGeometryImpl::getMaterial()
{
	return mParameters.material;
}

void ape::IndexedFaceSetGeometryImpl::setOwner(std::string ownerID)
{
	mOwnerID = ownerID;
}

std::string ape::IndexedFaceSetGeometryImpl::getOwner()
{
	return mOwnerID;
}

void ape::IndexedFaceSetGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
	allocationIdBitstream->Write(RakNet::RakString(mOwnerID.c_str()));
	allocationIdBitstream->Write(RakNet::RakString(mParameters.groupName.c_str()));
	allocationIdBitstream->Write(mCoordinatesSize);
	for (auto item : mParameters.coordinates)
		allocationIdBitstream->Write(item);
	allocationIdBitstream->Write(mIndicesSize);
	for (auto item : mParameters.indices)
		allocationIdBitstream->Write(item);
	allocationIdBitstream->Write(mNormalsSize);
	for (auto item : mParameters.normals)
		allocationIdBitstream->Write(item);
	allocationIdBitstream->Write(mParameters.generateNormals);
	allocationIdBitstream->Write(mColorsSize);
	for (auto item : mParameters.colors)
		allocationIdBitstream->Write(item);
	allocationIdBitstream->Write(mTextureCoordinatesSize);
	for (auto item : mParameters.textureCoordinates)
		allocationIdBitstream->Write(item);
	allocationIdBitstream->Write(RakNet::RakString(mParameters.materialName.c_str()));
}

RakNet::RM3SerializationResult ape::IndexedFaceSetGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::IndexedFaceSetGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	APE_LOG_FUNC_ENTER();
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		if (auto parentNode = mpSceneManager->getNode(parentName.C_String()).lock())
		{
			mParentNode = parentNode;
			mParentNodeName = parentName.C_String();
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE));
		}
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
	APE_LOG_FUNC_LEAVE();
}


