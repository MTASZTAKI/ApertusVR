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


#define BIG_PACKET_SIZE 2457600
#define SPLIT_MSG_PROGRESS_INTERVAL 1000
char *text;

#include <iostream>
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
	mCurrentPointsSize = 0;
	mCurrentColorsSize = 0;
	mIsCurrentPointsChanged = false;
	mIsCurrentColorsChanged = false;
	mCurrentPoints = Ape::PointCloudPoints();
	mCurrentColors = Ape::PointCloudColors();
	text = new char[BIG_PACKET_SIZE];
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
	mIsCurrentPointsChanged = true;
	mCurrentPointsSize = static_cast<int>(points.size());
	mCurrentPoints = points;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_POINTS));
}

void Ape::PointCloudImpl::updateColors(Ape::PointCloudColors colors)
{
	mIsCurrentColorsChanged = true;
	mCurrentColorsSize = static_cast<int>(colors.size());
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

		return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
	}
	return RakNet::RM3SR_DO_NOT_SERIALIZE;
}

void Ape::PointCloudImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	if (deserializeParameters->bitstreamWrittenTo[0])
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
}

void Ape::PointCloudImpl::listenStreamPeerSendThread(RakNet::RakPeerInterface* streamPeer)
{
	while (true)
	{
		RakNet::Packet *packet;
		for (packet = streamPeer->Receive(); packet; streamPeer->DeallocatePacket(packet), packet = streamPeer->Receive())
		{
			if (packet->data[0] == ID_NEW_INCOMING_CONNECTION || packet->data[0] == 253)
			{
				for (int i = 0; i < 100; i++)
				{
					LOG(LOG_TYPE_DEBUG, "Try for starting send " << BIG_PACKET_SIZE << " bytes sized big packet to " << packet->systemAddress.ToString(true));
					if (BIG_PACKET_SIZE <= SPLIT_MSG_PROGRESS_INTERVAL)
					{
						for (int i = 0; i < BIG_PACKET_SIZE; i++)
							text[i] = 255 - (i & 255);
					}
					else
						text[0] = (unsigned char)255;
					streamPeer->Send(text, BIG_PACKET_SIZE, IMMEDIATE_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				}
			}
			if (packet->data[0] == ID_CONNECTION_LOST)
			{
				LOG(LOG_TYPE_DEBUG, "ID_CONNECTION_LOST from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_DISCONNECTION_NOTIFICATION)
			{
				LOG(LOG_TYPE_DEBUG, "ID_DISCONNECTION_NOTIFICATION from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_NEW_INCOMING_CONNECTION)
			{
				LOG(LOG_TYPE_DEBUG, "ID_NEW_INCOMING_CONNECTION from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
			{
				LOG(LOG_TYPE_DEBUG, "ID_CONNECTION_REQUEST_ACCEPTED from " << packet->systemAddress.ToString());
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	//while (true)
	//{
	//	RakNet::BitStream bitStream;
	//	int msgCount = 1000;
	//	int msgSize = mPointsSize * mColorsSize * 4;
	//	for (int i = 0; i < msgCount; i++)
	//	{
	//		bitStream.Reset();
	//		bitStream.Write((RakNet::MessageID)ID_USER_PACKET_ENUM);
	//		bitStream.Write(msgSize);
	//		bitStream.Write(i);
	//		bitStream.Write(msgCount);
	//		bitStream.PadWithZeroToByteLength(msgSize);
	//		LOG(LOG_TYPE_DEBUG, "Try to send burst data ");
	//		streamPeer->Send(&bitStream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	//		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	//	}
	//}
}

void Ape::PointCloudImpl::listenStreamPeerReceiveThread(RakNet::RakPeerInterface* streamPeer)
{
	while (true)
	{
		RakNet::Packet *packet;
		for (packet = streamPeer->Receive(); packet; streamPeer->DeallocatePacket(packet), packet = streamPeer->Receive())
		{
			if (packet->data[0] == ID_DOWNLOAD_PROGRESS)
			{
				RakNet::BitStream progressBS(packet->data, packet->length, false);
				progressBS.IgnoreBits(8);
				unsigned int progress;
				unsigned int total;
				unsigned int partLength;

				progressBS.ReadBits((unsigned char*)&progress, BYTES_TO_BITS(sizeof(progress)), true);
				progressBS.ReadBits((unsigned char*)&total, BYTES_TO_BITS(sizeof(total)), true);
				progressBS.ReadBits((unsigned char*)&partLength, BYTES_TO_BITS(sizeof(partLength)), true);

				LOG(LOG_TYPE_DEBUG, "Progress: msgID=" << (unsigned char)packet->data[0] << " Progress " << progress << " " << total << " Partsize=" << partLength);
			}
			else if (packet->data[0] == 255)
			{
				if (packet->length != BIG_PACKET_SIZE)
				{
					LOG(LOG_TYPE_DEBUG, "Test failed. %i bytes (wrong number of bytes)." << packet->length);
					break;
				}
				if (BIG_PACKET_SIZE <= SPLIT_MSG_PROGRESS_INTERVAL)
				{
					for (int i = 0; i < BIG_PACKET_SIZE; i++)
					{
						if (packet->data[i] != 255 - (i & 255))
						{
							LOG(LOG_TYPE_DEBUG, "Test failed. %i bytes (bad data)." << packet->length);
							break;
						}
					}
				}
			}
			else if (packet->data[0] == 254)
			{
				LOG(LOG_TYPE_DEBUG, "Got high priority message");
			}
			else if (packet->data[0] == ID_CONNECTION_LOST)
			{
				LOG(LOG_TYPE_DEBUG, "ID_CONNECTION_LOST from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_DISCONNECTION_NOTIFICATION)
			{
				LOG(LOG_TYPE_DEBUG, "ID_DISCONNECTION_NOTIFICATION from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_NEW_INCOMING_CONNECTION)
			{
				LOG(LOG_TYPE_DEBUG, "ID_NEW_INCOMING_CONNECTION from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
			{
				LOG(LOG_TYPE_DEBUG, "ID_CONNECTION_REQUEST_ACCEPTED from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_CONNECTION_ATTEMPT_FAILED)
			{
				LOG(LOG_TYPE_DEBUG, "ID_CONNECTION_ATTEMPT_FAILED from " << packet->systemAddress.ToString());
			}
			//if (packet->data[0] == ID_USER_PACKET_ENUM)
			//{
			//	uint32_t msgSize, msgCount, i;
			//	RakNet::BitStream bitStream(packet->data, packet->length, false);
			//	bitStream.IgnoreBytes(sizeof(RakNet::MessageID));
			//	bitStream.Read(msgSize);
			//	bitStream.Read(i);
			//	bitStream.Read(msgCount);
			//	LOG(LOG_TYPE_DEBUG, "Burst data is received from: " << packet->guid.ToString() << " size: " << packet->length << " index: " << i);
			//	//printf("%i/%i len=%i", index + 1, msgCount, packet->length);
			//	if (msgSize > BITS_TO_BYTES(bitStream.GetReadOffset()) && packet->length != msgSize)
			//	{
			//		LOG(LOG_TYPE_DEBUG, "UNDERLENGTH");
			//	}
			//	break;
			//}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
