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
#include "ApeIndexedFaceSetGeometryImpl.h"

Ape::IndexedFaceSetGeometryImpl::IndexedFaceSetGeometryImpl(std::string name, bool isHostCreated) : Ape::IIndexedFaceSetGeometry(name), Ape::Replica("IndexedFaceSetGeometry", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mParameters = Ape::GeometryIndexedFaceSetParameters();
	mCoordinatesSize = 0;
	mIndicesSize = 0;
	mNormalsSize = 0;
	mColorsSize = 0;
	mTextureCoordinatesSize = 0;
}

Ape::IndexedFaceSetGeometryImpl::~IndexedFaceSetGeometryImpl()
{
	
}

void Ape::IndexedFaceSetGeometryImpl::setParameters(std::string groupName, Ape::GeometryCoordinates coordinates, Ape::GeometryIndices indices, Ape::GeometryNormals normals, bool generateNormals, Ape::GeometryColors colors, Ape::GeometryTextureCoordinates textureCoordinates, Ape::MaterialWeakPtr material)
{
	mCoordinatesSize = static_cast<int>(coordinates.size());
	mIndicesSize = static_cast<int>(indices.size());
	mNormalsSize = static_cast<int>(normals.size());
	mColorsSize = static_cast<int>(colors.size());
	mTextureCoordinatesSize = static_cast<int>(textureCoordinates.size());
	mParameters = Ape::GeometryIndexedFaceSetParameters(groupName, coordinates, indices, normals, generateNormals, colors, textureCoordinates, material);
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS));
}

Ape::GeometryIndexedFaceSetParameters Ape::IndexedFaceSetGeometryImpl::getParameters()
{
	return mParameters;
}

void Ape::IndexedFaceSetGeometryImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

Ape::MaterialWeakPtr Ape::IndexedFaceSetGeometryImpl::getMaterial()
{
	return mParameters.material;
}

void Ape::IndexedFaceSetGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::IndexedFaceSetGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	if (serializeParameters->whenLastSerialized == 0)
	{
		RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
		serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
		mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
		mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParameters.groupName.c_str()));

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mCoordinatesSize);
		for (auto item : mParameters.coordinates)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIndicesSize);
		for (auto item : mParameters.indices)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mNormalsSize);
		for (auto item : mParameters.normals)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters.generateNormals);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mColorsSize);
		for (auto item : mParameters.colors)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mTextureCoordinatesSize);
		for (auto item : mParameters.textureCoordinates)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParameters.materialName.c_str()));
		mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
		mVariableDeltaSerializer.EndSerialize(&serializationContext);
		return RakNet::RM3SR_SERIALIZED_ALWAYS;
	}
	return RakNet::RM3SR_DO_NOT_SERIALIZE;
}

void Ape::IndexedFaceSetGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	//std::cout << "deserialize" << std::endl;
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString groupName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, groupName))
		mParameters.groupName = groupName.C_String();
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
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mNormalsSize))
	{
		while (mParameters.normals.size() < mNormalsSize)
		{
			float item;
			if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, item))
				mParameters.normals.push_back(item);
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters.generateNormals))
		;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mColorsSize))
	{
		while (mParameters.colors.size() < mColorsSize)
		{
			float item;
			if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, item))
				mParameters.colors.push_back(item);
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mTextureCoordinatesSize))
	{
		while (mParameters.textureCoordinates.size() < mTextureCoordinatesSize)
		{
			float item;
			if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, item))
				mParameters.textureCoordinates.push_back(item);
		}
	}
	RakNet::RakString materialName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, materialName))
	{
		mParameters.materialName = materialName.C_String();
		if (auto entity = mpScene->getEntity(mParameters.materialName).lock())
			mParameters.material = std::static_pointer_cast<Ape::Material>(entity);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS));
	}
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpScene->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


