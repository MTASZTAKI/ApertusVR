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
#include "apeTextGeometryImpl.h"

ape::TextGeometryImpl::TextGeometryImpl(std::string name, bool replicate, bool isHost) : ape::ITextGeometry(name, replicate), ape::Replica("TextGeometry", name, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mCaption = "";
	mVisibility = false;
	mShowOnTop = false;
}

ape::TextGeometryImpl::~TextGeometryImpl()
{
	
}

std::string ape::TextGeometryImpl::getCaption()
{
	return mCaption;
}

void ape::TextGeometryImpl::setCaption( std::string caption )
{
	mCaption = caption;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TEXT_CAPTION));
}

void ape::TextGeometryImpl::clearCaption()
{
	setCaption("");
}

bool ape::TextGeometryImpl::isVisible()
{
	return mVisibility;
}

void ape::TextGeometryImpl::setVisible( bool enabled )
{
	mVisibility = enabled;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TEXT_VISIBLE));
}

void ape::TextGeometryImpl::showOnTop(bool show)
{
	mShowOnTop = show;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TEXT_SHOWONTOP));
}

bool ape::TextGeometryImpl::isShownOnTop()
{
	return mShowOnTop;
}

void ape::TextGeometryImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TEXT_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

void ape::TextGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::TextGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mCaption.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mVisibility);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::TextGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString caption;
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, caption))
	{
		mCaption = caption.C_String();
		//APE_LOG_DEBUG("mCaption: " << mCaption << " from: " << deserializeParameters->sourceConnection->GetRakNetGUID().ToString());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TEXT_CAPTION));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		if (auto parentNode = mpSceneManager->getNode(parentName.C_String()).lock())
		{
			mParentNode = parentNode;
			mParentNodeName = parentName.C_String();
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TEXT_PARENTNODE));
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mVisibility))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_TEXT_VISIBLE));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
