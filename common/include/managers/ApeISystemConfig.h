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


#ifndef APE_ISYSTEMCONFIG_H
#define APE_ISYSTEMCONFIG_H

#ifdef _WIN32
	#ifdef BUILDING_APE_SYSTEMCONFIG_DLL
		#define APE_SYSTEMCONFIG_DLL_EXPORT __declspec(dllexport)
	#else
		#define APE_SYSTEMCONFIG_DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define APE_SYSTEMCONFIG_DLL_EXPORT
#endif

#include <string>
#include <vector>
#include "managers/ApeISceneSession.h"
#include "utils/ApeSingleton.h"

namespace ape
{
	struct SessionConfig
	{
		struct NatPunchThroughServerConfig
		{
			bool use;

			std::string ip;

			std::string port;

			NatPunchThroughServerConfig()
			{
				this->use = true;
				this->ip = std::string();
				this->port = std::string();
			}

			NatPunchThroughServerConfig(
				bool use,
			    std::string ip,
			    std::string port
			)
			{
				this->use = use;
				this->ip = ip;
				this->port = port;
			}
		};

		struct LobbyServerConfig
		{
			std::string ip;
			std::string port;
			std::string sessionName;
			bool use;

			LobbyServerConfig()
			{
				this->ip = std::string();
				this->port = std::string();
				this->sessionName = std::string();
				this->use = false;
			}

			LobbyServerConfig(
			    std::string ip,
			    std::string port,
			    std::string sessionName = "",
			    bool use = false
			)
			{
				this->ip = ip;
				this->port = port;
				this->sessionName = sessionName;
				this->use = use;
			}
		};

		struct LocalNetworkConfig
		{
			bool use;

			std::string ip;

			std::string port;

			LocalNetworkConfig()
			{
				this->use = true;
				this->ip = std::string();
				this->port = std::string();
			}

			LocalNetworkConfig(
				bool use,
				std::string ip,
				std::string port
			)
			{
				this->use = use;
				this->ip = ip;
				this->port = port;
			}
		};

		SceneSession::ParticipantType participantType;

		std::string userName;

		NatPunchThroughServerConfig natPunchThroughServerConfig;

		LocalNetworkConfig localNetworkConfig;

		std::vector<std::string> resourceLocations;

		LobbyServerConfig lobbyServerConfig;

		SessionConfig()
		{
			this->participantType = SceneSession::ParticipantType::INVALID;
			this->userName = std::string();
			this->natPunchThroughServerConfig = NatPunchThroughServerConfig();
			this->localNetworkConfig = LocalNetworkConfig();
			this->resourceLocations = std::vector<std::string>();
			this->lobbyServerConfig = LobbyServerConfig();
		}

		SessionConfig(
			SceneSession::ParticipantType participantType,
			std::string userName,
			NatPunchThroughServerConfig natPunchThroughServerConfig,
			LocalNetworkConfig localNetwrokConfig,
			std::vector<std::string> resourceLocations,
			LobbyServerConfig lobbyServerConfig
		)
		{
			this->participantType = participantType;
			this->userName = userName;
			this->natPunchThroughServerConfig = natPunchThroughServerConfig;
			this->localNetworkConfig = localNetwrokConfig;
			this->resourceLocations = resourceLocations;
			this->lobbyServerConfig = lobbyServerConfig;
		}
	};

	struct WindowConfig
	{
		std::string name;

		void* handle;

		unsigned int width;

		unsigned int height;

		WindowConfig()
		{
			this->name = std::string();
			this->handle = nullptr;
			this->width = unsigned int();
			this->height = unsigned int();
		}

		WindowConfig(
		    std::string name,
			void* handle,
			unsigned int width,
			unsigned int height
		)
		{
			this->name = name;
			this->handle = handle;
			this->width = width;
			this->height = height;
		}
	};

	class APE_SYSTEMCONFIG_DLL_EXPORT ISystemConfig : public Singleton<ISystemConfig>
	{
	protected:
		virtual ~ISystemConfig() {};

	public:
		virtual std::vector<std::string> getPluginNames() = 0;

		virtual SessionConfig getSessionConfig() = 0;

		virtual WindowConfig& getWindowConfig() = 0;

		virtual std::string getConfigFolderPath() = 0;
	};
}
#endif
