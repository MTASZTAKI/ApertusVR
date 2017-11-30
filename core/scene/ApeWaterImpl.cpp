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

#include "ApeWaterImpl.h"

Ape::WaterImpl::WaterImpl(std::string name, bool isHostCreated) : Ape::IWater(name), Ape::Replica("Water", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mSky = Ape::SkyWeakPtr();
	mSkyName = std::string();
	mCameras = std::vector<Ape::CameraWeakPtr>();
	mCamerasName = std::vector<std::string>();
}

Ape::WaterImpl::~WaterImpl()
{

}

void Ape::WaterImpl::setSky(Ape::SkyWeakPtr sky)
{
	if (auto skySP = sky.lock())
	{
		mSky = sky;
		mSkyName = skySP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::WATER_SKY));
	}
	else
		mSky = Ape::SkyWeakPtr();
}

Ape::SkyWeakPtr Ape::WaterImpl::getSky()
{
	return mSky;
}

void Ape::WaterImpl::setCameras(std::vector<Ape::CameraWeakPtr> cameras)
{
	for (auto camera : cameras)
	{
		if (auto cameraSP = camera.lock())
		{
			mCameras.push_back(camera);
			mCamerasName.push_back(cameraSP->getName());
		}
		else
		{
			mCameras.push_back(Ape::CameraWeakPtr());
			mCamerasName.push_back(std::string());
		}
	}
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::WATER_CAMERAS));
}

std::vector<Ape::CameraWeakPtr> Ape::WaterImpl::getCameras()
{
	return mCameras;
}

void Ape::WaterImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::WaterImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mCamerasName);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mSkyName.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Ape::WaterImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mCamerasName))
	{
		for (auto cameraName : mCamerasName)
		{
			if (auto camera = std::static_pointer_cast<Ape::ICamera>(mpScene->getEntity(cameraName).lock()))
			{
				mCameras.push_back(camera);
				mCamerasName.push_back(camera->getName());
			}
			else
			{
				mCameras.push_back(Ape::CameraWeakPtr());
				mCamerasName.push_back(std::string());
			}
		}
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::WATER_CAMERAS));
	}
	RakNet::RakString skyName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, skyName))
	{
		mSkyName = skyName.C_String();
		mSky = std::static_pointer_cast<Ape::ISky>(mpScene->getEntity(mSkyName).lock());
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::WATER_SKY));
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
