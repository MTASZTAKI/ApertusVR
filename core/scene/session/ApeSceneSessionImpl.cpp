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


#include "ApeSceneSessionImpl.h"
#include "ApeReplicaManager.h"
#include "ApeNodeImpl.h"

#define STREAM_PORT 3000
#define BIG_PACKET_SIZE 83296256
//#define BIG_PACKET_SIZE 50000
char *text;

Ape::SceneSessionImpl::SceneSessionImpl()
	: mpRakReplicaPeer(nullptr)
	, mpRakStreamPeer(nullptr)
	, mpReplicaManager3(nullptr)
	, mpNatPunchthroughClient(nullptr)
	, mpLobbyManager(nullptr)
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpPluginManager = Ape::IPluginManager::getSingletonPtr();
	mIsConnectedToNATServer = false;
	mIsConnectedToHost = false;
	mNATServerAddress.FromString("");
	mParticipantType = mpSystemConfig->getSceneSessionConfig().participantType;
	mHostGuid = RakNet::RakNetGUID();
	mHostAddress = RakNet::SystemAddress();

	if (mParticipantType == Ape::SceneSession::HOST || mParticipantType == Ape::SceneSession::GUEST)
	{
		init();
	}
	if (mParticipantType == Ape::SceneSession::HOST)
	{
		bool createSessionResult = mpLobbyManager->createSession(mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.sessionName, mGuid.ToString());
		LOG(LOG_TYPE_DEBUG, "lobbyManager->createSession(): " << createSessionResult);
		create();
	}
	else if (mParticipantType == Ape::SceneSession::GUEST)
	{
		Ape::SceneSessionUniqueID uuid;
		if (mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.useLobby)
		{
			LOG(LOG_TYPE_DEBUG, "use lobbyManager to get scene session guid");
			bool getSessionRes = mpLobbyManager->getSessionHostGuid(mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.sessionName, uuid);
			LOG(LOG_TYPE_DEBUG, "lobbyManager->getSessionHostGuid() res: " << getSessionRes << " uuid: " << uuid);
			if (getSessionRes && !uuid.empty())
			{
				connect(uuid);
			}
		}
		else
		{
			uuid = mpSystemConfig->getSceneSessionConfig().sessionGUID;
			LOG(LOG_TYPE_DEBUG, "lobbyManager is turned off, use sessionGUID: " << uuid);
			connect(uuid);
		}
	}
}

Ape::SceneSessionImpl::~SceneSessionImpl()
{
	if (mpRakReplicaPeer)
	{
		mpRakReplicaPeer->Shutdown(100);
		RakNet::RakPeerInterface::DestroyInstance(mpRakReplicaPeer);
	}

	if (mpNatPunchthroughClient)
		RakNet::NatPunchthroughClient::DestroyInstance(mpNatPunchthroughClient);

	if (mpLobbyManager) {
		if (mParticipantType == Ape::SceneSession::ParticipantType::HOST)
			mpLobbyManager->removeSession(mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.sessionName);

		delete mpLobbyManager;
		mpLobbyManager = nullptr;
	}
}

