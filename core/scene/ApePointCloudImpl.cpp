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

#include "ApePointCloudImpl.h"

Ape::PointCloudImpl::PointCloudImpl(std::string name, bool isHostCreated) : Ape::IPointCloud(name), Ape::Replica("PointCloud", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mParentNode = Ape::NodeWeakPtr();
	mParentNodeName = std::string();
	mParameters = Ape::PointCloudSetParameters();
	mPointsSize = 0;
	mColorsSize = 0;
	mCurrentPoints = Ape::PointCloudPoints();
	mCurrentColors = Ape::PointCloudColors();
}

Ape::PointCloudImpl::~PointCloudImpl()
{

}

void Ape::PointCloudImpl::setParameters(Ape::PointCloudPoints points, Ape::PointCloudColors colors, float boundigSphereRadius)
{
	mPointsSize = static_cast<int>(points.size());
	mColorsSize = static_cast<int>(colors.size());
	mParameters = Ape::PointCloudSetParameters(points, colors, boundigSphereRadius);
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_PARAMETERS));
}

Ape::PointCloudSetParameters Ape::PointCloudImpl::getParameters()
{
	return mParameters;
}

void Ape::PointCloudImpl::updatePoints(Ape::PointCloudPoints points)
{
	mCurrentPoints = points;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_POINTS));
}

void Ape::PointCloudImpl::updateColors(Ape::PointCloudColors colors)
{
	mCurrentColors = colors;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_COLORS));
}

Ape::PointCloudPoints Ape::PointCloudImpl::getCurrentPoints()
{
	return mCurrentPoints;
}

Ape::PointCloudColors Ape::PointCloudImpl::getCurrentColors()
{
	return mCurrentColors;
}


void Ape::PointCloudImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNodeSP;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

Ape::NodeWeakPtr Ape::PointCloudImpl::getParentNode()
{
	return mParentNode;
}

void Ape::PointCloudImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult Ape::PointCloudImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	if (serializeParameters->whenLastSerialized == 0)
	{
		RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
		serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
		mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mPointsSize);
		for (auto item : mParameters.points)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mColorsSize);
		for (auto item : mParameters.colors)
			mVariableDeltaSerializer.SerializeVariable(&serializationContext, item);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, mParameters.boundigSphereRadius);

		mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
		
		mVariableDeltaSerializer.EndSerialize(&serializationContext);
		return RakNet::RM3SR_SERIALIZED_ALWAYS;
	}
	return RakNet::RM3SR_DO_NOT_SERIALIZE;
}

void Ape::PointCloudImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mPointsSize))
	{
		while (mParameters.points.size() < mPointsSize)
		{
			float item;
			if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, item))
				mParameters.points.push_back(item);
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mColorsSize))
	{
		while (mParameters.colors.size() < mColorsSize)
		{
			float item;
			if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, item))
				mParameters.colors.push_back(item);
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters.boundigSphereRadius))
	{
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_PARAMETERS));
	}
	RakNet::RakString parentNodeName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentNodeName))
	{
		if (auto parentNode = mpScene->getNode(parentNodeName.C_String()).lock())
		{
			mParentNode = parentNode;
			mParentNodeName = parentNodeName.C_String();
			mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_PARENTNODE));
		}
	}
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}
