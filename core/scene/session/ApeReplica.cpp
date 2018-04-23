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

#include "ApeReplica.h"

Ape::Replica::Replica(RakNet::RakString objectType, bool isHostCreated)
{
	mObjectType = objectType;
	mIsHostCreated = isHostCreated;
}

Ape::Replica::~Replica()
{

}

RakNet::RM3ConstructionState Ape::Replica::QueryConstruction( RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3 )
{
	return QueryConstruction_ClientConstruction(destinationConnection, mIsHostCreated);
}

bool Ape::Replica::QueryRemoteConstruction( RakNet::Connection_RM3 *sourceConnection )
{
	return QueryRemoteConstruction_ClientConstruction(sourceConnection, mIsHostCreated);
}

void Ape::Replica::SerializeDestruction( RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection )
{
	mVariableDeltaSerializer.RemoveRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());
	destructionBitstream->Write(mObjectType + RakNet::RakString(" SerializeDestruction"));
}

bool Ape::Replica::DeserializeDestruction( RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection )
{
	PrintStringInBitstream(destructionBitstream);
	return true;
}

void Ape::Replica::SerializeConstruction( RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection )
{
	mVariableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());
}

bool Ape::Replica::DeserializeConstruction( RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection )
{
	return true;
}

RakNet::RM3ActionOnPopConnection Ape::Replica::QueryActionOnPopConnection( RakNet::Connection_RM3 *droppedConnection ) const
{
	return QueryActionOnPopConnection_Client(droppedConnection);
}

void Ape::Replica::DeallocReplica( RakNet::Connection_RM3 *sourceConnection )
{
	delete this;
}

RakNet::RM3QuerySerializationResult Ape::Replica::QuerySerialization( RakNet::Connection_RM3 *destinationConnection )
{
	return QuerySerialization_ClientSerializable(destinationConnection, mIsHostCreated);
}

void Ape::Replica::OnUserReplicaPreSerializeTick()
{
	mVariableDeltaSerializer.OnPreSerializeTick();
}

void Ape::Replica::OnPoppedConnection( RakNet::Connection_RM3 *droppedConnection )
{
	mVariableDeltaSerializer.RemoveRemoteSystemVariableHistory(droppedConnection->GetRakNetGUID());
}

void Ape::Replica::NotifyReplicaOfMessageDeliveryStatus( RakNet::RakNetGUID guid, uint32_t receiptId, bool messageArrived )
{
	mVariableDeltaSerializer.OnMessageReceipt(guid,receiptId,messageArrived);
}

void Ape::Replica::PrintStringInBitstream( RakNet::BitStream *bs )
{
	if (bs->GetNumberOfBitsUsed() == 0) return;
	RakNet::RakString rakString;
	bs->Read(rakString);
	LOG(LOG_TYPE_DEBUG, "Receive: " << rakString.C_String());
}