void Ape::SceneSessionImpl::init()
{
	Ape::SceneSessionConfig::NatPunchThroughServerConfig natPunchThroughServerConfig = mpSystemConfig->getSceneSessionConfig().natPunchThroughServerConfig;
	mNATServerIP = natPunchThroughServerConfig.ip;
	mNATServerPort = natPunchThroughServerConfig.port;

	Ape::SceneSessionConfig::LobbyServerConfig lobbyServerConfig = mpSystemConfig->getSceneSessionConfig().lobbyServerConfig;

	mLobbyServerIP = lobbyServerConfig.ip;
	LOG(LOG_TYPE_DEBUG, "mLobbyServerIP: " << mLobbyServerIP);
	mLobbyServerPort = lobbyServerConfig.port;
	LOG(LOG_TYPE_DEBUG, "mLobbyServerPort: " << mLobbyServerPort);
	mLobbyServerSessionName = lobbyServerConfig.sessionName;
	LOG(LOG_TYPE_DEBUG, "mLobbyServerSessionName: " << mLobbyServerSessionName);

	mpLobbyManager = new LobbyManager(mLobbyServerIP, mLobbyServerPort, mLobbyServerSessionName);
	mpRakReplicaPeer = RakNet::RakPeerInterface::GetInstance();
	mpNetworkIDManager = RakNet::NetworkIDManager::GetInstance();
	mpNatPunchthroughClient = RakNet::NatPunchthroughClient::GetInstance();
	mpReplicaManager3 = std::make_shared<Ape::ReplicaManager>();
	mpRakReplicaPeer->AttachPlugin(mpNatPunchthroughClient);
	mpRakReplicaPeer->AttachPlugin(mpReplicaManager3.get());
	mpReplicaManager3->SetNetworkIDManager(mpNetworkIDManager);
	mpReplicaManager3->SetAutoManageConnections(false,true);
	RakNet::SocketDescriptor sd;
	sd.socketFamily = AF_INET; 
	sd.port = 0;
	RakNet::StartupResult sr = mpRakReplicaPeer->Startup(8, &sd, 1);
	RakAssert(sr == RakNet::RAKNET_STARTED);
	mpRakReplicaPeer->SetMaximumIncomingConnections(8);
	mpRakReplicaPeer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mGuid = mpRakReplicaPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mAddress = mpRakReplicaPeer->GetMyBoundAddress();
	LOG(LOG_TYPE_DEBUG, "Our guid is: " << mGuid.ToString());
	LOG(LOG_TYPE_DEBUG, "Started on: " << mAddress.ToString(true));
	RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(mNATServerIP.c_str(), atoi(mNATServerPort.c_str()), 0, 0);
	if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		LOG(LOG_TYPE_DEBUG, "Failed connect call to " << mNATServerIP.c_str() << ". Code=" << car);
	}
	else
	{
		LOG(LOG_TYPE_DEBUG, "Try to connect to NAT punchthrough server: " << mNATServerIP << "|" << mNATServerPort);
		while (!mIsConnectedToNATServer)
		{
			listenReplicaPeer();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	LOG(LOG_TYPE_DEBUG, "runReplicaPeerListenThread");
	std::thread runReplicaPeerListenThread((std::bind(&SceneSessionImpl::runReplicaPeerListen, this)));
	runReplicaPeerListenThread.detach();

	text = new char[BIG_PACKET_SIZE];
	mpRakStreamPeer = RakNet::RakPeerInterface::GetInstance();
	int socketFamily;
	socketFamily = AF_INET;
	if (mParticipantType == Ape::SceneSession::HOST)
	{
		mpRakStreamPeer->SetTimeoutTime(5000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		RakNet::SocketDescriptor socketDescriptor(STREAM_PORT, 0);
		socketDescriptor.socketFamily = socketFamily;
		mpRakStreamPeer->SetMaximumIncomingConnections(4);
		RakNet::StartupResult sr;
		sr = mpRakStreamPeer->Startup(4, &socketDescriptor, 1);
		if (sr != RakNet::RAKNET_STARTED)
		{
			LOG(LOG_TYPE_DEBUG, "Stream server failed to start. Error=" << sr);
		}
		LOG(LOG_TYPE_DEBUG, "Started stream server on " << mpRakStreamPeer->GetMyBoundAddress().ToString(true));
	}
	else if (mParticipantType == Ape::SceneSession::GUEST)
	{
		mpRakStreamPeer->SetTimeoutTime(5000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		RakNet::SocketDescriptor socketDescriptor(0, 0);
		socketDescriptor.socketFamily = socketFamily;
		RakNet::StartupResult sr;
		sr = mpRakStreamPeer->Startup(4, &socketDescriptor, 1);
		if (sr != RakNet::RAKNET_STARTED)
		{
			LOG(LOG_TYPE_DEBUG, "Stream client failed to start. Error=" << sr);
		}
		mpRakStreamPeer->SetSplitMessageProgressInterval(10000);
		LOG(LOG_TYPE_DEBUG, "Started stream client on " << mpRakStreamPeer->GetMyBoundAddress().ToString(true));
	}
	LOG(LOG_TYPE_DEBUG, "runStreamPeerListenThread");
	std::thread runStreamPeerListenThread((std::bind(&SceneSessionImpl::runStreamPeerListen, this)));
	runStreamPeerListenThread.detach();
}

void Ape::SceneSessionImpl::connect(SceneSessionUniqueID sceneSessionUniqueID)
{
	mIsHost = false;
	mHostGuid.FromString(sceneSessionUniqueID.c_str());
	LOG(LOG_TYPE_DEBUG, "Try to NAT punch to: " << mHostGuid.ToString());
	if (mpNatPunchthroughClient->OpenNAT(mHostGuid, mNATServerAddress))
	{
		LOG(LOG_TYPE_DEBUG, "Wait for server response....");
	}
	else
	{
		LOG(LOG_TYPE_DEBUG, "Failed to connect.......");
	}
}

void Ape::SceneSessionImpl::leave()
{
	
}

void Ape::SceneSessionImpl::destroy()
{
}

void Ape::SceneSessionImpl::create()
{
	mIsHost = true;
	mParticipantType = Ape::SceneSession::ParticipantType::HOST;
	LOG(LOG_TYPE_DEBUG, "Listening....");
	mpNatPunchthroughClient->FindRouterPortStride(mNATServerAddress);
}

bool Ape::SceneSessionImpl::isHost()
{
	return mIsHost;
}

Ape::SceneSessionUniqueID Ape::SceneSessionImpl::getGUID()
{
	return mGuid.ToString();
}

std::weak_ptr<RakNet::ReplicaManager3> Ape::SceneSessionImpl::getReplicaManager()
{
	return mpReplicaManager3;
}

Ape::SceneSession::ParticipantType Ape::SceneSessionImpl::getParticipantType()
{
	return mParticipantType;
}


void Ape::SceneSessionImpl::runReplicaPeerListen()
{
	if (mParticipantType == Ape::SceneSession::GUEST)
	{
		LOG(LOG_TYPE_DEBUG, "thread start waiting for all plugin init");
		while (!mpPluginManager->isAllPluginInitialized())
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		LOG(LOG_TYPE_DEBUG, "thread start listening after all plugins are inited");
	}
	while (true)
	{
		listenReplicaPeer();
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
}

void Ape::SceneSessionImpl::runStreamPeerListen()
{
	while (true)
	{
		listenStreamPeer();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Ape::SceneSessionImpl::listenReplicaPeer()
{
	RakNet::Packet *packet;
	for (packet = mpRakReplicaPeer->Receive(); packet; mpRakReplicaPeer->DeallocatePacket(packet), packet = mpRakReplicaPeer->Receive())
	{
		switch (packet->data[0])
		{
			case ID_DISCONNECTION_NOTIFICATION:
				LOG(LOG_TYPE_DEBUG, "ID_DISCONNECTION_NOTIFICATION");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					LOG(LOG_TYPE_DEBUG, "ID_CONNECTION_REQUEST_ACCEPTED from " << packet->systemAddress.ToString(true) << ", guid=" << packet->guid.ToString() << ", participantType=" << mParticipantType);
					if (mNATServerIP == packet->systemAddress.ToString(false))
					{
						mNATServerAddress = packet->systemAddress;
						RakNet::ConnectionState cs = mpRakReplicaPeer->GetConnectionState(mNATServerAddress);
						if (cs == RakNet::IS_CONNECTED)
							mIsConnectedToNATServer = true;
						else
							mIsConnectedToNATServer = false;
					}
					else if (mParticipantType == Ape::SceneSession::ParticipantType::HOST)
					{
						RakNet::Connection_RM3 *connection = mpReplicaManager3->AllocConnection(packet->systemAddress, packet->guid);
						if (mpReplicaManager3->PushConnection(connection))
						{
							LOG(LOG_TYPE_DEBUG, "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful");
						}
						else
						{
							mpReplicaManager3->DeallocConnection(connection);
							LOG(LOG_TYPE_DEBUG, "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated");
						}
					}
				}
				break;
			case ID_ALREADY_CONNECTED:
				{
					LOG(LOG_TYPE_DEBUG, "ID_ALREADY_CONNECTED with: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString());
					break;
				}
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				{
					LOG(LOG_TYPE_DEBUG, "Failed to connect to: " << packet->systemAddress.ToString() << ". Reason:" << RakNet::PacketLogger::BaseIDTOString(packet->data[0]));
					break;
				}
			case ID_NAT_TARGET_NOT_CONNECTED:
			case ID_NAT_TARGET_UNRESPONSIVE:
			case ID_NAT_CONNECTION_TO_TARGET_LOST:
			case ID_NAT_ALREADY_IN_PROGRESS:
			case ID_NAT_PUNCHTHROUGH_FAILED:
				{
					LOG(LOG_TYPE_DEBUG, "NAT punch to: " << packet->guid.ToString() << " failed. Reason: " << RakNet::PacketLogger::BaseIDTOString(packet->data[0]));
					break;
				}
			case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
				{
					unsigned char weAreTheSender = packet->data[1];
					LOG(LOG_TYPE_DEBUG, "ID_NAT_PUNCHTHROUGH_SUCCEEDED: weAreTheSender=" << weAreTheSender);
					if (mParticipantType == Ape::SceneSession::ParticipantType::HOST)
					{
						mpRakReplicaPeer->GetConnectionState(packet->systemAddress);
						RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
						if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
						{
							LOG(LOG_TYPE_DEBUG, "Failed connect call to " << packet->systemAddress.ToString(true) << ". Code=" << car);
						}
						else
						{
							LOG(LOG_TYPE_DEBUG, "NAT punch success from remote system " << packet->systemAddress.ToString(true));
						}
					}
					else if (mParticipantType == Ape::SceneSession::ParticipantType::GUEST)
					{
						if (!weAreTheSender)
						{
							mpRakReplicaPeer->GetConnectionState(packet->systemAddress);
							RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
							if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
							{
								LOG(LOG_TYPE_DEBUG, "Failed connect call to " << packet->systemAddress.ToString(true) << ". Code=" << car);
							}
							else
							{
								LOG(LOG_TYPE_DEBUG, "NAT punch success from remote system " << packet->systemAddress.ToString(true));
							}
						}
						else
						{
							RakNet::Connection_RM3 *connection = mpReplicaManager3->AllocConnection(packet->systemAddress, packet->guid);
							if (mpReplicaManager3->PushConnection(connection))
							{
								LOG(LOG_TYPE_DEBUG, "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful");
								mIsConnectedToHost = true;
								mHostAddress = packet->systemAddress;
								LOG(LOG_TYPE_DEBUG, "Try to connect to host after NatPunchthrough for streaming: " << mHostAddress.ToString(false) << "|" << STREAM_PORT);
								mpRakStreamPeer->Connect(mHostAddress.ToString(false), STREAM_PORT, 0, 0);
							}
							else
							{
								mpReplicaManager3->DeallocConnection(connection);
								LOG(LOG_TYPE_DEBUG, "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated");
							}
						}
					}
				}
				break;
			case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
				{
					if (mpReplicaManager3->GetAllConnectionDownloadsCompleted() == true)
					{
						LOG(LOG_TYPE_DEBUG, "Completed all remote downloads");
					}
					break;
				}
			default:
				LOG(LOG_TYPE_DEBUG, "Unknown message type" << packet->data[0]);
		}
	}
}

void Ape::SceneSessionImpl::listenStreamPeer()
{
	RakNet::Packet *packet;
	if (mParticipantType == Ape::SceneSession::HOST)
	{
		for (packet = mpRakStreamPeer->Receive(); packet; mpRakStreamPeer->DeallocatePacket(packet), packet = mpRakStreamPeer->Receive())
		{
			if (packet->data[0] == ID_NEW_INCOMING_CONNECTION || packet->data[0] == 253)
			{
				for (int i = 0; i < 100; i++)
				{
					LOG(LOG_TYPE_DEBUG, "Try for starting send " << BIG_PACKET_SIZE << " bytes sized big packet to " << packet->systemAddress.ToString(true));
					if (BIG_PACKET_SIZE <= 100000)
					{
						for (int i = 0; i < BIG_PACKET_SIZE; i++)
							text[i] = 255 - (i & 255);
					}
					else
						text[0] = (unsigned char)255;
					mpRakStreamPeer->Send(text, BIG_PACKET_SIZE, IMMEDIATE_PRIORITY, RELIABLE_ORDERED_WITH_ACK_RECEIPT, 0, packet->systemAddress, false);
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
	}
	else if (mParticipantType == Ape::SceneSession::GUEST)
	{
		for (packet = mpRakStreamPeer->Receive(); packet; mpRakStreamPeer->DeallocatePacket(packet), packet = mpRakStreamPeer->Receive())
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
				if (BIG_PACKET_SIZE <= 100000)
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
		}
	}
}

