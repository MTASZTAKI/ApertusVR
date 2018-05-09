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

#include "ApeBrowserImpl.h"

Ape::BrowserImpl::BrowserImpl(std::string name, bool isHostCreated) : Ape::IBrowser(name), Ape::Replica("Browser", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mGeometry = Ape::GeometryWeakPtr();
	mURL = std::string();
	mResoultion = Ape::Vector2();
	mGeometryName = std::string();
	mZoomLevel = 0;
	mZOrder = 0;
	mMouseLastClick = Ape::Browser::MouseClick::UNKNOWN;
	mMouseLastClickIsDown = false;
	mMouseScrollDelta = Ape::Vector2();
	mMouseLastPosition = Ape::Vector2();
	mID = 0;
	mLastKeyValue = 0;
	mIsFocusOnEditableField = false;
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

void Ape::BrowserImpl::showOnOverlay(bool enable, int zOrder)
{
	if (enable)
	{
		mZOrder = zOrder;
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_OVERLAY));
	}
}

void Ape::BrowserImpl::setZoomLevel(int level)
{
	mZoomLevel = level;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_ZOOM));
}

int Ape::BrowserImpl::getZoomLevel()
{
	return mZoomLevel;
}

int Ape::BrowserImpl::getZOrder()
{
	return mZOrder;
}

void Ape::BrowserImpl::mouseClick(Ape::Browser::MouseClick click, bool isClickDown)
{
	mMouseLastClick = click;
	mMouseLastClickIsDown = isClickDown;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_MOUSE_CLICK));
}

void Ape::BrowserImpl::mouseMoved(Ape::Vector2 position)
{
	mMouseLastPosition = position;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_MOUSE_MOVED));
}

void Ape::BrowserImpl::mouseScroll(Ape::Vector2 delta)
{
	mMouseScrollDelta = delta;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_MOUSE_SCROLL));
}

Ape::Browser::MouseState Ape::BrowserImpl::getMouseState()
{
	return Ape::Browser::MouseState(mMouseLastPosition, mMouseLastClick, mMouseLastClickIsDown, mMouseScrollDelta);
}

void Ape::BrowserImpl::keyASCIIValue(int keyASCIIValue)
{
	mLastKeyValue = keyASCIIValue;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_KEY_VALUE));
}

int Ape::BrowserImpl::getLastKeyASCIIValue()
{
	return mLastKeyValue;
}

bool Ape::BrowserImpl::isFocusOnEditableField()
{
	return mIsFocusOnEditableField;
}

void Ape::BrowserImpl::setFocusOnEditableField(bool enable)
{
	mIsFocusOnEditableField = enable;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD));
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
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mZoomLevel);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseLastPosition);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseLastClickIsDown);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseLastClick);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseScrollDelta);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLastKeyValue);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mGeometryName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
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
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mZoomLevel))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_ZOOM));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseLastPosition))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_MOUSE_MOVED));
	mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseLastClickIsDown);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseLastClick))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_MOUSE_CLICK));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseScrollDelta))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_MOUSE_SCROLL));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLastKeyValue))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_KEY_VALUE));
	RakNet::RakString geometryName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, geometryName))
	{
		mGeometryName = geometryName.C_String();
		mGeometry = std::static_pointer_cast<Ape::Geometry>(mpScene->getEntity(mGeometryName).lock());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::BROWSER_GEOMETRY));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
