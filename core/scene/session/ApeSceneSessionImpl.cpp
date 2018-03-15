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


#include "ApeSceneSessionImpl.h"
#include "ApeReplicaManager.h"
#include "ApeNodeImpl.h"

template<> Ape::ISceneSession* Ape::Singleton<Ape::ISceneSession>::msSingleton = 0;

Ape::SceneSessionImpl::SceneSessionImpl()
	: mpRakPeer(nullptr)
	, mpReplicaManager3(nullptr)
	, mpNatPunchthroughClient(nullptr)
	, mpLobbyManager(nullptr)
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mIsConnectedToNATServer = false;
	mbIsConnectedToSessionServer = false;
	mNATServerAddress.FromString("");
	mParticipantType = mpSystemConfig->getSceneSessionConfig().participantType;

	if (mParticipantType == Ape::SceneSession::HOST || mParticipantType == Ape::SceneSession::GUEST)
	{
		init();
	}
	if (mParticipantType == Ape::SceneSession::HOST)
	{
		//if (mpLobbyManager->createSession(mpSystemConfig->getSceneSessionConfig().sessionName, mGuid.ToString()))
			create();
		/*else
			std::cout << "SceneSessionImpl(): lobbyManager->createSession() failed." << std::endl;*/
	}
	else if (mParticipantType == Ape::SceneSession::GUEST)
	{
		/*Ape::SceneSessionUniqueID uuid;
		if (mpLobbyManager->getSessionHostGuid(mpSystemConfig->getSceneSessionConfig().sessionName, uuid))
		{
			if (!uuid.empty())*/
			connect(mpSystemConfig->getSceneSessionConfig().sessionGUID);
			/*else
				std::cout << "SceneSessionImpl(): lobbyManager->getSessionHostGuid() returned empty uuid." << std::endl;
		}
		else
		{
			std::cout << "SceneSessionImpl(): lobbyManager->getSessionHostGuid() failed." << std::endl;
		}*/
	}
}

Ape::SceneSessionImpl::~SceneSessionImpl()
{
	if (mpRakPeer)
	{
		mpRakPeer->Shutdown(100);
		RakNet::RakPeerInterface::DestroyInstance(mpRakPeer);
	}

	if (mpNatPunchthroughClient)
		RakNet::NatPunchthroughClient::DestroyInstance(mpNatPunchthroughClient);

	/*if (mpLobbyManager) {
		if (mParticipantType == Ape::SceneSession::ParticipantType::HOST)
			mpLobbyManager->removeSession(mpSystemConfig->getSceneSessionConfig().sessionName);

		delete mpLobbyManager;
		mpLobbyManager = nullptr;
	}*/
}

void Ape::SceneSessionImpl::init()
{
	Ape::SceneSessionConfig::NatPunchThroughServerConfig natPunchThroughServerConfig = mpSystemConfig->getSceneSessionConfig().natPunchThroughServerConfig;
	mNATServerIP = natPunchThroughServerConfig.ip;
	mNATServerPort = natPunchThroughServerConfig.port;

	Ape::SceneSessionConfig::LobbyServerConfig lobbyServerConfig = mpSystemConfig->getSceneSessionConfig().lobbyServerConfig;
	mLobbyServerIP = lobbyServerConfig.ip;
	std::cout << "mLobbyServerIP: " << mLobbyServerIP << std::endl;
	mLobbyServerPort = lobbyServerConfig.port;
	std::cout << "mLobbyServerPort: " << mLobbyServerPort << std::endl;
	//mpLobbyManager = new LobbyManager(mLobbyServerIP, mLobbyServerPort);

	mpRakPeer = RakNet::RakPeerInterface::GetInstance();
	mpNetworkIDManager = RakNet::NetworkIDManager::GetInstance();
	mpNatPunchthroughClient = RakNet::NatPunchthroughClient::GetInstance();
	mpReplicaManager3 = std::make_shared<Ape::ReplicaManager>();
	mpRakPeer->AttachPlugin(mpNatPunchthroughClient);
	mpRakPeer->AttachPlugin(mpReplicaManager3.get());
	mpReplicaManager3->SetNetworkIDManager(mpNetworkIDManager);
	mpReplicaManager3->SetAutoManageConnections(false,true);
	RakNet::SocketDescriptor sd;
	sd.socketFamily = AF_INET; 
	sd.port = 0;
	mpRakPeer->Startup(8, &sd, 1);
	RakAssert(sr == RakNet::RAKNET_STARTED);
	mpRakPeer->SetMaximumIncomingConnections(8);
	mpRakPeer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mGuid = mpRakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mAddress = mpRakPeer->GetMyBoundAddress();
	printf("Our guid is %s\n", mGuid.ToString());
	printf("Started on %s\n", mAddress.ToString(true));
	RakNet::ConnectionAttemptResult car = mpRakPeer->Connect(mNATServerIP.c_str(), atoi(mNATServerPort.c_str()), 0, 0);
	if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
		printf("Failed connect call to %s. Code=%i\n", mNATServerIP.c_str(), car);
	else
		std::cout << "Try to connect to NAT punchthrough server: " << mNATServerIP << "|" << mNATServerPort << std::endl;
	std::thread runThread((std::bind(&SceneSessionImpl::run, this)));
	runThread.detach();
}

