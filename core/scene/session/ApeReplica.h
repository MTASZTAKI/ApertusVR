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

#ifndef APE_REPLICA_H
#define APE_REPLICA_H

#include "ReplicaManager3.h"
#include "VariableDeltaSerializer.h"
#include "GetTime.h"

namespace Ape
{
	class Replica : public RakNet::Replica3
	{
	private:
		bool mIsHostCreated;

		void PrintStringInBitstream(RakNet::BitStream *bs);

	protected:
		RakNet::RakString mObjectType;

		RakNet::VariableDeltaSerializer mVariableDeltaSerializer;

	public:
		Replica(RakNet::RakString objectType, bool isHostCreated);

		virtual ~Replica();
		
		virtual void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const = 0;
		
		virtual RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3);
		
		virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection);
		
		virtual void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection);
		
		virtual bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection);
		
		virtual void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection);
		
		virtual bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection);
		
		virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const;
		
		virtual void DeallocReplica(RakNet::Connection_RM3 *sourceConnection);
		
		virtual RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection);
		
		virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) = 0;
		
		virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters) = 0;
		
		virtual void OnUserReplicaPreSerializeTick();
		
		virtual void OnPoppedConnection(RakNet::Connection_RM3 *droppedConnection);
		
		virtual void NotifyReplicaOfMessageDeliveryStatus(RakNet::RakNetGUID guid, uint32_t receiptId, bool messageArrived);
		
	};
}

#endif