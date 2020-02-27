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

#include "apeFileTextureImpl.h"

ape::FileTextureImpl::FileTextureImpl(std::string name, bool replicate, bool isHost) : ape::IFileTexture(name, replicate), ape::Replica("FileTexture", name, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpScene = ape::ISceneManager::getSingletonPtr();
	mFileName = std::string();
}

ape::FileTextureImpl::~FileTextureImpl()
{
	
}

std::string ape::FileTextureImpl::getFileName()
{
	return mFileName;
}

void ape::FileTextureImpl::setFileName(std::string fileName)
{
	mFileName = fileName;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_FILE_FILENAME));
}

void ape::FileTextureImpl::setMapType(MapType mapType)
{
	mMapType = mapType;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_FILE_TYPE));
}

ape::IFileTexture::MapType ape::FileTextureImpl::getMapType()
{
	return mMapType;
}


void ape::FileTextureImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::FileTextureImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mFileName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mMapType);
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::FileTextureImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString fileName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, fileName))
	{
		mFileName = fileName.C_String();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_FILE_FILENAME));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mMapType))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_FILE_TYPE));
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


