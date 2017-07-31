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

#include "ApeBrowserImpl.h"

Ape::BrowserImpl::BrowserImpl(std::string name, bool isHostCreated) : Ape::IBrowser(name), Ape::Replica("Browser", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mGeometry = Ape::GeometryWeakPtr();
	mURL = std::string();
	mResoultion = Ape::Vector2();
	mGeometryName = std::string();
}

Ape::BrowserImpl::~BrowserImpl()
{
	
}

void Ape::BrowserImpl::setURL(std::string url)
{
	mURL = url;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_URL));
}

std::string Ape::BrowserImpl::getURL()
{
	return mURL;
}

void Ape::BrowserImpl::setResoultion(float vertical, float horizontal)
{
	mResoultion.x = vertical;
	mResoultion.y = horizontal;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_RESOLUTION));
}

Ape::Vector2 Ape::BrowserImpl::getResoultion()
{
	return mResoultion;
}

void Ape::BrowserImpl::setGeometry(Ape::GeometryWeakPtr geometry)
{
	if (auto geometrySP = geometry.lock())
	{
		mGeometry = geometry;
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_GEOMETRY));
	}
}

Ape::GeometryWeakPtr Ape::BrowserImpl::getGeometry()
{
	return mGeometry;
}


void Ape::BrowserImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::BrowserImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mURL);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mResoultion);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mGeometryName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::BrowserImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString url;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, url))
	{
		mURL = url.C_String();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_URL));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mResoultion))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_RESOLUTION));
	RakNet::RakString geometryName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, geometryName))
	{
		mGeometryName = geometryName.C_String();
		mGeometry = std::static_pointer_cast<Ape::Geometry>(mpScene->getEntity(mGeometryName).lock());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_GEOMETRY));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
