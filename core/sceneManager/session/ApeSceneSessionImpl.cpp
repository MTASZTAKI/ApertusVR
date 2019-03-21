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

#define STREAM_PORT 3010

#include "ApeSceneSessionImpl.h"
#include "ApeReplicaManager.h"
#include "ApeNodeImpl.h"

Ape::SceneSessionImpl::SceneSessionImpl()
	: mpRakReplicaPeer(nullptr)
	, mpRakStreamPeer(nullptr)
	, mpReplicaManager3(nullptr)
	, mpNatPunchthroughClient(nullptr)
	, mpLobbyManager(nullptr)
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpPluginManager = Ape::IPluginManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::POINT_CLOUD, std::bind(&SceneSessionImpl::eventCallBack, this, std::placeholders::_1));
	mStreamReplicas = std::vector<Ape::Replica*>();
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
		if (mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.useLobby)
		{
			bool createSessionResult = mpLobbyManager->createSession(mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.sessionName, mGuid.ToString());
			APE_LOG_DEBUG("lobbyManager->createSession(): " << createSessionResult);
		}
		create();
	}
	else if (mParticipantType == Ape::SceneSession::GUEST)
	{
		Ape::SceneSessionUniqueID uuid;
		if (mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.useLobby)
		{
			APE_LOG_DEBUG("use lobbyManager to get scene session guid");
			std::string name = mpSystemConfig->getSceneSessionConfig().lobbyServerConfig.sessionName;
			bool getSessionRes = mpLobbyManager->getSessionHostGuid(name, uuid);
			APE_LOG_DEBUG("lobbyManager->getSessionHostGuid() res: " << getSessionRes << " uuid: " << uuid);
			if (getSessionRes && !uuid.empty())
			{
				connect(uuid);
			}
		}
		else
		{
			uuid = mpSystemConfig->getSceneSessionConfig().sessionGUID;
			APE_LOG_DEBUG("lobbyManager is turned off, use sessionGUID: " << uuid);
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

void Ape::SceneSessionImpl::eventCallBack(const Ape::Event & event)
{
	if (mParticipantType == Ape::SceneSession::HOST)
	{
		if (event.type == Ape::Event::Type::POINT_CLOUD_PARAMETERS)
		{
			if (auto entity = mpSceneManager->getEntity(event.subjectName).lock())
			{
				if (auto pointCloud = ((Ape::PointCloudImpl*)entity.get()))
				{
					mStreamReplicas.push_back(pointCloud);
					APE_LOG_DEBUG("listenStreamPeerSendThread for replica named: " << event.subjectName);
					std::thread runStreamPeerListenThread((std::bind(&Ape::Replica::listenStreamPeerSendThread, pointCloud, mpRakStreamPeer)));
					runStreamPeerListenThread.detach();
				}
			}
		}
	}
	else if (mParticipantType == Ape::SceneSession::GUEST)
	{
		if (event.type == Ape::Event::Type::POINT_CLOUD_CREATE)
		{
			if (auto entity = mpSceneManager->getEntity(event.subjectName).lock())
			{
				if (auto pointCloud = ((Ape::PointCloudImpl*)entity.get()))
				{
					mStreamReplicas.push_back(pointCloud);
					APE_LOG_DEBUG("listenStreamPeerReceiveThread for replica named: " << event.subjectName);
					std::thread runStreamPeerListenThread((std::bind(&Ape::Replica::listenStreamPeerReceiveThread, pointCloud, mpRakStreamPeer)));
					runStreamPeerListenThread.detach();
				}
			}
		}
	}
}

void Ape::SceneSessionImpl::init()
{
	Ape::SceneSessionConfig::NatPunchThroughServerConfig natPunchThroughServerConfig = mpSystemConfig->getSceneSessionConfig().natPunchThroughServerConfig;
	mNATServerIP = natPunchThroughServerConfig.ip;
	mNATServerPort = natPunchThroughServerConfig.port;

	Ape::SceneSessionConfig::LobbyServerConfig lobbyServerConfig = mpSystemConfig->getSceneSessionConfig().lobbyServerConfig;

	mLobbyServerIP = lobbyServerConfig.ip;
	APE_LOG_DEBUG("mLobbyServerIP: " << mLobbyServerIP);
	mLobbyServerPort = lobbyServerConfig.port;
	APE_LOG_DEBUG("mLobbyServerPort: " << mLobbyServerPort);
	mLobbyServerSessionName = lobbyServerConfig.sessionName;
	APE_LOG_DEBUG("mLobbyServerSessionName: " << mLobbyServerSessionName);

	mpLobbyManager = new LobbyManager(mLobbyServerIP, mLobbyServerPort, mLobbyServerSessionName);
	mpRakReplicaPeer = RakNet::RakPeerInterface::GetInstance();
	mpNetworkIDManager = RakNet::NetworkIDManager::GetInstance();
	if (natPunchThroughServerConfig.use)
		mpNatPunchthroughClient = RakNet::NatPunchthroughClient::GetInstance();
	mpReplicaManager3 = std::make_shared<Ape::ReplicaManager>();
	if (natPunchThroughServerConfig.use)
		mpRakReplicaPeer->AttachPlugin(mpNatPunchthroughClient);
	mpRakReplicaPeer->AttachPlugin(mpReplicaManager3.get());
	mpReplicaManager3->SetNetworkIDManager(mpNetworkIDManager);
	mpReplicaManager3->SetAutoManageConnections(false,true);
	RakNet::SocketDescriptor sd;
	sd.socketFamily = AF_INET;
	if (mParticipantType == Ape::SceneSession::HOST)
	{
		sd.port = atoi(mpSystemConfig->getSceneSessionConfig().sessionPort.c_str());
	}
	else if (mParticipantType == Ape::SceneSession::GUEST)
	{
		sd.port = 0;
	}

	RakNet::StartupResult sr = mpRakReplicaPeer->Startup(8, &sd, 1);
	APE_LOG_DEBUG("Raknet StartupResult: " << (int)sr);
	RakAssert(sr == RakNet::RAKNET_STARTED);
	mpRakReplicaPeer->SetMaximumIncomingConnections(8);
	mpRakReplicaPeer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mGuid = mpRakReplicaPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mAddress = mpRakReplicaPeer->GetMyBoundAddress();
	APE_LOG_DEBUG("Our guid is: " << mGuid.ToString());
	APE_LOG_DEBUG("Started on: " << mAddress.ToString(true));
	if (natPunchThroughServerConfig.use)
	{
		RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(mNATServerIP.c_str(), atoi(mNATServerPort.c_str()), 0, 0);
		if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			APE_LOG_DEBUG("Failed connect call to " << mNATServerIP.c_str() << ". Code=" << car);
		}
		else
		{
			APE_LOG_DEBUG("Try to connect to NAT punchthrough server: " << mNATServerIP << "|" << mNATServerPort);
			while (!mIsConnectedToNATServer)
			{
				listenReplicaPeer();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	}

	APE_LOG_DEBUG("runReplicaPeerListenThread");
	std::thread runReplicaPeerListenThread((std::bind(&SceneSessionImpl::runReplicaPeerListen, this)));
	runReplicaPeerListenThread.detach();

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
			APE_LOG_DEBUG("Stream server failed to start. Error=" << sr);
		}
		APE_LOG_DEBUG("Started stream server on " << mpRakStreamPeer->GetMyBoundAddress().ToString(true));
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
			APE_LOG_DEBUG("Stream client failed to start. Error=" << sr);
		}
		APE_LOG_DEBUG("Started stream client on " << mpRakStreamPeer->GetMyBoundAddress().ToString(true));
	}
}

