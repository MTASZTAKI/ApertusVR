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

#include "apeBrowserImpl.h"

ape::BrowserImpl::BrowserImpl(std::string name, bool replicate, std::string ownerID, bool isHost) : ape::IBrowser(name, replicate, ownerID), ape::Replica("Browser", name, ownerID, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mGeometry = ape::GeometryWeakPtr();
	mURL = std::string();
	mResoultion = ape::Vector2();
	mGeometryName = std::string();
	mZoomLevel = 0;
	mZOrder = 0;
	mMouseLastClick = ape::Browser::MouseClick::UNKNOWN;
	mMouseLastClickIsDown = false;
	mMouseScrollDelta = ape::Vector2();
	mMouseLastPosition = ape::Vector2();
	mID = 0;
	mLastKeyValue = 0;
	mIsFocusOnEditableField = false;
	mReloadDeltaTrigger = false;
}

ape::BrowserImpl::~BrowserImpl()
{
	
}

void ape::BrowserImpl::setURL(std::string url)
{
	mURL = url;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_URL));
}

std::string ape::BrowserImpl::getURL()
{
	return mURL;
}

void ape::BrowserImpl::setResoultion(float vertical, float horizontal)
{
	mResoultion.x = vertical;
	mResoultion.y = horizontal;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_RESOLUTION));
}

ape::Vector2 ape::BrowserImpl::getResoultion()
{
	return mResoultion;
}

void ape::BrowserImpl::setGeometry(ape::GeometryWeakPtr geometry)
{
	if (auto geometrySP = geometry.lock())
	{
		mGeometry = geometry;
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_GEOMETRY));
	}
}

ape::GeometryWeakPtr ape::BrowserImpl::getGeometry()
{
	return mGeometry;
}

void ape::BrowserImpl::showOnOverlay(bool enable, int zOrder)
{
	if (enable)
	{
		mZOrder = zOrder;
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_OVERLAY));
	}
}

void ape::BrowserImpl::setZoomLevel(int level)
{
	mZoomLevel = level;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_ZOOM));
}

int ape::BrowserImpl::getZoomLevel()
{
	return mZoomLevel;
}

int ape::BrowserImpl::getZOrder()
{
	return mZOrder;
}

void ape::BrowserImpl::mouseClick(ape::Browser::MouseClick click, bool isClickDown)
{
	mMouseLastClick = click;
	mMouseLastClickIsDown = isClickDown;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_MOUSE_CLICK));
}

void ape::BrowserImpl::mouseMoved(ape::Vector2 position)
{
	mMouseLastPosition = position;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_MOUSE_MOVED));
}

void ape::BrowserImpl::mouseScroll(ape::Vector2 delta)
{
	mMouseScrollDelta = delta;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_MOUSE_SCROLL));
}

ape::Browser::MouseState ape::BrowserImpl::getMouseState()
{
	return ape::Browser::MouseState(mMouseLastPosition, mMouseLastClick, mMouseLastClickIsDown, mMouseScrollDelta);
}

void ape::BrowserImpl::keyASCIIValue(int keyASCIIValue)
{
	mLastKeyValue = keyASCIIValue;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_KEY_VALUE));
}

int ape::BrowserImpl::getLastKeyASCIIValue()
{
	return mLastKeyValue;
}

bool ape::BrowserImpl::isFocusOnEditableField()
{
	return mIsFocusOnEditableField;
}

void ape::BrowserImpl::setFocusOnEditableField(bool enable)
{
	mIsFocusOnEditableField = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD));
}

void ape::BrowserImpl::reload()
{
	mReloadDeltaTrigger = !mReloadDeltaTrigger;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_RELOAD));
}

void ape::BrowserImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::BrowserImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mResoultion);
	//mVariableDeltaSerializer.SerializeVariable(&serializationContext, mZoomLevel);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mURL.c_str()));
	/*mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseLastPosition);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseLastClickIsDown);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseLastClick);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMouseScrollDelta);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mLastKeyValue);*/
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mGeometryName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mReloadDeltaTrigger);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::BrowserImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mResoultion))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_RESOLUTION));
	/*if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mZoomLevel))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_ZOOM));*/
	RakNet::RakString url;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, url))
	{
		mURL = url.C_String();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_URL));
	}
	/*if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseLastPosition))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_MOUSE_MOVED));
	mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseLastClickIsDown);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseLastClick))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_MOUSE_CLICK));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMouseScrollDelta))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_MOUSE_SCROLL));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mLastKeyValue))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_KEY_VALUE));*/
	RakNet::RakString geometryName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, geometryName))
	{
		mGeometryName = geometryName.C_String();
		mGeometry = std::static_pointer_cast<ape::Geometry>(mpSceneManager->getEntity(mGeometryName).lock());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_GEOMETRY));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mReloadDeltaTrigger))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::BROWSER_RELOAD));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
