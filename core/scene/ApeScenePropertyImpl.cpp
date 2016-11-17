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

#include "ApeScenePropertyImpl.h"

Ape::ScenePropertyImpl::ScenePropertyImpl(std::string name, bool isHostCreated) : Ape::Replica("SceneProperty", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mName = name;
	mAmbientColor = Ape::Color();
	mResourceFolders = Ape::ResourceFolders();
	mSkyBoxMaterialName = std::string();
}

Ape::ScenePropertyImpl::~ScenePropertyImpl()
{
	
}

void Ape::ScenePropertyImpl::addResourceLocation(std::string resourceFolder, std::string resourceGroupName)
{
	mResourceFolders.push_back(resourceFolder);
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SCENEPROPERTY_ADDRESOURCEFOLDER));
}

void Ape::ScenePropertyImpl::setAmbientColor(Ape::Color ambient) 
{
	mAmbientColor = ambient;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SCENEPROPERTY_AMBIENTCOLOR));
}

void Ape::ScenePropertyImpl::setSkyBoxMaterialName(std::string materialName)
{
	mSkyBoxMaterialName = materialName;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SCENEPROPERTY_SKYBOXMATERIAL));
}

Ape::ResourceFolders Ape::ScenePropertyImpl::getResourceFolders()
{
	return mResourceFolders;
}

Ape::Color Ape::ScenePropertyImpl::getAmbientColor() 
{
	return mAmbientColor;
}

std::string Ape::ScenePropertyImpl::getSkyBoxMaterialName()
{
	return mSkyBoxMaterialName;
}

void Ape::ScenePropertyImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
}

RakNet::RM3SerializationResult Ape::ScenePropertyImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mAmbientColor);
	for (auto resourceFolder : mResourceFolders)
		mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(resourceFolder.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mSkyBoxMaterialName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::ScenePropertyImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	RakNet::RakString skyBoxMaterialName;
	RakNet::RakString resourceFolder;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mAmbientColor))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SCENEPROPERTY_AMBIENTCOLOR));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, resourceFolder))
	{
		mResourceFolders.push_back(resourceFolder.C_String());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SCENEPROPERTY_ADDRESOURCEFOLDER));
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, skyBoxMaterialName))
	{
		mSkyBoxMaterialName = skyBoxMaterialName;
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::SCENEPROPERTY_SKYBOXMATERIAL));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
