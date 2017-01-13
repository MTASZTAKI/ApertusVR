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

#include "ApeNodeImpl.h"

Ape::NodeImpl::NodeImpl(std::string name, bool isHostCreated) : Ape::Replica("Node", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mName = name;
	mParentNode = Ape::NodeWeakPtr();
	mParentNodeName = std::string();
	mPosition = Ape::Vector3();
	mScale = Ape::Vector3(1.0f, 1.0f, 1.0f);
	mOrientation = Ape::Quaternion();
}

Ape::NodeImpl::~NodeImpl()
{
	
}

std::string Ape::NodeImpl::getName() const
{
	return mName;
}

Ape::Vector3 Ape::NodeImpl::getPosition() const
{
	return mPosition;
}

Ape::Quaternion Ape::NodeImpl::getOrientation() const
{
	return mOrientation;
}

Ape::Vector3 Ape::NodeImpl::getScale() const
{
	return mScale;
}

Ape::Vector3 Ape::NodeImpl::getDerivedPosition() const
{
	if (auto parentNode = mParentNode.lock())
		return parentNode->getDerivedPosition() + (parentNode->getDerivedOrientation() * (parentNode->getDerivedScale() * mPosition));
	else 
		return mPosition;
}

Ape::Quaternion Ape::NodeImpl::getDerivedOrientation() const
{
	if (auto parentNode = mParentNode.lock())
		return parentNode->getDerivedOrientation() * mOrientation;
	else
		return mOrientation;
}

Ape::Vector3 Ape::NodeImpl::getDerivedScale() const
{
	if (auto parentNode = mParentNode.lock())
		return parentNode->getDerivedScale() * mScale;
	else
		return mScale;
}

void Ape::NodeImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

Ape::NodeWeakPtr Ape::NodeImpl::getParentNode()
{
	return mParentNode;
}

void Ape::NodeImpl::setPosition( Vector3 position )
{
	mPosition = position;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_POSITION));
}

void Ape::NodeImpl::setOrientation( Quaternion orientation )
{
	mOrientation = orientation;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_ORIENTATION));
}

void Ape::NodeImpl::setScale( Vector3 scale )
{
	mScale = scale;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_SCALE));
}

void Ape::NodeImpl::translate(Vector3 transformVector, Ape::Node::TransformationSpace nodeTransformSpace )
{
	switch(nodeTransformSpace)
	{
	case Ape::Node::TransformationSpace::LOCAL:
		setPosition(mPosition + (mOrientation * transformVector));
		break;
	case Ape::Node::TransformationSpace::WORLD:
	{
		if (auto parentNode = mParentNode.lock())
			setPosition(mPosition + ((parentNode->getDerivedOrientation().Inverse() * transformVector) / parentNode->getDerivedScale()));
		else 
			setPosition(mPosition + transformVector);
		break;
	}
	case Ape::Node::TransformationSpace::PARENT:
		setPosition(mPosition + transformVector);
		break;
	case Ape::Node::TransformationSpace::INVALID:
		break;
	default:
		break;
	}
}

void Ape::NodeImpl::rotate( Radian angle, Vector3 axis, Ape::Node::TransformationSpace nodeTransformSpace )
{
	Quaternion qnorm;
	qnorm.FromAngleAxis(angle, axis);
	qnorm.normalise();

	switch (nodeTransformSpace)
	{
	case Ape::Node::TransformationSpace::PARENT:
		setOrientation(qnorm * mOrientation);
		break;
	case Ape::Node::TransformationSpace::WORLD:
		setOrientation(mOrientation * getDerivedOrientation().Inverse() * qnorm * getDerivedOrientation());
		break;
	case Ape::Node::TransformationSpace::LOCAL:
		setOrientation(mOrientation * qnorm);
		break;
	case Ape::Node::TransformationSpace::INVALID:
	default:
		break;
	}
}

void Ape::NodeImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::NodeImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mPosition);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mOrientation);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mScale);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::NodeImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mPosition))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_POSITION));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mOrientation))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_ORIENTATION));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		mParentNode = mpScene->getNode(mParentNodeName);
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_PARENTNODE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mScale))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_SCALE));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}