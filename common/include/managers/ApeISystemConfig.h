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

namespace Ape
{
	struct PluginManagerConfig
	{
		std::vector<std::string> pluginnames;
		std::vector<std::string> jsPluginNames;

		PluginManagerConfig()
		{
			this->pluginnames = std::vector<std::string>();
			this->jsPluginNames = std::vector<std::string>();
		}

		PluginManagerConfig(
		    std::vector<std::string> pluginnames,
		    std::vector<std::string> jsPluginNames
		)
		{
			this->pluginnames = pluginnames;
			this->jsPluginNames = jsPluginNames;
		}
	};

	struct SceneSessionConfig
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
			bool useLobby;

			LobbyServerConfig()
			{
				this->ip = std::string();
				this->port = std::string();
				this->sessionName = std::string();
				this->useLobby = false;
			}

			LobbyServerConfig(
			    std::string ip,
			    std::string port,
			    std::string sessionName = "",
			    bool useLobby = false
			)
			{
				this->ip = ip;
				this->port = port;
				this->sessionName = sessionName;
				this->useLobby = useLobby;
			}
		};

		SceneSession::ParticipantType participantType;

		NatPunchThroughServerConfig natPunchThroughServerConfig;

		LobbyServerConfig lobbyServerConfig;

		std::string uniqueUserNamePrefix;

		std::vector<std::string> sessionResourceLocation;

		std::string sessionGUID;

		std::string sessionIP;

		std::string sessionPort;

		SceneSessionConfig()
		{
			this->natPunchThroughServerConfig = NatPunchThroughServerConfig();
			this->lobbyServerConfig = LobbyServerConfig();
			this->participantType = SceneSession::ParticipantType::INVALID;
			this->sessionGUID = std::string();
			this->sessionIP = std::string();
			this->sessionPort = std::string();
			this->uniqueUserNamePrefix = std::string();
			this->sessionResourceLocation = std::vector<std::string>();
		}

		SceneSessionConfig(
		    NatPunchThroughServerConfig natPunchThroughServerConfig,
		    LobbyServerConfig lobbyServerConfig,
		    SceneSession::ParticipantType participantType,
		    std::string sessionName,
		    std::string uniqueUserNamePrefix,
		    std::string generatedUniqueUserName,
		    std::vector<std::string> sessionResourceLocation,
			std::string sessionIP,
			std::string sessionPort
		)
		{
			this->natPunchThroughServerConfig = natPunchThroughServerConfig;
			this->lobbyServerConfig = lobbyServerConfig;
			this->participantType = participantType;
			this->sessionGUID = sessionName;
			this->sessionIP = sessionIP;
			this->sessionPort = sessionPort;
			this->uniqueUserNamePrefix = uniqueUserNamePrefix;
			this->sessionResourceLocation = sessionResourceLocation;
		}
	};

	struct MainWindowConfig
	{
		std::string name;

		std::string creator;

		MainWindowConfig()
		{
			this->name = std::string();
			this->creator = std::string();
		}

		MainWindowConfig(
		    std::string name,
		    std::string creator
		)
		{
			this->name = name;
			this->creator = creator;
		}
	};

	class APE_SYSTEMCONFIG_DLL_EXPORT ISystemConfig : public Singleton<ISystemConfig>
	{
	protected:
		virtual ~ISystemConfig() {};

	public:
		virtual PluginManagerConfig getPluginManagerConfig() = 0;

		virtual SceneSessionConfig getSceneSessionConfig() = 0;

		virtual MainWindowConfig getMainWindowConfig() = 0;

		virtual std::string getFolderPath() = 0;
	};
}
#endif
