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

#include "apeSceneNetworkImpl.h"
#include "apeReplicaManager.h"
#include "apeNodeImpl.h"

ape::SceneNetworkImpl::SceneNetworkImpl()
	: mpRakReplicaPeer(nullptr)
	, mpRakStreamPeer(nullptr)
	, mpReplicaManager3(nullptr)
	, mpNatPunchthroughClient(nullptr)
	, mpLobbyManager(nullptr)
{
	msSingleton = this;
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&SceneNetworkImpl::eventCallBack, this, std::placeholders::_1));
	mStreamReplicas = std::vector<ape::Replica*>();
	mIsConnectedToNATServer = false;
	mIsNATPunchthrough2HostSucceeded = false;
	mIsNATPunchthrough2HostResponded = false;
	mNATServerAddress.FromString("");
	mParticipantType = mpCoreConfig->getNetworkConfig().participant;
	mReplicaHostGuid = RakNet::RakNetGUID();

	ape::NetworkConfig::LobbyConfig lobbyServerConfig = mpCoreConfig->getNetworkConfig().lobbyConfig;
	mLobbyServerIP = lobbyServerConfig.ip;
	APE_LOG_DEBUG("mLobbyServerIP: " << mLobbyServerIP);
	mLobbyServerPort = lobbyServerConfig.port;
	APE_LOG_DEBUG("mLobbyServerPort: " << mLobbyServerPort);
	if (!mLobbyServerSessionName.length())
		mLobbyServerSessionName = lobbyServerConfig.roomName;
	APE_LOG_DEBUG("mLobbyServerSessionName: " << mLobbyServerSessionName);
	mpLobbyManager = new LobbyManager(mLobbyServerIP, mLobbyServerPort, mLobbyServerSessionName);

	if (mpCoreConfig->getNetworkConfig().resourceZipUrl.size() && mpCoreConfig->getNetworkConfig().resourceDownloadLocation.size())
	{
		APE_LOG_DEBUG("use lobbyManager to update the resources...");
		mpLobbyManager->downloadResources(mpCoreConfig->getNetworkConfig().resourceZipUrl, mpCoreConfig->getNetworkConfig().resourceDownloadLocation, mpCoreConfig->getNetworkConfig().resourceMd5Url);
	}

	if (mParticipantType == ape::SceneNetwork::HOST)
	{
		mIsReplicaHost = true;
		init();
		if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
		{
			bool createSessionResult = mpLobbyManager->createSession(mpCoreConfig->getNetworkConfig().lobbyConfig.roomName, mGuid.ToString());
			APE_LOG_DEBUG("lobbyManager->createSession(): " << createSessionResult);
		}
		createReplicaHost();
	}
	else if (mParticipantType == ape::SceneNetwork::GUEST)
	{
		mIsReplicaHost = false;
		init();
		std::string uuid;
		if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
		{
			APE_LOG_DEBUG("use lobbyManager to get scene session guid");
			std::string name = mpCoreConfig->getNetworkConfig().lobbyConfig.roomName;
			bool getSessionRes = mpLobbyManager->getSessionHostGuid(name, uuid);
			APE_LOG_DEBUG("lobbyManager->getSessionHostGuid() res: " << getSessionRes << " uuid: " << uuid);
			//if (getSessionRes && !uuid.empty())
			//{
				//connect2ReplicaHost(uuid);
			//}
		}
		/*while (!mIsNATPunchthrough2HostSucceeded)
		{*/
			connect2ReplicaHost(uuid);
			/*while (!mIsNATPunchthrough2HostResponded)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}*/
	}
}

ape::SceneNetworkImpl::~SceneNetworkImpl()
{
	if (mpRakReplicaPeer)
	{
		mpRakReplicaPeer->Shutdown(100);
		RakNet::RakPeerInterface::DestroyInstance(mpRakReplicaPeer);
	}

	if (mpNatPunchthroughClient)
		RakNet::NatPunchthroughClient::DestroyInstance(mpNatPunchthroughClient);

	if (mpLobbyManager)
	{
		if (mParticipantType == ape::SceneNetwork::ParticipantType::HOST)
			mpLobbyManager->removeSession(mpCoreConfig->getNetworkConfig().lobbyConfig.roomName);

		delete mpLobbyManager;
		mpLobbyManager = nullptr;
	}
}

