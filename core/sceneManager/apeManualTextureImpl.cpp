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

#include "apeManualTextureImpl.h"

ape::ManualTextureImpl::ManualTextureImpl(std::string name, bool isHostCreated) : ape::IManualTexture(name), ape::Replica("ManualTexture", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mParameters = ape::ManualTextureParameters();
	mCameraName = std::string();
	mCamera = ape::CameraWeakPtr();
	mpBuffer = nullptr;
	mpGraphicsApiID = nullptr;
}

ape::ManualTextureImpl::~ManualTextureImpl()
{
	
}

void ape::ManualTextureImpl::setParameters(unsigned int width, unsigned int height, ape::Texture::PixelFormat pixelFormat, ape::Texture::Usage usage, bool gammaCorrection, bool useFsaa)
{
	mParameters.width = width;
	mParameters.height = height;
	mParameters.pixelFormat = pixelFormat;
	mParameters.usage = usage;
	mParameters.gammaCorrection = gammaCorrection;
	mParameters.useFsaa = useFsaa;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_MANUAL_PARAMETERS));
}

ape::ManualTextureParameters ape::ManualTextureImpl::getParameters()
{
	return mParameters;
}

void ape::ManualTextureImpl::setSourceCamera(ape::CameraWeakPtr camera)
{
	if (auto cameraSP = camera.lock())
	{
		mCamera = camera;
		mCameraName = cameraSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA));
	}
	else
		mCamera = ape::CameraWeakPtr();
}

ape::CameraWeakPtr ape::ManualTextureImpl::getSourceCamera()
{
	return mCamera;
}

void ape::ManualTextureImpl::setGraphicsApiID(void * id)
{
	mpGraphicsApiID = id;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_MANUAL_GRAPHICSAPIID));
}

void * ape::ManualTextureImpl::getGraphicsApiID()
{
	return mpGraphicsApiID;
}

void ape::ManualTextureImpl::setBuffer(const void* buffer)
{
	mpBuffer = buffer;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_MANUAL_BUFFER));
}

const void* ape::ManualTextureImpl::getBuffer()
{
	return mpBuffer;
}

void ape::ManualTextureImpl::registerFunction(std::function<void()> callback)
{
	mFunctions.push_back(callback);
}

std::vector<std::function<void()>> ape::ManualTextureImpl::getFunctionList()
{
	return mFunctions;
}

void ape::ManualTextureImpl::unRegisterFunction(std::function<void()> callback)
{
	for (auto it = mFunctions.begin(); it != mFunctions.end();)
	{
		if ((*it).target_type() == callback.target_type())
			it = mFunctions.erase(it);
		else
			++it;
	}
}

void ape::ManualTextureImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::ManualTextureImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mCameraName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::ManualTextureImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS));
	RakNet::RakString cameraName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, cameraName))
	{
		mCameraName = cameraName.C_String();
		mCamera = std::static_pointer_cast<ape::ICamera>(mpSceneManager->getEntity(mCameraName).lock());
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}


