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

#include "ApeNodeImpl.h"

Ape::NodeImpl::NodeImpl(std::string name, bool isHostCreated) : Ape::Replica("Node", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mName = name;
	mParentNode = Ape::NodeWeakPtr();
	mParentNodeName = std::string();
	mPosition = Ape::Vector3();
	mScale = Ape::Vector3(1.0f, 1.0f, 1.0f);
	mOrientation = Ape::Quaternion();
	mChildrenVisibility = true;
	mIsFixedYaw = false;
	mIsInheritOrientation = true;
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

bool Ape::NodeImpl::getChildrenVisibility()
{
	return mChildrenVisibility;
}

bool Ape::NodeImpl::isFixedYaw()
{
	return mIsFixedYaw;
}

void Ape::NodeImpl::setParentNode(Ape::NodeWeakPtr newParentNode)
{
	if (auto parentNodeSP = mpSceneManager->getNode(mParentNodeName).lock())
	{
		((Ape::NodeImpl*)parentNodeSP.get())->removeChildNode(mpSceneManager->getNode(mName));
	}
	if (auto newParentNodeSP = newParentNode.lock())
	{
		mParentNode = newParentNode;
		mParentNodeName = newParentNodeSP->getName();
		((Ape::NodeImpl*)newParentNodeSP.get())->addChildNode(mpSceneManager->getNode(mName));
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

Ape::NodeWeakPtr Ape::NodeImpl::getParentNode()
{
	return mParentNode;
}

void Ape::NodeImpl::addChildNode(Ape::NodeWeakPtr node)
{
	mChildNodes.push_back(node);
}

std::vector<Ape::NodeWeakPtr> Ape::NodeImpl::getChildNodes()
{
	return mChildNodes;
}

bool Ape::NodeImpl::hasChildNode()
{
	return mChildNodes.size() > 0;
}

bool Ape::NodeImpl::isChildNode(Ape::NodeWeakPtr childNode)
{
	if (auto childNodeSP = childNode.lock())
	{
		for (std::vector<Ape::NodeWeakPtr>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it)
		{
			if (auto itemSP = it->lock())
			{
				if (itemSP->getName() == childNodeSP->getName())
				{
					return true;
				}
			}
		}
	}
	return false;
}

void Ape::NodeImpl::removeChildNode(Ape::NodeWeakPtr childNode)
{
	if (auto childNodeSP = childNode.lock())
	{
		std::vector<Ape::NodeWeakPtr>::iterator it = mChildNodes.begin();
		for (; it != mChildNodes.end(); ++it)
		{
			if (auto itemSP = it->lock())
			{
				if (itemSP->getName() == childNodeSP->getName())
				{
					break;
				}
			}
		}
		if (it != mChildNodes.end())
		{
			if (auto itemSP = it->lock())
			{
				mChildNodes.erase(it);
			}
		}
	}
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

void Ape::NodeImpl::setChildrenVisibility(bool visible)
{
	mChildrenVisibility = visible;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_CHILDVISIBILITY));
}

void Ape::NodeImpl::setFixedYaw(bool fix)
{
	mIsFixedYaw = fix;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_FIXEDYAW));
}

void Ape::NodeImpl::showBoundingBox(bool show)
{
	mIsBoundingBoxVisible = show;
	if (show)
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_SHOWBOUNDINGBOX));
	else
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_HIDEBOUNDINGBOX));
}

void Ape::NodeImpl::setInheritOrientation(bool enable)
{
	mIsInheritOrientation = enable;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_INHERITORIENTATION));
}

bool Ape::NodeImpl::isInheritOrientation()
{
	return mIsInheritOrientation;
}

void Ape::NodeImpl::setInitalState()
{
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_INHERITORIENTATION));
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
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mChildrenVisibility);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsFixedYaw);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsInheritOrientation);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
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
		mParentNode = mpSceneManager->getNode(mParentNodeName);
		if (auto parentNode = mParentNode.lock())
		{
			((Ape::NodeImpl*)parentNode.get())->addChildNode(mpSceneManager->getNode(mName));
		}
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_PARENTNODE));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mScale))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_SCALE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mChildrenVisibility))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_CHILDVISIBILITY));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsFixedYaw))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_FIXEDYAW));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsInheritOrientation))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::NODE_INHERITORIENTATION));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}