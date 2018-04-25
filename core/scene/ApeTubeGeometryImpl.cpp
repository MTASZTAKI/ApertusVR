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

#include "ApeTubeGeometryImpl.h"

Ape::TubeGeometryImpl::TubeGeometryImpl(std::string name, bool isHostCreated) : Ape::ITubeGeometry(name), Ape::Replica("TubeGeometry", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mParameters = Ape::GeometryTubeParameters();
	mMaterial = Ape::MaterialWeakPtr();
	mMaterialName = std::string();
}

Ape::TubeGeometryImpl::~TubeGeometryImpl()
{
	
}

void Ape::TubeGeometryImpl::setParameters(float height, float tile)
{
	mParameters.height = height;
	mParameters.tile = tile;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_TUBE_PARAMETERS));
}

Ape::GeometryTubeParameters Ape::TubeGeometryImpl::getParameters()
{
	return mParameters;
}

void Ape::TubeGeometryImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_TUBE_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

void Ape::TubeGeometryImpl::setMaterial(Ape::MaterialWeakPtr material)
{
	if (auto materialSP = material.lock())
	{
		mMaterial = material;
		mMaterialName = materialSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_TUBE_MATERIAL));
	}
	else
		mMaterial = Ape::MaterialWeakPtr();
}

Ape::MaterialWeakPtr Ape::TubeGeometryImpl::getMaterial()
{
	return mMaterial;
}

void Ape::TubeGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::TubeGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
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

void Ape::TubeGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_TUBE_PARAMETERS));
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpScene->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_TUBE_PARENTNODE));
	}
	RakNet::RakString materialName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, materialName))
	{
		if (auto material = std::static_pointer_cast<Ape::Material>(mpScene->getEntity(materialName.C_String()).lock()))
		{
			mMaterial = material;
			mMaterialName = material->getName();
			mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_TUBE_MATERIAL));
		}
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


