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

#include "apeNodeImpl.h"

ape::NodeImpl::NodeImpl(std::string name, bool replicate, std::string ownerID, bool isHost) : ape::Replica("Node", name, ownerID, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mName = name;
	mParentNode = ape::NodeWeakPtr();
	mParentNodeName = std::string();
	mPosition = ape::Vector3();
	mScale = ape::Vector3(1.0f, 1.0f, 1.0f);
	mOrientation = ape::Quaternion();
	mChildrenVisibility = true;
	mIsFixedYaw = false;
	mIsInheritOrientation = true;
	mIsReplicated = replicate;
}

ape::NodeImpl::~NodeImpl()
{
	
}

std::string ape::NodeImpl::getName() const
{
	return mName;
}

ape::Vector3 ape::NodeImpl::getPosition() const
{
	return mPosition;
}

ape::Quaternion ape::NodeImpl::getOrientation() const
{
	return mOrientation;
}

ape::Vector3 ape::NodeImpl::getScale() const
{
	return mScale;
}

ape::Vector3 ape::NodeImpl::getDerivedPosition() const
{
	if (auto parentNode = mParentNode.lock())
		return parentNode->getDerivedPosition() + (parentNode->getDerivedOrientation() * (parentNode->getDerivedScale() * mPosition));
	else 
		return mPosition;
}

ape::Quaternion ape::NodeImpl::getDerivedOrientation() const
{
	if (auto parentNode = mParentNode.lock())
		return parentNode->getDerivedOrientation() * mOrientation;
	else
		return mOrientation;
}

ape::Vector3 ape::NodeImpl::getDerivedScale() const
{
	if (auto parentNode = mParentNode.lock())
		return parentNode->getDerivedScale() * mScale;
	else
		return mScale;
}

bool ape::NodeImpl::getChildrenVisibility()
{
	return mChildrenVisibility;
}

bool ape::NodeImpl::isFixedYaw()
{
	return mIsFixedYaw;
}

void ape::NodeImpl::detachFromParentNode()
{
	if (auto parentNode = mParentNode.lock())
	{
		((ape::NodeImpl*)parentNode.get())->removeChildNode(mpSceneManager->getNode(mName));
		mParentNode = ape::NodeWeakPtr();
		mParentNodeName = "";
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_DETACH));
	}
}

void ape::NodeImpl::setParentNode(ape::NodeWeakPtr newParentNode)
{
	if (auto parentNodeSP = mpSceneManager->getNode(mParentNodeName).lock())
	{
		((ape::NodeImpl*)parentNodeSP.get())->removeChildNode(mpSceneManager->getNode(mName));
	}
	if (auto newParentNodeSP = newParentNode.lock())
	{
		mParentNode = newParentNode;
		mParentNodeName = newParentNodeSP->getName();
		((ape::NodeImpl*)newParentNodeSP.get())->addChildNode(mpSceneManager->getNode(mName));
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

ape::NodeWeakPtr ape::NodeImpl::getParentNode()
{
	return mParentNode;
}

void ape::NodeImpl::addChildNode(ape::NodeWeakPtr node)
{
	mChildNodes.push_back(node);
}

std::vector<ape::NodeWeakPtr> ape::NodeImpl::getChildNodes()
{
	return mChildNodes;
}

bool ape::NodeImpl::hasChildNode()
{
	return mChildNodes.size() > 0;
}

bool ape::NodeImpl::isChildNode(ape::NodeWeakPtr childNode)
{
	if (auto childNodeSP = childNode.lock())
	{
		for (std::vector<ape::NodeWeakPtr>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it)
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

void ape::NodeImpl::removeChildNode(ape::NodeWeakPtr childNode)
{
	if (auto childNodeSP = childNode.lock())
	{
		std::vector<ape::NodeWeakPtr>::iterator it = mChildNodes.begin();
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

void ape::NodeImpl::setPosition( Vector3 position )
{
	mPosition = position;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_POSITION));
}

void ape::NodeImpl::setOrientation( Quaternion orientation )
{
	mOrientation = orientation;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_ORIENTATION));
}

void ape::NodeImpl::setScale( Vector3 scale )
{
	mScale = scale;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_SCALE));
}

