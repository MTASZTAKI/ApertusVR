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

#include <iostream>
#include "ApePointCloudImpl.h"

Ape::PointCloudImpl::PointCloudImpl(std::string name, bool isHostCreated) : Ape::IPointCloud(name), Ape::Replica("PointCloud", isHostCreated)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
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
	mStreamHeaderSizeInBytes = 9; // 9 means 1byte(char) for packetID and 8 more(2 integer) for point cloud size
}

Ape::PointCloudImpl::~PointCloudImpl()
{

}

void Ape::PointCloudImpl::setParameters(Ape::PointCloudPoints points, Ape::PointCloudColors colors, float boundigSphereRadius, float pointScaleOffset, float unitScaleDistance)
{
	mPointsSize = static_cast<int>(points.size());
	mColorsSize = static_cast<int>(colors.size());
	mParameters = Ape::PointCloudSetParameters(points, colors, boundigSphereRadius, pointScaleOffset, unitScaleDistance);
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
		mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mParameters.boundigSphereRadius);
		RakNet::RakString parentNodeName;
		if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentNodeName))
		{
			if (auto parentNode = mpSceneManager->getNode(parentNodeName.C_String()).lock())
			{
				mParentNode = parentNode;
				mParentNodeName = parentNodeName.C_String();
			}
		}
		mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
	}
}

void Ape::PointCloudImpl::sendStreamPacket(RakNet::RakPeerInterface* streamPeer, RakNet::Packet * packet)
{
	int streamPacketSizeInBytes = (mCurrentPointsSize * sizeof(short)) + (mCurrentColorsSize * sizeof(char)) + mStreamHeaderSizeInBytes;
	char* streamPacket = new char[streamPacketSizeInBytes];
	//APE_LOG_DEBUG("Try for starting send " << streamPacketSizeInBytes << " bytes sized big packet to " << packet->systemAddress.ToString(true) <<
	//" mCurrentPointsSize " << mCurrentPointsSize << " mCurrentColorsSize " << mCurrentColorsSize);
	streamPacket[0] = (unsigned char)255;

	dataUnionBytesInt myUnion;
	myUnion.i = mCurrentPointsSize;
	streamPacket[1] = myUnion.iBuff[0];
	streamPacket[2] = myUnion.iBuff[1];
	streamPacket[3] = myUnion.iBuff[2];
	streamPacket[4] = myUnion.iBuff[3];
	myUnion.i = mCurrentColorsSize;
	streamPacket[5] = myUnion.iBuff[0];
	streamPacket[6] = myUnion.iBuff[1];
	streamPacket[7] = myUnion.iBuff[2];
	streamPacket[8] = myUnion.iBuff[3];

	int packetDataIndex = mStreamHeaderSizeInBytes;
	for (auto item : mCurrentPoints)
	{
		dataUnionBytesShort myUnion;
		myUnion.s = item;
		for (int i = 0; i < sizeof(short); i++)
		{
			streamPacket[packetDataIndex] = myUnion.sBuff[i];
			packetDataIndex++;
		}
	}
	for (auto item : mCurrentColors)
	{
		streamPacket[packetDataIndex] = (short)(item * 255.0f);
		packetDataIndex++;
	}
	streamPeer->Send(streamPacket, streamPacketSizeInBytes, HIGH_PRIORITY, RELIABLE_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);
	delete[] streamPacket;
	streamPacket = nullptr;
}

void Ape::PointCloudImpl::sendInitStreamPacket(RakNet::RakPeerInterface * streamPeer, RakNet::Packet * packet)
{
	int streamPacketSizeInBytes = (mPointsSize * sizeof(short)) + (mColorsSize * sizeof(char)) + mStreamHeaderSizeInBytes;
	char* streamPacket = new char[streamPacketSizeInBytes];
	APE_LOG_DEBUG("Try for sending init stream packet " << streamPacketSizeInBytes << " bytes sized big packet to " << packet->systemAddress.ToString(true) <<
		" mPointsSize " << mPointsSize << " mColorsSize " << mColorsSize);
	streamPacket[0] = (unsigned char)254;

	dataUnionBytesInt myUnion;
	myUnion.i = mPointsSize;
	streamPacket[1] = myUnion.iBuff[0];
	streamPacket[2] = myUnion.iBuff[1];
	streamPacket[3] = myUnion.iBuff[2];
	streamPacket[4] = myUnion.iBuff[3];
	myUnion.i = mColorsSize;
	streamPacket[5] = myUnion.iBuff[0];
	streamPacket[6] = myUnion.iBuff[1];
	streamPacket[7] = myUnion.iBuff[2];
	streamPacket[8] = myUnion.iBuff[3];

	int packetDataIndex = mStreamHeaderSizeInBytes;
	for (auto item : mParameters.points)
	{
		dataUnionBytesShort myUnion;
		myUnion.s = item;
		for (int i = 0; i < sizeof(short); i++)
		{
			streamPacket[packetDataIndex] = myUnion.sBuff[i];
			packetDataIndex++;
		}
	}
	for (auto item : mParameters.colors)
	{
		streamPacket[packetDataIndex] = (short)(item * 255.0f);
		packetDataIndex++;
	}
	streamPeer->Send(streamPacket, streamPacketSizeInBytes, HIGH_PRIORITY, RELIABLE_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);
	delete[] streamPacket;
	streamPacket = nullptr;
}

