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

#ifndef APE_REPLICA_H
#define APE_REPLICA_H

#include "ReplicaManager3.h"
#include "VariableDeltaSerializer.h"
#include "GetTime.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"

namespace ape
{
	class Replica : public RakNet::Replica3
	{
	private:
		bool mIsHost;

		void PrintStringInBitstream(RakNet::BitStream *bs);

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		std::string mReplicaName;

	protected:
		RakNet::RakString mObjectType;

		std::string mOwnerID;

		RakNet::VariableDeltaSerializer mVariableDeltaSerializer;

	public:
		Replica(RakNet::RakString objectType, std::string name, std::string ownerID, bool isHost);

		virtual ~Replica();
		
		virtual void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const = 0;
		
		RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3);
		
		bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection);
		
		void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection);
		
		bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection);

		RakNet::RM3DestructionState QueryDestruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3);

		bool QueryRelayDestruction(RakNet::Connection_RM3 *sourceConnection) const;
		
		void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection);
		
		bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection);
		
		RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const;
		
		void DeallocReplica(RakNet::Connection_RM3 *sourceConnection);
		
		RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection);
		
		virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) = 0;
		
		virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters) = 0;

		virtual void listenStreamPeerSendThread(RakNet::RakPeerInterface* streamPeer);

		virtual void listenStreamPeerReceiveThread(RakNet::RakPeerInterface* streamPeer);
		
		void OnUserReplicaPreSerializeTick();
		
		void OnPoppedConnection(RakNet::Connection_RM3 *droppedConnection);
		
		void NotifyReplicaOfMessageDeliveryStatus(RakNet::RakNetGUID guid, uint32_t receiptId, bool messageArrived);
		
	};
}

#endif