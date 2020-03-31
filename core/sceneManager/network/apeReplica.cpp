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

#include "apeReplica.h"

ape::Replica::Replica(RakNet::RakString objectType, std::string name, std::string ownerID, bool isHost)
{
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mReplicaName = name;
	mObjectType = objectType;
	mOwnerID = ownerID;
	mIsHost = isHost;
	mIsLastTickSerializedByMe = false;
}

ape::Replica::~Replica()
{

}

RakNet::RM3ConstructionState ape::Replica::QueryConstruction( RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3 )
{
	return QueryConstruction_ClientConstruction(destinationConnection, mIsHost);
}

bool ape::Replica::QueryRemoteConstruction( RakNet::Connection_RM3 *sourceConnection )
{
	return QueryRemoteConstruction_ClientConstruction(sourceConnection, mIsHost);
}

void ape::Replica::SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	mVariableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());
}

bool ape::Replica::DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

RakNet::RM3DestructionState ape::Replica::QueryDestruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3)
{
	//APE_LOG_DEBUG("QueryDestruction");
	return RakNet::RM3DS_DO_NOT_QUERY_DESTRUCTION;
}

bool ape::Replica::QueryRelayDestruction(RakNet::Connection_RM3 *sourceConnection) const
{
	//APE_LOG_DEBUG("QueryRelayDestruction");
	return true;
}

void ape::Replica::SerializeDestruction( RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection )
{
	mVariableDeltaSerializer.RemoveRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());
	destructionBitstream->Write(mObjectType + RakNet::RakString("SerializeDestruction"));
	//APE_LOG_DEBUG("SerializeDestruction");
}

bool ape::Replica::DeserializeDestruction( RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection )
{
	if (destructionBitstream->GetNumberOfBitsUsed() != 0)
	{
		RakNet::RakString rakString;
		destructionBitstream->Read(rakString);
		APE_LOG_DEBUG(mReplicaName << " : " << rakString.C_String());
	}
	return true;
}

RakNet::RM3ActionOnPopConnection ape::Replica::QueryActionOnPopConnection( RakNet::Connection_RM3 *droppedConnection ) const
{
	//return RakNet::RM3AOPC_DO_NOTHING;
	return QueryActionOnPopConnection_Client(droppedConnection);
}

void ape::Replica::DeallocReplica( RakNet::Connection_RM3 *sourceConnection )
{
	//APE_LOG_DEBUG("DeallocReplica");
	if (mIsHost)
		BroadcastDestruction();
	if (mObjectType == "Node")
		mpSceneManager->deleteNode(mReplicaName);
	else
		mpSceneManager->deleteEntity(mReplicaName);
}

RakNet::RM3QuerySerializationResult ape::Replica::QuerySerialization( RakNet::Connection_RM3 *destinationConnection )
{
	if (creatingSystemGUID == replicaManager->GetRakPeerInterface()->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS))
	{
		if (mOwnerID == creatingSystemGUID.ToString())
		{
			return RakNet::RM3QSR_CALL_SERIALIZE;
		}
		else
		{
			if (!mIsHost)
			{
				//APE_LOG_DEBUG("Replica: " << mReplicaName << " is serialized by: " << mOwnerID << " therefore, it is not serialized");
				return RakNet::RM3QSR_DO_NOT_CALL_SERIALIZE;
			}
			else if (destinationConnection->GetRakNetGUID().ToString() != mOwnerID)
			{
				//APE_LOG_DEBUG("Replica: " << mReplicaName << " is serialized by the host to: " << destinationConnection->GetRakNetGUID().ToString());
				return RakNet::RM3QSR_CALL_SERIALIZE;
			}
		}
	}
	if (mIsHost && (destinationConnection->GetRakNetGUID() != creatingSystemGUID))
	{
		return RakNet::RM3QSR_CALL_SERIALIZE;
	}
	if (mIsHost && (destinationConnection->GetRakNetGUID().ToString() != mOwnerID))
	{
		mIsLastTickSerializedByMe = true;
		//APE_LOG_DEBUG("Replica: " << mReplicaName << " is serialized by the host to: " << destinationConnection->GetRakNetGUID().ToString());
		return RakNet::RM3QSR_CALL_SERIALIZE;
	}
	if (mIsHost && (destinationConnection->GetRakNetGUID().ToString() == mOwnerID) && mIsLastTickSerializedByMe)
	{
		mIsLastTickSerializedByMe = false;
		//APE_LOG_DEBUG("Replica: " << mReplicaName << " is serialized by the host to for a last time: " << destinationConnection->GetRakNetGUID().ToString());
		return RakNet::RM3QSR_CALL_SERIALIZE;
	}
	if ((mOwnerID != creatingSystemGUID.ToString()) && (mOwnerID == mpCoreConfig->getNetworkGUID()))
	{
		mIsLastTickSerializedByMe = true;
		//APE_LOG_DEBUG("serialize this replica: " << mReplicaName << " creator: " << creatingSystemGUID.ToString() << " to the host");
		return RakNet::RM3QSR_CALL_SERIALIZE;
	}
	if ((mOwnerID == creatingSystemGUID.ToString()) && (mOwnerID != mpCoreConfig->getNetworkGUID()) && mIsLastTickSerializedByMe)
	{
		mIsLastTickSerializedByMe = false;
		//APE_LOG_DEBUG("serialize this replica for a last time: " << mReplicaName << " and new owner is: " << mOwnerID << " to the host");
		return RakNet::RM3QSR_CALL_SERIALIZE;
	}
	return RakNet::RM3QSR_DO_NOT_CALL_SERIALIZE;
}

void ape::Replica::OnUserReplicaPreSerializeTick()
{
	mVariableDeltaSerializer.OnPreSerializeTick();
}

void ape::Replica::OnPoppedConnection(RakNet::Connection_RM3 *droppedConnection)
{
	mVariableDeltaSerializer.RemoveRemoteSystemVariableHistory(droppedConnection->GetRakNetGUID());
}

void ape::Replica::NotifyReplicaOfMessageDeliveryStatus(RakNet::RakNetGUID guid, uint32_t receiptId, bool messageArrived)
{
	mVariableDeltaSerializer.OnMessageReceipt(guid, receiptId, messageArrived);
}

void ape::Replica::listenStreamPeerSendThread(RakNet::RakPeerInterface* streamPeer)
{

}

void ape::Replica::listenStreamPeerReceiveThread(RakNet::RakPeerInterface* streamPeer)
{

}