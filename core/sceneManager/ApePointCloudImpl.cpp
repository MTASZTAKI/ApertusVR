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
#include "apePointCloudImpl.h"

ape::PointCloudImpl::PointCloudImpl(std::string name, bool isHostCreated) : ape::IPointCloud(name), ape::Replica("PointCloud", isHostCreated)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mParentNode = ape::NodeWeakPtr();
	mParentNodeName = std::string();
	mParameters = ape::PointCloudSetParameters();
	mPointsSize = 0;
	mColorsSize = 0;
	mCurrentPointsSize = 0;
	mCurrentColorsSize = 0;
	mIsCurrentPointsChanged = false;
	mIsCurrentColorsChanged = false;
	mCurrentPoints = ape::PointCloudPoints();
	mCurrentColors = ape::PointCloudColors();
	mStreamHeaderSizeInBytes = 33; // 33 means: 1byte(1 char) for packetID and 8bytes more(2 integers) for point cloud size + 24bytes more(6 floats) for the rest of the parameters
}

ape::PointCloudImpl::~PointCloudImpl()
{

}

void ape::PointCloudImpl::setParameters(ape::PointCloudPoints points, ape::PointCloudColors colors, float boundigSphereRadius, float pointSize, bool pointScale,
	float pointScaleOffset, float unitScaleDistance, float scaleFactor)
{
	mPointsSize = static_cast<int>(points.size());
	mColorsSize = static_cast<int>(colors.size());
	mParameters = ape::PointCloudSetParameters(points, colors, boundigSphereRadius, pointSize, pointScale, pointScaleOffset, unitScaleDistance, scaleFactor);
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::POINT_CLOUD_PARAMETERS));
}

ape::PointCloudSetParameters ape::PointCloudImpl::getParameters()
{
	return mParameters;
}

void ape::PointCloudImpl::updatePoints(ape::PointCloudPoints points)
{
	mIsCurrentPointsChanged = true;
	mCurrentPointsSize = static_cast<int>(points.size());
	mCurrentPoints = points;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::POINT_CLOUD_POINTS));
}

void ape::PointCloudImpl::updateColors(ape::PointCloudColors colors)
{
	mIsCurrentColorsChanged = true;
	mCurrentColorsSize = static_cast<int>(colors.size());
	mCurrentColors = colors;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::POINT_CLOUD_COLORS));
}

ape::PointCloudPoints ape::PointCloudImpl::getCurrentPoints()
{
	return mCurrentPoints;
}

ape::PointCloudColors ape::PointCloudImpl::getCurrentColors()
{
	return mCurrentColors;
}


void ape::PointCloudImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNodeSP;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::POINT_CLOUD_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

ape::NodeWeakPtr ape::PointCloudImpl::getParentNode()
{
	return mParentNode;
}

void ape::PointCloudImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str()));
}

RakNet::RM3SerializationResult ape::PointCloudImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
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

void ape::PointCloudImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
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

