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


#ifndef APE_ICORECONFIG_H
#define APE_ICORECONFIG_H

#ifdef _WIN32
	#ifdef BUILDING_APE_CORECONFIG_DLL
		#define APE_CORECONFIG_DLL_EXPORT __declspec(dllexport)
	#else
		#define APE_CORECONFIG_DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define APE_CORECONFIG_DLL_EXPORT
#endif

#include <string>
#include <vector>
#include "apeISceneNetwork.h"
#include "apeSingleton.h"

namespace ape
{
	struct NetworkConfig
	{
		enum Selected
		{
			INTERNET,
			LAN,
			NONE,
			INVALID
		};

		struct NatPunchThroughConfig
		{
			std::string ip;

			std::string port;

			NatPunchThroughConfig()
			{
				this->ip = std::string();
				this->port = std::string();
			}

			NatPunchThroughConfig(
			    std::string ip,
			    std::string port
			)
			{
				this->ip = ip;
				this->port = port;
			}
		};

		struct LobbyConfig
		{
			std::string ip;
			std::string port;
			std::string roomName;

			LobbyConfig()
			{
				this->ip = std::string();
				this->port = std::string();
				this->roomName = std::string();
			}

			LobbyConfig(
			    std::string ip,
			    std::string port,
			    std::string roomName = ""
			)
			{
				this->ip = ip;
				this->port = port;
				this->roomName = roomName;
			}
		};

		struct LanConfig
		{
			std::string hostReplicaIP;

			std::string hostReplicaPort;

			std::string hostStreamIP;

			std::string hostStreamPort;

			LanConfig()
			{
				this->hostReplicaIP = std::string();
				this->hostReplicaPort = std::string();
				this->hostStreamIP = std::string();
				this->hostStreamPort = std::string();
			}

			LanConfig(
				std::string hostReplicaIP,
				std::string hostReplicaPort,
				std::string hostStreamIP,
				std::string hostStreamPort
			)
			{
				this->hostReplicaIP = hostReplicaIP;
				this->hostReplicaPort = hostReplicaPort;
				this->hostStreamIP = hostStreamIP;
				this->hostStreamPort = hostStreamPort;
			}
		};

		std::string userName;

		Selected selected;

		NatPunchThroughConfig natPunchThroughConfig;

		LanConfig lanConfig;

		std::string resourceZipUrl;

		std::string resourceMd5Url;

		std::string resourceDownloadLocation;

		std::vector<std::string> resourceLocations;

		LobbyConfig lobbyConfig;

		SceneNetwork::ParticipantType participant;

		NetworkConfig()
		{
			this->userName = std::string();
			this->natPunchThroughConfig = NatPunchThroughConfig();
			this->lanConfig = LanConfig();
			this->resourceZipUrl = std::string();
			this->resourceMd5Url = std::string();
			this->resourceDownloadLocation = std::string();
			this->resourceLocations = std::vector<std::string>();
			this->lobbyConfig = LobbyConfig();
			this->selected = Selected::INVALID;
			this->participant = SceneNetwork::ParticipantType::INVALID;
		}

		NetworkConfig(
			std::string userName,
			NatPunchThroughConfig natPunchThroughConfig,
			LanConfig lanConfig,
			std::string resourceZipUrl,
		    std::string resourceMd5Url,
			std::string resourceDownloadLocation,
			std::vector<std::string> resourceLocations,
			LobbyConfig lobbyConfig,
			Selected selected,
			SceneNetwork::ParticipantType participant
		)
		{
			this->userName = userName;
			this->natPunchThroughConfig = natPunchThroughConfig;
			this->lanConfig = lanConfig;
			this->resourceZipUrl = resourceZipUrl;
			this->resourceMd5Url = resourceMd5Url;
			this->resourceDownloadLocation = resourceDownloadLocation;
			this->resourceLocations = resourceLocations;
			this->lobbyConfig = lobbyConfig;
			this->selected = selected;
			this->participant = participant;
		}
	};

	struct WindowConfig
	{
		std::string name;

		std::string renderSystem;

		void* handle;

		void* device;

		unsigned int width;

		unsigned int height;

		WindowConfig()
		{
			this->name = std::string();
			this->renderSystem = std::string();
			this->handle = nullptr;
			this->device = nullptr;
			this->width = 0u;
			this->height = 0u;
		}

		WindowConfig(
		    std::string name,
			std::string renderSystem,
			void* handle,
			void* device,
			unsigned int width,
			unsigned int height
		)
		{
			this->name = name;
			this->renderSystem = renderSystem;
			this->handle = handle;
			this->device = device;
			this->width = width;
			this->height = height;
		}
	};

	class APE_CORECONFIG_DLL_EXPORT ICoreConfig : public Singleton<ICoreConfig>
	{
	protected:
		virtual ~ICoreConfig() {};

	public:
		virtual std::vector<std::string> getPluginNames() = 0;

		virtual NetworkConfig getNetworkConfig() = 0;

		virtual WindowConfig getWindowConfig() = 0;

		virtual void setWindowConfig(WindowConfig windowConfig) = 0;

		virtual void setNetworkGUID(std::string networkGUID) = 0;

		virtual std::string getNetworkGUID() = 0;

		virtual std::string getConfigFolderPath() = 0;
	};
}
#endif
