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
#include "ApeRayGeometryImpl.h"

Ape::RayGeometryImpl::RayGeometryImpl(std::string name, bool isHostCreated) : Ape::IRayGeometry(name), Ape::Replica("RayGeometry", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mIntersectingEnabled = false;
	mIntersections = std::vector<Ape::GeometryWeakPtr>();
	mParentNode = Ape::NodeWeakPtr();
}

Ape::RayGeometryImpl::~RayGeometryImpl()
{
	
}

void Ape::RayGeometryImpl::setIntersectingEnabled(bool enable)
{
	mIntersectingEnabled = enable;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_RAY_INTERSECTIONENABLE));
}

void Ape::RayGeometryImpl::setIntersections(std::vector<Ape::GeometryWeakPtr> intersections)
{
	mIntersections = intersections;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_RAY_INTERSECTION));
}

void Ape::RayGeometryImpl::fireIntersectionQuery()
{
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY));
}

void Ape::RayGeometryImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_RAY_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

void Ape::RayGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::RayGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIntersectingEnabled);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIntersections);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void Ape::RayGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIntersectingEnabled))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_RAY_INTERSECTIONENABLE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIntersections))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_RAY_INTERSECTION));
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpScene->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_RAY_PARENTNODE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


