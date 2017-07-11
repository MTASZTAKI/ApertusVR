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

#include <iostream>
#include "ApeManualTextureImpl.h"

Ape::ManualTextureImpl::ManualTextureImpl(std::string name, bool isHostCreated) : Ape::IManualTexture(name), Ape::Replica("ManualTexture", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mParameters = Ape::ManualTextureParameters();
	mCameraName = std::string();
	mCamera = Ape::CameraWeakPtr();
	mpBuffer = nullptr;
}

Ape::ManualTextureImpl::~ManualTextureImpl()
{
	
}

void Ape::ManualTextureImpl::setParameters(float width, float height)
{
	mParameters.width = width;
	mParameters.height = height;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_MANUAL_PARAMETERS));
}

Ape::ManualTextureParameters Ape::ManualTextureImpl::getParameters()
{
	return mParameters;
}

void Ape::ManualTextureImpl::setSourceCamera(Ape::CameraWeakPtr camera)
{
	if (auto cameraSP = camera.lock())
	{
		mCamera = camera;
		mCameraName = cameraSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA));
	}
	else
		mCamera = Ape::CameraWeakPtr();
}

Ape::CameraWeakPtr Ape::ManualTextureImpl::getSourceCamera()
{
	return mCamera;
}

void Ape::ManualTextureImpl::setBuffer(const void* buffer)
{
	mpBuffer = buffer;
	std::cout << mpBuffer << std::endl;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::TEXTURE_MANUAL_BUFFER));
}

const void* Ape::ManualTextureImpl::getBuffer()
{
	return mpBuffer;
}

void Ape::ManualTextureImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::ManualTextureImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mCameraName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::ManualTextureImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters))
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS));
	RakNet::RakString cameraName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, cameraName))
	{
		mCameraName = cameraName.C_String();
		mCamera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(mCameraName).lock());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::MATERIAL_MANUAL_PASS));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