void ape::SceneNetworkImpl::eventCallBack(const ape::Event & event)
{
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::LAN && mpCoreConfig->getNetworkConfig().lanConfig.hostStreamPort.length())
	{
		if (auto entity = mpSceneManager->getEntity(event.subjectName).lock())
		{
			if (auto pointCloud = ((ape::PointCloudImpl*)entity.get()))
			{
				if (mIsStreamHost)
				{
					if (event.type == ape::Event::Type::POINT_CLOUD_PARAMETERS)
					{
						mStreamReplicas.push_back(pointCloud);
						APE_LOG_DEBUG("listenStreamPeerSenderThread for replica named: " << event.subjectName);
						std::thread runStreamPeerSenderListenThread((std::bind(&ape::Replica::listenStreamPeerSendThread, pointCloud, mpRakStreamPeer)));
						runStreamPeerSenderListenThread.detach();
					}
				}
				else if(event.type == ape::Event::Type::POINT_CLOUD_CREATE)
				{
					mStreamReplicas.push_back(pointCloud);
					APE_LOG_DEBUG("listenStreamPeerReceiverThread for replica named: " << event.subjectName);
					std::thread runStreamPeerReceiverListenThread((std::bind(&ape::Replica::listenStreamPeerReceiveThread, pointCloud, mpRakStreamPeer)));
					runStreamPeerReceiverListenThread.detach();
				}
			}
		}
	}
}