void Ape::SceneSessionImpl::connect(SceneSessionUniqueID sceneSessionUniqueID)
{
	mIsHost = false;
	mParticipantType = Ape::SceneSession::ParticipantType::GUEST;
	RakNet::RakNetGUID serverGUID;
	serverGUID.FromString(sceneSessionUniqueID.c_str());
	std::cout << "Try to NAT punch to: " << serverGUID.ToString() << std::endl;
	if (mpNatPunchthroughClient->OpenNAT(serverGUID, mNATServerAddress))
		std::cout << "Wait for server response...." << std::endl;
	else
		std::cout << "Failed to connect......." << std::endl;
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
	std::cout << "Listening...." << std::endl;
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


void Ape::SceneSessionImpl::run()
{
	while (true)
	{
		listen();
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
}

void Ape::SceneSessionImpl::listen()
{
	RakNet::Packet *packet;
	for (packet = mpRakPeer->Receive(); packet; mpRakPeer->DeallocatePacket(packet), packet = mpRakPeer->Receive())
	{
		switch (packet->data[0])
		{
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("ID_NEW_INCOMING_CONNECTION from %s. guid=%s.\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("ID_CONNECTION_REQUEST_ACCEPTED from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
					if (mNATServerIP == packet->systemAddress.ToString(false))
					{
						mNATServerAddress = packet->systemAddress;
						RakNet::ConnectionState cs = mpRakPeer->GetConnectionState(mNATServerAddress);
						if (cs == RakNet::IS_CONNECTED)
							mIsConnectedToNATServer = true;
						else
							mIsConnectedToNATServer = false;
					}
					else if (mParticipantType == Ape::SceneSession::ParticipantType::HOST)
					{
						RakNet::Connection_RM3 *connection = mpReplicaManager3->AllocConnection(packet->systemAddress, packet->guid);
						if (mpReplicaManager3->PushConnection(connection))
							std::cout << "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful" << std::endl;
						else
						{
							mpReplicaManager3->DeallocConnection(connection);
							std::cout << "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated" << std::endl;
						}
					}
				}
				break;
			case ID_ALREADY_CONNECTED:
				std::cout << "ID_ALREADY_CONNECTED with: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << std::endl;
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
					printf("Failed to connect to %s. Reason %s\n", packet->systemAddress.ToString(true), RakNet::PacketLogger::BaseIDTOString(packet->data[0]));
				break;
			case ID_NAT_TARGET_NOT_CONNECTED:
			case ID_NAT_TARGET_UNRESPONSIVE:
			case ID_NAT_CONNECTION_TO_TARGET_LOST:
			case ID_NAT_ALREADY_IN_PROGRESS:
			case ID_NAT_PUNCHTHROUGH_FAILED:
					printf("NAT punch to %s failed. Reason %s\n", packet->guid.ToString(), RakNet::PacketLogger::BaseIDTOString(packet->data[0]));
				break;
			case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
				{
					unsigned char weAreTheSender = packet->data[1];
					if (mParticipantType == Ape::SceneSession::ParticipantType::HOST)
					{
						mpRakPeer->GetConnectionState(packet->systemAddress);
						RakNet::ConnectionAttemptResult car = mpRakPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
						if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
							printf("Failed connect call to %s. Code=%i\n", packet->systemAddress.ToString(true), car);
						else
							printf("NAT punch success from remote system %s.\n", packet->systemAddress.ToString(true));
					}
					else if (mParticipantType == Ape::SceneSession::ParticipantType::GUEST)
					{
						if (!weAreTheSender)
						{
							mpRakPeer->GetConnectionState(packet->systemAddress);
							RakNet::ConnectionAttemptResult car = mpRakPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
							if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
								printf("Failed connect call to %s. Code=%i\n", packet->systemAddress.ToString(true), car);
							else
								printf("NAT punch success from remote system %s.\n", packet->systemAddress.ToString(true));
						}
						else
						{
							RakNet::Connection_RM3 *connection = mpReplicaManager3->AllocConnection(packet->systemAddress, packet->guid);
							if (mpReplicaManager3->PushConnection(connection))
							{
								std::cout << "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful" << std::endl;
								mbIsConnectedToSessionServer = true;
							}
							else
							{
								mpReplicaManager3->DeallocConnection(connection);
								std::cout << "Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated" << std::endl;
							}
						}
					}
				}
				break;
			case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
				{
					if (mpReplicaManager3->GetAllConnectionDownloadsCompleted() == true)
						printf("Completed all remote downloads\n");
					break;
				}
			case ID_RAKVOICE_OPEN_CHANNEL_REQUEST:
			case ID_RAKVOICE_OPEN_CHANNEL_REPLY:
					printf("Got new channel from %s\n", packet->systemAddress.ToString());
				break;
			case ID_RAKVOICE_CLOSE_CHANNEL:
					printf("ID_RAKVOICE_CLOSE_CHANNEL\n");
				break;
			case ID_USER_PACKET_ENUM:
				break;
		}
	}
}