void Ape::SceneSessionImpl::connect(SceneSessionUniqueID sceneSessionUniqueID)
{
	mIsHost = false;
	mHostGuid.FromString(sceneSessionUniqueID.c_str());
	if (mpSystemConfig->getSceneSessionConfig().natPunchThroughServerConfig.use)
	{
		APE_LOG_DEBUG("Try to NAT punch to: " << mHostGuid.ToString());
		if (mpNatPunchthroughClient->OpenNAT(mHostGuid, mNATServerAddress))
		{
			APE_LOG_DEBUG("Wait for server response....");
		}
		else
		{
			APE_LOG_DEBUG("Failed to connect.......");
		}
	}
	else
	{
		APE_LOG_DEBUG("Try to connect to host IP: " << mpSystemConfig->getSceneSessionConfig().sessionIP << " port: " << mpSystemConfig->getSceneSessionConfig().sessionPort);
		RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(mpSystemConfig->getSceneSessionConfig().sessionIP.c_str(), atoi(mpSystemConfig->getSceneSessionConfig().sessionPort.c_str()), 0, 0);
		if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			APE_LOG_DEBUG("Failed connect call to " << mpSystemConfig->getSceneSessionConfig().sessionIP << ". Code=" << car);
		}
		else
		{
			APE_LOG_DEBUG("Connection attempt was successful to remote system " << mpSystemConfig->getSceneSessionConfig().sessionIP);
		}
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
	APE_LOG_DEBUG("Listening....");
	if (mpSystemConfig->getSceneSessionConfig().natPunchThroughServerConfig.use)
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

void Ape::SceneSessionImpl::setScene(Ape::ISceneManager* scene)
{
	mpSceneManager = scene;
}

Ape::SceneSession::ParticipantType Ape::SceneSessionImpl::getParticipantType()
{
	return mParticipantType;
}


void Ape::SceneSessionImpl::runReplicaPeerListen()
{
	if (mParticipantType == Ape::SceneSession::GUEST)
	{
		APE_LOG_DEBUG("thread start waiting for all plugin init");
		//TODO_CORE
		//while (!mpPluginManager->isAllPluginInitialized())
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		APE_LOG_DEBUG("thread start listening after all plugins are inited");
	}
	while (true)
	{
		listenReplicaPeer();
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
}

void Ape::SceneSessionImpl::listenReplicaPeer()
{
	RakNet::Packet *packet;
	for (packet = mpRakReplicaPeer->Receive(); packet; mpRakReplicaPeer->DeallocatePacket(packet), packet = mpRakReplicaPeer->Receive())
	{
		switch (packet->data[0])
		{
			case ID_NEW_INCOMING_CONNECTION:
			{
				APE_LOG_DEBUG("ID_NEW_INCOMING_CONNECTION"); 
				if (mParticipantType == Ape::SceneSession::HOST)
				{
					RakNet::Connection_RM3 *connection = mpReplicaManager3->AllocConnection(packet->systemAddress, packet->guid);
					if (mpReplicaManager3->PushConnection(connection))
					{
						APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful");
					}
					else
					{
						mpReplicaManager3->DeallocConnection(connection);
						APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated");
					}
				}
			}
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				APE_LOG_DEBUG("ID_DISCONNECTION_NOTIFICATION");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					APE_LOG_DEBUG("ID_CONNECTION_REQUEST_ACCEPTED from " << packet->systemAddress.ToString(true) << ", guid=" << packet->guid.ToString() << ", participantType=" << mParticipantType);
					if (mpSystemConfig->getSceneSessionConfig().natPunchThroughServerConfig.use)
					{
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
								APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful");
							}
							else
							{
								mpReplicaManager3->DeallocConnection(connection);
								APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated");
							}
						}
					}
					else if (mParticipantType == Ape::SceneSession::ParticipantType::GUEST)
					{
						RakNet::Connection_RM3 *connection = mpReplicaManager3->AllocConnection(packet->systemAddress, packet->guid);
						if (mpReplicaManager3->PushConnection(connection))
						{
							APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful");
						}
						else
						{
							mpReplicaManager3->DeallocConnection(connection);
							APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated");
						}
					}
				}
				break;
			case ID_ALREADY_CONNECTED:
				{
					APE_LOG_DEBUG("ID_ALREADY_CONNECTED with: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString());
					break;
				}
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				{
					APE_LOG_DEBUG("Failed to connect to: " << packet->systemAddress.ToString() << ". Reason:" << RakNet::PacketLogger::BaseIDTOString(packet->data[0]));
					break;
				}
			case ID_NAT_TARGET_NOT_CONNECTED:
			case ID_NAT_TARGET_UNRESPONSIVE:
			case ID_NAT_CONNECTION_TO_TARGET_LOST:
			case ID_NAT_ALREADY_IN_PROGRESS:
			case ID_NAT_PUNCHTHROUGH_FAILED:
				{
					APE_LOG_DEBUG("NAT punch to: " << packet->guid.ToString() << " failed. Reason: " << RakNet::PacketLogger::BaseIDTOString(packet->data[0]));
					break;
				}
			case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
				{
					unsigned char weAreTheSender = packet->data[1];
					APE_LOG_DEBUG("ID_NAT_PUNCHTHROUGH_SUCCEEDED: weAreTheSender=" << weAreTheSender);
					if (mParticipantType == Ape::SceneSession::ParticipantType::HOST)
					{
						mpRakReplicaPeer->GetConnectionState(packet->systemAddress);
						RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
						if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
						{
							APE_LOG_DEBUG("Failed connect call to " << packet->systemAddress.ToString(true) << ". Code=" << car);
						}
						else
						{
							APE_LOG_DEBUG("NAT punch success from remote system " << packet->systemAddress.ToString(true));
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
								APE_LOG_DEBUG("Failed connect call to " << packet->systemAddress.ToString(true) << ". Code=" << car);
							}
							else
							{
								APE_LOG_DEBUG("NAT punch success from remote system " << packet->systemAddress.ToString(true));
							}
						}
						else
						{
							RakNet::Connection_RM3 *connection = mpReplicaManager3->AllocConnection(packet->systemAddress, packet->guid);
							if (mpReplicaManager3->PushConnection(connection))
							{
								APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was successful");
							}
							else
							{
								mpReplicaManager3->DeallocConnection(connection);
								APE_LOG_DEBUG("Alloc connection to: " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString() << " was not successful thus this was deallocated");
							}
						}
					}
				}
				break;
			case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
				{
					if (mpReplicaManager3->GetAllConnectionDownloadsCompleted() == true)
					{
						APE_LOG_DEBUG("Completed all remote downloads");
						if (mParticipantType == Ape::SceneSession::ParticipantType::GUEST)
						{
							//std::this_thread::sleep_for(std::chrono::milliseconds(10000));
							mIsConnectedToHost = true;
							mHostAddress = packet->systemAddress;
							APE_LOG_DEBUG("Try to connect to host for streaming: " << mHostAddress.ToString(false) << "|" << STREAM_PORT);
							mpRakStreamPeer->Connect(mHostAddress.ToString(false), STREAM_PORT, 0, 0);
						}
					}
					break;
				}
			default:
			{
				//APE_LOG_DEBUG("Unknown message type" << packet->data[0]);
			}
		}
	}
}