void Ape::PointCloudImpl::listenStreamPeerSendThread(RakNet::RakPeerInterface* streamPeer)
{
	while (true)
	{
		RakNet::Packet* packet;
		for (packet = streamPeer->Receive(); packet; streamPeer->DeallocatePacket(packet), packet = streamPeer->Receive())
		{
			if (packet->data[0] == ID_NEW_INCOMING_CONNECTION)
			{
				APE_LOG_DEBUG("ID_NEW_INCOMING_CONNECTION from " << packet->systemAddress.ToString());
				sendInitStreamPacket(streamPeer, packet);
			}
			else if (packet->data[0] == ID_CONNECTION_LOST)
			{
				APE_LOG_DEBUG("ID_CONNECTION_LOST from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_DISCONNECTION_NOTIFICATION)
			{
				APE_LOG_DEBUG("ID_DISCONNECTION_NOTIFICATION from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
			{
				APE_LOG_DEBUG("ID_CONNECTION_REQUEST_ACCEPTED from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_SND_RECEIPT_ACKED)
			{
				//APE_LOG_DEBUG("ID_SND_RECEIPT_ACKED from " << packet->systemAddress.ToString());
				sendStreamPacket(streamPeer, packet);
			}
			else
			{
				//APE_LOG_DEBUG("UNKNOWN MSG " << packet->data[0]);
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Ape::PointCloudImpl::listenStreamPeerReceiveThread(RakNet::RakPeerInterface* streamPeer)
{
	while (true)
	{
		RakNet::Packet* packet;
		for (packet = streamPeer->Receive(); packet; streamPeer->DeallocatePacket(packet), packet = streamPeer->Receive())
		{
			if (packet->data[0] == 254)
			{
				dataUnionBytesInt myUnion;
				myUnion.iBuff[0] = packet->data[1];
				myUnion.iBuff[1] = packet->data[2];
				myUnion.iBuff[2] = packet->data[3];
				myUnion.iBuff[3] = packet->data[4];
				mPointsSize = myUnion.i;
				myUnion.iBuff[0] = packet->data[5];
				myUnion.iBuff[1] = packet->data[6];
				myUnion.iBuff[2] = packet->data[7];
				myUnion.iBuff[3] = packet->data[8];
				mColorsSize = myUnion.i;
				mParameters.points.clear();
				mParameters.points.resize(mPointsSize);
				mParameters.colors.clear();
				mParameters.colors.resize(mColorsSize);
				int packetDataIndex = mStreamHeaderSizeInBytes;
				for (int i = 0; i < mPointsSize; i++)
				{
					dataUnionBytesShort myUnion;
					for (int j = 0; j<sizeof(short); j++)
					{
						myUnion.sBuff[j] = packet->data[packetDataIndex];
						mParameters.points[i] = myUnion.s;
						packetDataIndex++;
					}
				}
				for (int i = 0; i < mColorsSize; i++)
				{
					mParameters.colors[i] = ((short)packet->data[packetDataIndex]) / 255.0f;
					packetDataIndex++;
				}
				APE_LOG_DEBUG("Received init stream packed with size: " << packet->length << " packetDataIndex after read " << packetDataIndex);
				mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_PARAMETERS));
				mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_PARENTNODE));
			}
			else if (packet->data[0] == 255)
			{
				dataUnionBytesInt myUnion;
				myUnion.iBuff[0] = packet->data[1];
				myUnion.iBuff[1] = packet->data[2];
				myUnion.iBuff[2] = packet->data[3];
				myUnion.iBuff[3] = packet->data[4];
				mCurrentPointsSize = myUnion.i;
				myUnion.iBuff[0] = packet->data[5];
				myUnion.iBuff[1] = packet->data[6];
				myUnion.iBuff[2] = packet->data[7];
				myUnion.iBuff[3] = packet->data[8];
				mCurrentColorsSize = myUnion.i;
				mCurrentPoints.clear();
				mCurrentPoints.resize(mCurrentPointsSize);
				mCurrentColors.clear();
				mCurrentColors.resize(mCurrentColorsSize);
				int packetDataIndex = mStreamHeaderSizeInBytes;
				for (int i = 0; i < mCurrentPointsSize; i++)
				{
					dataUnionBytesShort myUnion;
					for (int j = 0; j<sizeof(short); j++)
					{
						myUnion.sBuff[j] = packet->data[packetDataIndex];
						mCurrentPoints[i] = myUnion.s;
						packetDataIndex++;
					}
				}
				mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_POINTS));
				for (int i = 0; i < mCurrentColorsSize; i++)
				{
					mCurrentColors[i] = ((short)packet->data[packetDataIndex]) / 255.0f;
					packetDataIndex++;
				}
				//APE_LOG_DEBUG("Received stream packed with size: " << packet->length << " packetDataIndex after read " << packetDataIndex);
				mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::POINT_CLOUD_COLORS));
			}
			else if (packet->data[0] == ID_CONNECTION_LOST)
			{
				APE_LOG_DEBUG("ID_CONNECTION_LOST from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_DISCONNECTION_NOTIFICATION)
			{
				APE_LOG_DEBUG("ID_DISCONNECTION_NOTIFICATION from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_NEW_INCOMING_CONNECTION)
			{
				APE_LOG_DEBUG("ID_NEW_INCOMING_CONNECTION from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
			{
				APE_LOG_DEBUG("ID_CONNECTION_REQUEST_ACCEPTED from " << packet->systemAddress.ToString());
			}
			else if (packet->data[0] == ID_CONNECTION_ATTEMPT_FAILED)
			{
				APE_LOG_DEBUG("ID_CONNECTION_ATTEMPT_FAILED from " << packet->systemAddress.ToString());
			}
			else
			{
				//APE_LOG_DEBUG("UNKNOWN MSG " << packet->data[0]);
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