void ape::SceneNetworkImpl::init()
{
	ape::NetworkConfig::NatPunchThroughConfig natPunchThroughServerConfig = mpCoreConfig->getNetworkConfig().natPunchThroughConfig;
	mNATServerIP = natPunchThroughServerConfig.ip;
	mNATServerPort = natPunchThroughServerConfig.port;
	ape::NetworkConfig::LanConfig localNetworkConfig = mpCoreConfig->getNetworkConfig().lanConfig;
	mpRakReplicaPeer = RakNet::RakPeerInterface::GetInstance();
	mpNetworkIDManager = RakNet::NetworkIDManager::GetInstance();
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
	{
		mpNatPunchthroughClient = RakNet::NatPunchthroughClient::GetInstance();
		mpRakReplicaPeer->AttachPlugin(mpNatPunchthroughClient);
	}
	mpReplicaManager3 = std::make_shared<ape::ReplicaManager>();
	mpRakReplicaPeer->AttachPlugin(mpReplicaManager3.get());
	mpReplicaManager3->SetNetworkIDManager(mpNetworkIDManager);
	mpReplicaManager3->SetAutoManageConnections(false,true);
	RakNet::SocketDescriptor sd;
	sd.socketFamily = AF_INET;
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::LAN)
	{
		if (mParticipantType == ape::SceneNetwork::HOST)
		{
			sd.port = atoi(localNetworkConfig.hostReplicaPort.c_str());
		}
	}
	RakNet::StartupResult sr = mpRakReplicaPeer->Startup(8, &sd, 1);
	APE_LOG_DEBUG("Raknet StartupResult: " << (int)sr);
	RakAssert(sr == RakNet::RAKNET_STARTED);
	mpRakReplicaPeer->SetMaximumIncomingConnections(8);
	mpRakReplicaPeer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mGuid = mpRakReplicaPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	mpCoreConfig->setNetworkGUID(mGuid.ToString());

	mAddress = mpRakReplicaPeer->GetSystemAddressFromGuid(mGuid);
	APE_LOG_DEBUG("Our guid is: " << mGuid.ToString());
	APE_LOG_DEBUG("Started on: " << mAddress.ToString(true));
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
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
	std::thread runReplicaPeerListenThread((std::bind(&SceneNetworkImpl::runReplicaPeerListen, this)));
	runReplicaPeerListenThread.detach();
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::LAN && mpCoreConfig->getNetworkConfig().lanConfig.hostStreamPort.length())
	{
		mpRakStreamPeer = RakNet::RakPeerInterface::GetInstance();
		int socketFamily;
		socketFamily = AF_INET;
		auto myIPAddress = std::string(mAddress.ToString(false));
		if (mpCoreConfig->getNetworkConfig().lanConfig.hostStreamIP == myIPAddress)
		{
			mIsStreamHost = true;
			mpRakStreamPeer->SetTimeoutTime(5000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
			RakNet::SocketDescriptor socketDescriptor(atoi(mpCoreConfig->getNetworkConfig().lanConfig.hostStreamPort.c_str()), 0);
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
		else
		{
			mIsStreamHost = false;
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
}

void ape::SceneNetworkImpl::connect2ReplicaHost(std::string guid)
{
	mReplicaHostGuid.FromString(guid.c_str());
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
	{
		APE_LOG_DEBUG("Try to NAT punch to host: " << mReplicaHostGuid.ToString());
		if (mpNatPunchthroughClient->OpenNAT(mReplicaHostGuid, mNATServerAddress))
		{
			APE_LOG_DEBUG("Wait for host response....");
			mIsNATPunchthrough2HostResponded = false;
		}
		else
		{
			mIsNATPunchthrough2HostResponded = true;
			APE_LOG_DEBUG("Failed to connect.......");
		}
	}
	else if ((mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::LAN))
	{
		APE_LOG_DEBUG("Try to connect to host IP: " << mpCoreConfig->getNetworkConfig().lanConfig.hostReplicaIP << " port: " << mpCoreConfig->getNetworkConfig().lanConfig.hostReplicaPort);
		RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(mpCoreConfig->getNetworkConfig().lanConfig.hostReplicaIP.c_str(), atoi(mpCoreConfig->getNetworkConfig().lanConfig.hostReplicaPort.c_str()), 0, 0);
		if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			APE_LOG_DEBUG("Failed connect call to " << mpCoreConfig->getNetworkConfig().lanConfig.hostReplicaPort << ". Code=" << car);
		}
		else
		{
			APE_LOG_DEBUG("Connection attempt was successful to remote system " << mpCoreConfig->getNetworkConfig().lanConfig.hostReplicaPort);
		}
	}
}

void ape::SceneNetworkImpl::leave()
{
	
}

void ape::SceneNetworkImpl::destroy()
{
}

void ape::SceneNetworkImpl::createReplicaHost()
{
	APE_LOG_DEBUG("Listening....");
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
		mpNatPunchthroughClient->FindRouterPortStride(mNATServerAddress);
}

bool ape::SceneNetworkImpl::isReplicaHost()
{
	return mIsReplicaHost;
}

std::string ape::SceneNetworkImpl::getGUID()
{
	return mGuid.ToString();
}

std::weak_ptr<RakNet::ReplicaManager3> ape::SceneNetworkImpl::getReplicaManager()
{
	return mpReplicaManager3;
}

ape::SceneNetwork::ParticipantType ape::SceneNetworkImpl::getParticipantType()
{
	return mParticipantType;
}

bool ape::SceneNetworkImpl::isRoomRunning(std::string roomName)
{
	std::string guid;
	return mpLobbyManager->getSessionHostGuid(roomName, guid);
}

void ape::SceneNetworkImpl::connectToRoom(std::string roomName, std::vector<std::string> configURLs, std::vector<std::string> configLocations)
{
	mParticipantType = ape::SceneNetwork::GUEST;
	mLobbyServerSessionName = roomName;
	mIsReplicaHost = false;
	init();
	std::string uuid;
	if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
	{
		if (configURLs.size() && configLocations.size())
		{
			APE_LOG_DEBUG("use lobbyManager to update the configs...");
			for (int i = 0; i < configURLs.size(); i++)
			{
				mpLobbyManager->downloadConfig(configURLs[i], configLocations[i]);
			}
		}
		APE_LOG_DEBUG("use lobbyManager to get scene session guid");
		bool getSessionRes = mpLobbyManager->getSessionHostGuid(roomName, uuid);
		APE_LOG_DEBUG("lobbyManager->getSessionHostGuid() res: " << getSessionRes << " uuid: " << uuid);
	}
	connect2ReplicaHost(uuid);
}

void ape::SceneNetworkImpl::downloadConfigs(std::vector<std::string> configURLs, std::vector<std::string> configLocations)
{
	if (configURLs.size() && configLocations.size())
	{
		APE_LOG_DEBUG("use lobbyManager to update the configs...");
		for (int i = 0; i < configURLs.size(); i++)
		{
			mpLobbyManager->downloadConfig(configURLs[i], configLocations[i]);
		}
	}
}

std::string ape::SceneNetworkImpl::getCurrentRoomName()
{
	return mLobbyServerSessionName;
}


void ape::SceneNetworkImpl::runReplicaPeerListen()
{
	while (true)
	{
		listenReplicaPeer();
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
}

void ape::SceneNetworkImpl::listenReplicaPeer()
{
	RakNet::Packet *packet;
	for (packet = mpRakReplicaPeer->Receive(); packet; mpRakReplicaPeer->DeallocatePacket(packet), packet = mpRakReplicaPeer->Receive())
	{
		switch (packet->data[0])
		{
			case ID_NEW_INCOMING_CONNECTION:
				{
					if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
					{
						APE_LOG_DEBUG("ID_NEW_INCOMING_CONNECTION from " << packet->systemAddress.ToString() << " guid: " << packet->guid.ToString());
					}
					else if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::LAN)
					{
						if (mParticipantType == ape::SceneNetwork::HOST)
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
			case ID_DISCONNECTION_NOTIFICATION:
				APE_LOG_DEBUG("ID_DISCONNECTION_NOTIFICATION");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					APE_LOG_DEBUG("ID_CONNECTION_REQUEST_ACCEPTED from " << packet->systemAddress.ToString(true) << ", guid=" << packet->guid.ToString() << ", participantType=" << mParticipantType);
					if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
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
						else if (mParticipantType == ape::SceneNetwork::ParticipantType::HOST)
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
					else if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::LAN)
					{
						if (mParticipantType == ape::SceneNetwork::ParticipantType::HOST)
						{
							mpRakReplicaPeer->GetConnectionState(packet->systemAddress);
							RakNet::ConnectionAttemptResult car = mpRakReplicaPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
							if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
							{
								APE_LOG_DEBUG("Failed connect call to " << packet->systemAddress.ToString(true) << ". Code=" << car);
							}
							else
							{
								APE_LOG_DEBUG("Connection success from remote system " << packet->systemAddress.ToString(true));
							}
						}
						else if (mParticipantType == ape::SceneNetwork::ParticipantType::GUEST)
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
					mIsNATPunchthrough2HostResponded = true;
					mIsNATPunchthrough2HostSucceeded = false;
					APE_LOG_DEBUG("NAT punch to: " << packet->guid.ToString() << " failed. Reason: " << RakNet::PacketLogger::BaseIDTOString(packet->data[0]));
					break;
				}
			case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
				{
					mIsNATPunchthrough2HostResponded = true;
					mIsNATPunchthrough2HostSucceeded = true;
					if (mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::INTERNET)
					{
						unsigned char weAreTheSender = packet->data[1];
						if (mParticipantType == ape::SceneNetwork::ParticipantType::HOST)
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
						else if (mParticipantType == ape::SceneNetwork::ParticipantType::GUEST)
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
				}
				break;
			case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
				{
					if (mpReplicaManager3->GetAllConnectionDownloadsCompleted() == true)
					{
						APE_LOG_DEBUG("Completed all remote downloads");
						if (!mIsStreamHost && mpCoreConfig->getNetworkConfig().selected == ape::NetworkConfig::Selected::LAN)
						{
							APE_LOG_DEBUG("Try to connect to stream host: " << mpCoreConfig->getNetworkConfig().lanConfig.hostStreamIP << "|" << mpCoreConfig->getNetworkConfig().lanConfig.hostStreamPort);
							RakNet::ConnectionAttemptResult car = mpRakStreamPeer->Connect(mpCoreConfig->getNetworkConfig().lanConfig.hostStreamIP.c_str(), atoi(mpCoreConfig->getNetworkConfig().lanConfig.hostStreamPort.c_str()), 0, 0);
							APE_LOG_DEBUG("Raknet Stream Connection to Host Result: " << (int)car);
							RakAssert(car == RakNet::RAKNET_STARTED);
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