void ape::PointCloudImpl::sendStreamPacket(RakNet::RakPeerInterface* streamPeer, RakNet::Packet * packet)
{
	int streamPacketSizeInBytes = (mCurrentPointsSize * sizeof(short)) + (mCurrentColorsSize * sizeof(char)) + mStreamHeaderSizeInBytes;
	char* streamPacket = new char[streamPacketSizeInBytes];
	APE_LOG_DEBUG("Try for starting send " << streamPacketSizeInBytes << " bytes sized big packet to " << packet->systemAddress.ToString(true) << " mCurrentPointsSize " << mCurrentPointsSize << " mCurrentColorsSize " << mCurrentColorsSize);
	streamPacket[0] = (unsigned char)255;

	dataUnionBytesInt myIntUnion;
	myIntUnion.i = mCurrentPointsSize;
	streamPacket[1] = myIntUnion.iBuff[0];
	streamPacket[2] = myIntUnion.iBuff[1];
	streamPacket[3] = myIntUnion.iBuff[2];
	streamPacket[4] = myIntUnion.iBuff[3];
	myIntUnion.i = mCurrentColorsSize;
	streamPacket[5] = myIntUnion.iBuff[0];
	streamPacket[6] = myIntUnion.iBuff[1];
	streamPacket[7] = myIntUnion.iBuff[2];
	streamPacket[8] = myIntUnion.iBuff[3];

	dataUnionBytesFloat myFloatUnion;
	myFloatUnion.f = mParameters.boundigSphereRadius;
	streamPacket[9] = myFloatUnion.fBuff[0];
	streamPacket[10] = myFloatUnion.fBuff[1];
	streamPacket[11] = myFloatUnion.fBuff[2];
	streamPacket[12] = myFloatUnion.fBuff[3];
	myFloatUnion.f = mParameters.pointSize;
	streamPacket[13] = myFloatUnion.fBuff[0];
	streamPacket[14] = myFloatUnion.fBuff[1];
	streamPacket[15] = myFloatUnion.fBuff[2];
	streamPacket[16] = myFloatUnion.fBuff[3];
	myIntUnion.i = static_cast<int>(mParameters.pointScale);
	streamPacket[17] = myIntUnion.iBuff[0];
	streamPacket[18] = myIntUnion.iBuff[1];
	streamPacket[19] = myIntUnion.iBuff[2];
	streamPacket[20] = myIntUnion.iBuff[3];
	myFloatUnion.f = mParameters.pointScaleOffset;
	streamPacket[21] = myFloatUnion.fBuff[0];
	streamPacket[22] = myFloatUnion.fBuff[1];
	streamPacket[23] = myFloatUnion.fBuff[2];
	streamPacket[24] = myFloatUnion.fBuff[3];
	myFloatUnion.f = mParameters.unitScaleDistance;
	streamPacket[25] = myFloatUnion.fBuff[0];
	streamPacket[26] = myFloatUnion.fBuff[1];
	streamPacket[27] = myFloatUnion.fBuff[2];
	streamPacket[28] = myFloatUnion.fBuff[3];
	myFloatUnion.f = mParameters.scaleFactor;
	streamPacket[29] = myFloatUnion.fBuff[0];
	streamPacket[30] = myFloatUnion.fBuff[1];
	streamPacket[31] = myFloatUnion.fBuff[2];
	streamPacket[32] = myFloatUnion.fBuff[3];

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

void ape::PointCloudImpl::sendInitStreamPacket(RakNet::RakPeerInterface * streamPeer, RakNet::Packet * packet)
{
	int streamPacketSizeInBytes = (mPointsSize * sizeof(short)) + (mColorsSize * sizeof(char)) + mStreamHeaderSizeInBytes;
	char* streamPacket = new char[streamPacketSizeInBytes];
	APE_LOG_DEBUG("Try for sending init stream packet " << streamPacketSizeInBytes << " bytes sized big packet to " << packet->systemAddress.ToString(true) <<
		" mPointsSize " << mPointsSize << " mColorsSize " << mColorsSize);
	streamPacket[0] = (unsigned char)254;

	dataUnionBytesInt myIntUnion;
	myIntUnion.i = mPointsSize;
	streamPacket[1] = myIntUnion.iBuff[0];
	streamPacket[2] = myIntUnion.iBuff[1];
	streamPacket[3] = myIntUnion.iBuff[2];
	streamPacket[4] = myIntUnion.iBuff[3];
	myIntUnion.i = mColorsSize;
	streamPacket[5] = myIntUnion.iBuff[0];
	streamPacket[6] = myIntUnion.iBuff[1];
	streamPacket[7] = myIntUnion.iBuff[2];
	streamPacket[8] = myIntUnion.iBuff[3];

	dataUnionBytesFloat myFloatUnion;
	myFloatUnion.f = mParameters.boundigSphereRadius;
	streamPacket[9] = myFloatUnion.fBuff[0];
	streamPacket[10] = myFloatUnion.fBuff[1];
	streamPacket[11] = myFloatUnion.fBuff[2];
	streamPacket[12] = myFloatUnion.fBuff[3];
	myFloatUnion.f = mParameters.pointSize;
	streamPacket[13] = myFloatUnion.fBuff[0];
	streamPacket[14] = myFloatUnion.fBuff[1];
	streamPacket[15] = myFloatUnion.fBuff[2];
	streamPacket[16] = myFloatUnion.fBuff[3];
	myIntUnion.i = static_cast<int>(mParameters.pointScale);
	streamPacket[17] = myIntUnion.iBuff[0];
	streamPacket[18] = myIntUnion.iBuff[1];
	streamPacket[19] = myIntUnion.iBuff[2];
	streamPacket[20] = myIntUnion.iBuff[3];
	myFloatUnion.f = mParameters.pointScaleOffset;
	streamPacket[21] = myFloatUnion.fBuff[0];
	streamPacket[22] = myFloatUnion.fBuff[1];
	streamPacket[23] = myFloatUnion.fBuff[2];
	streamPacket[24] = myFloatUnion.fBuff[3];
	myFloatUnion.f = mParameters.unitScaleDistance;
	streamPacket[25] = myFloatUnion.fBuff[0];
	streamPacket[26] = myFloatUnion.fBuff[1];
	streamPacket[27] = myFloatUnion.fBuff[2];
	streamPacket[28] = myFloatUnion.fBuff[3];
	myFloatUnion.f = mParameters.scaleFactor;
	streamPacket[29] = myFloatUnion.fBuff[0];
	streamPacket[30] = myFloatUnion.fBuff[1];
	streamPacket[31] = myFloatUnion.fBuff[2];
	streamPacket[32] = myFloatUnion.fBuff[3];

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

void ape::PointCloudImpl::listenStreamPeerSendThread(RakNet::RakPeerInterface* streamPeer)
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

void ape::PointCloudImpl::listenStreamPeerReceiveThread(RakNet::RakPeerInterface* streamPeer)
{
	while (true)
	{
		RakNet::Packet* packet;
		for (packet = streamPeer->Receive(); packet; streamPeer->DeallocatePacket(packet), packet = streamPeer->Receive())
		{
			if (packet->data[0] == 254)
			{
				dataUnionBytesInt myIntUnion;
				myIntUnion.iBuff[0] = packet->data[1];
				myIntUnion.iBuff[1] = packet->data[2];
				myIntUnion.iBuff[2] = packet->data[3];
				myIntUnion.iBuff[3] = packet->data[4];
				mPointsSize = myIntUnion.i;
				myIntUnion.iBuff[0] = packet->data[5];
				myIntUnion.iBuff[1] = packet->data[6];
				myIntUnion.iBuff[2] = packet->data[7];
				myIntUnion.iBuff[3] = packet->data[8];
				mColorsSize = myIntUnion.i;

				dataUnionBytesFloat myFloatUnion;
				myFloatUnion.fBuff[0] = packet->data[9];
				myFloatUnion.fBuff[1] = packet->data[10];
				myFloatUnion.fBuff[2] = packet->data[11];
				myFloatUnion.fBuff[3] = packet->data[12];
				mParameters.boundigSphereRadius = myFloatUnion.f;
				myFloatUnion.fBuff[0] = packet->data[13];
				myFloatUnion.fBuff[1] = packet->data[14];
				myFloatUnion.fBuff[2] = packet->data[15];
				myFloatUnion.fBuff[3] = packet->data[16];
				mParameters.pointSize = myFloatUnion.f;
				myIntUnion.iBuff[0] = packet->data[17];
				myIntUnion.iBuff[1] = packet->data[18];
				myIntUnion.iBuff[2] = packet->data[19];
				myIntUnion.iBuff[3] = packet->data[20];
				mParameters.pointScale = static_cast<bool>(myIntUnion.i);
				myFloatUnion.fBuff[0] = packet->data[21];
				myFloatUnion.fBuff[1] = packet->data[22];
				myFloatUnion.fBuff[2] = packet->data[23];
				myFloatUnion.fBuff[3] = packet->data[24];
				mParameters.pointScaleOffset = myFloatUnion.f;
				myFloatUnion.fBuff[0] = packet->data[25];
				myFloatUnion.fBuff[1] = packet->data[26];
				myFloatUnion.fBuff[2] = packet->data[27];
				myFloatUnion.fBuff[3] = packet->data[28];
				mParameters.unitScaleDistance = myFloatUnion.f;
				myFloatUnion.fBuff[0] = packet->data[29];
				myFloatUnion.fBuff[1] = packet->data[30];
				myFloatUnion.fBuff[2] = packet->data[31];
				myFloatUnion.fBuff[3] = packet->data[32];
				mParameters.scaleFactor = myFloatUnion.f;

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
				mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::POINT_CLOUD_PARAMETERS));
			}
			else if (packet->data[0] == 255)
			{
				dataUnionBytesInt myIntUnion;
				myIntUnion.iBuff[0] = packet->data[1];
				myIntUnion.iBuff[1] = packet->data[2];
				myIntUnion.iBuff[2] = packet->data[3];
				myIntUnion.iBuff[3] = packet->data[4];
				mPointsSize = myIntUnion.i;
				myIntUnion.iBuff[0] = packet->data[5];
				myIntUnion.iBuff[1] = packet->data[6];
				myIntUnion.iBuff[2] = packet->data[7];
				myIntUnion.iBuff[3] = packet->data[8];
				mColorsSize = myIntUnion.i;

				dataUnionBytesFloat myFloatUnion;
				myFloatUnion.fBuff[0] = packet->data[9];
				myFloatUnion.fBuff[1] = packet->data[10];
				myFloatUnion.fBuff[2] = packet->data[11];
				myFloatUnion.fBuff[3] = packet->data[12];
				mParameters.boundigSphereRadius = myFloatUnion.f;
				myFloatUnion.fBuff[0] = packet->data[13];
				myFloatUnion.fBuff[1] = packet->data[14];
				myFloatUnion.fBuff[2] = packet->data[15];
				myFloatUnion.fBuff[3] = packet->data[16];
				mParameters.pointSize = myFloatUnion.f;
				myIntUnion.iBuff[0] = packet->data[17];
				myIntUnion.iBuff[1] = packet->data[18];
				myIntUnion.iBuff[2] = packet->data[19];
				myIntUnion.iBuff[3] = packet->data[20];
				mParameters.pointScale = static_cast<bool>(myIntUnion.i);
				myFloatUnion.fBuff[0] = packet->data[21];
				myFloatUnion.fBuff[1] = packet->data[22];
				myFloatUnion.fBuff[2] = packet->data[23];
				myFloatUnion.fBuff[3] = packet->data[24];
				mParameters.pointScaleOffset = myFloatUnion.f;
				myFloatUnion.fBuff[0] = packet->data[25];
				myFloatUnion.fBuff[1] = packet->data[26];
				myFloatUnion.fBuff[2] = packet->data[27];
				myFloatUnion.fBuff[3] = packet->data[28];
				mParameters.unitScaleDistance = myFloatUnion.f;
				myFloatUnion.fBuff[0] = packet->data[29];
				myFloatUnion.fBuff[1] = packet->data[30];
				myFloatUnion.fBuff[2] = packet->data[31];
				myFloatUnion.fBuff[3] = packet->data[32];
				mParameters.scaleFactor = myFloatUnion.f;

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
				for (int i = 0; i < mCurrentColorsSize; i++)
				{
					mCurrentColors[i] = ((short)packet->data[packetDataIndex]) / 255.0f;
					packetDataIndex++;
				}
				APE_LOG_DEBUG("Received stream packed with size: " << packet->length << " packetDataIndex after read " << packetDataIndex);
				mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::POINT_CLOUD_POINTS));
				mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::POINT_CLOUD_COLORS));
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