void ape::NodeImpl::setChildrenVisibility(bool visible)
{
	mChildrenVisibility = visible;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_CHILDVISIBILITY));
}

void ape::NodeImpl::setFixedYaw(bool fix)
{
	mIsFixedYaw = fix;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_FIXEDYAW));
}

void ape::NodeImpl::showBoundingBox(bool show)
{
	mIsBoundingBoxVisible = show;
	if (show)
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_SHOWBOUNDINGBOX));
	else
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_HIDEBOUNDINGBOX));
}

void ape::NodeImpl::setInheritOrientation(bool enable)
{
	mIsInheritOrientation = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_INHERITORIENTATION));
}

bool ape::NodeImpl::isInheritOrientation()
{
	return mIsInheritOrientation;
}

void ape::NodeImpl::setInitalState()
{
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_INHERITORIENTATION));
}

bool ape::NodeImpl::isReplicated()
{
	return mIsReplicated;
}

void ape::NodeImpl::setOwner(std::string ownerID)
{
	mOwnerID = ownerID;
}

std::string ape::NodeImpl::getOwner()
{
	return mOwnerID;
}

void ape::NodeImpl::translate(Vector3 transformVector, ape::Node::TransformationSpace nodeTransformSpace )
{
	switch(nodeTransformSpace)
	{
	case ape::Node::TransformationSpace::LOCAL:
		setPosition(mPosition + (mOrientation * transformVector));
		break;
	case ape::Node::TransformationSpace::WORLD:
	{
		if (auto parentNode = mParentNode.lock())
			setPosition(mPosition + ((parentNode->getDerivedOrientation().Inverse() * transformVector) / parentNode->getDerivedScale()));
		else 
			setPosition(mPosition + transformVector);
		break;
	}
	case ape::Node::TransformationSpace::PARENT:
		setPosition(mPosition + transformVector);
		break;
	case ape::Node::TransformationSpace::INVALID:
		break;
	default:
		break;
	}
}

void ape::NodeImpl::rotate( Radian angle, Vector3 axis, ape::Node::TransformationSpace nodeTransformSpace )
{
	Quaternion qnorm;
	qnorm.FromAngleAxis(angle, axis);
	qnorm.normalise();

	switch (nodeTransformSpace)
	{
	case ape::Node::TransformationSpace::PARENT:
		setOrientation(qnorm * mOrientation);
		break;
	case ape::Node::TransformationSpace::WORLD:
		setOrientation(mOrientation * getDerivedOrientation().Inverse() * qnorm * getDerivedOrientation());
		break;
	case ape::Node::TransformationSpace::LOCAL:
		setOrientation(mOrientation * qnorm);
		break;
	case ape::Node::TransformationSpace::INVALID:
	default:
		break;
	}
}

void ape::NodeImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
	allocationIdBitstream->Write(RakNet::RakString(mOwnerID.c_str()));
}

RakNet::RM3SerializationResult ape::NodeImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mScale);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mPosition);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mOrientation);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mChildrenVisibility);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsFixedYaw);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsInheritOrientation);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mOwnerID.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::NodeImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString parentName;
	RakNet::RakString ownerID;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mScale))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_SCALE));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mPosition))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_POSITION));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mOrientation))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_ORIENTATION));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		mParentNodeName = parentName.C_String();
		if (mParentNodeName == "")
		{
			detachFromParentNode();
		}
		else
		{
			mParentNode = mpSceneManager->getNode(mParentNodeName);
			setParentNode(mParentNode);
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mChildrenVisibility))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_CHILDVISIBILITY));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsFixedYaw))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_FIXEDYAW));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsInheritOrientation))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::NODE_INHERITORIENTATION));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, ownerID))
	{
		APE_LOG_DEBUG("Hey, the replica: " << mName << " is requested to be owned by the system: " << ownerID);
		setOwner(ownerID.C_String());
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}