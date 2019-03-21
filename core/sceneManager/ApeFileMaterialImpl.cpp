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

#include "ApeFileMaterialImpl.h"

ape::FileMaterialImpl::FileMaterialImpl(std::string name, bool isHostCreated) : ape::IFileMaterial(name), ape::Replica("FileMaterial", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mFileName = std::string();
	mIsSkyBox = false;
	mTexture = ape::TextureWeakPtr();
	mTextureName = std::string();
	mPassGpuParameters = ape::PassGpuParameters();
}

ape::FileMaterialImpl::~FileMaterialImpl()
{
	
}

std::string ape::FileMaterialImpl::getfFileName()
{
	return mFileName;
}

void ape::FileMaterialImpl::setFileName(std::string fileName)
{
	mFileName = fileName;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_FILE_FILENAME));
}

void ape::FileMaterialImpl::setAsSkyBox()
{
	mIsSkyBox = true;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_FILE_SETASSKYBOX));
}

void ape::FileMaterialImpl::setPassTexture(ape::TextureWeakPtr texture)
{
	if (auto textureSP = texture.lock())
	{
		mTexture = texture;
		mTextureName = textureSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_FILE_TEXTURE));
	}
	else
		mTexture = ape::TextureWeakPtr();
}

void ape::FileMaterialImpl::setPassGpuParameters(ape::PassGpuParameters passGpuParameters)
{
	mPassGpuParameters = passGpuParameters;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_FILE_GPUPARAMETERS));
}

ape::TextureWeakPtr ape::FileMaterialImpl::getPassTexture()
{
	return mTexture;
}

ape::PassGpuParameters ape::FileMaterialImpl::getPassGpuParameters()
{
	return mPassGpuParameters;
}

void ape::FileMaterialImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::FileMaterialImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mFileName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsSkyBox);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mTextureName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::FileMaterialImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString fileName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, fileName))
	{
		mFileName = fileName.C_String();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_FILE_FILENAME));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsSkyBox))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_FILE_SETASSKYBOX));
	RakNet::RakString textureName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, textureName))
	{
		mTextureName = textureName.C_String();
		mTexture = std::static_pointer_cast<ape::Texture>(mpSceneManager->getEntity(mTextureName).lock());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::MATERIAL_FILE_TEXTURE));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


