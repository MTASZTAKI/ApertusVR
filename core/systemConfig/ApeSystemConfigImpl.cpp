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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include "ApeSystemConfigImpl.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

ape::SystemConfigImpl::SystemConfigImpl(std::string configFolderPath)
{
	struct stat info;
	msSingleton = this;
	mConfigFolderPath = configFolderPath;

	if (stat(mConfigFolderPath.c_str(), &info) != 0)
	{
		std::cout << "SystemConfigImpl: cannot access to " << mConfigFolderPath << std::endl;
	}
	else if (info.st_mode & S_IFDIR)
	{
		std::cout << "SystemConfigImpl: loading config files from " << mConfigFolderPath << std::endl;
	}
	else
	{
		std::cout << "SystemConfigImpl: no directory at " << mConfigFolderPath << std::endl;
	}

	std::stringstream fileFullPath; 
	fileFullPath << mConfigFolderPath << "/ApeSystem.json";
	FILE* apeSystemConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeSystemConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeSystemConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& session = jsonDocument["session"];
			for (rapidjson::Value::MemberIterator sessionMemberIterator =
				session.MemberBegin();
				sessionMemberIterator != session.MemberEnd(); ++sessionMemberIterator)
			{
				if (sessionMemberIterator->name == "participantType")
				{
					std::string participantType = jsonDocument["session"]["participantType"].GetString();
					if (participantType == "Host")
						mSessionConfig.participantType = ape::SceneSession::ParticipantType::HOST;
					else if (participantType == "Guest")
						mSessionConfig.participantType = ape::SceneSession::ParticipantType::GUEST;
					else if (participantType == "Local")
						mSessionConfig.participantType = ape::SceneSession::ParticipantType::LOCAL;
					else
						mSessionConfig.participantType = ape::SceneSession::ParticipantType::INVALID;
				}
				else if (sessionMemberIterator->name == "userName")
					mSessionConfig.userName = jsonDocument["session"]["userName"].GetString();
				else if (sessionMemberIterator->name == "natPunchThroughServer")
				{
					rapidjson::Value& natPunchThroughServer = jsonDocument["session"]["natPunchThroughServer"];
					for (rapidjson::Value::MemberIterator natPunchThroughServerMemberIterator =
						natPunchThroughServer.MemberBegin();
						natPunchThroughServerMemberIterator != natPunchThroughServer.MemberEnd(); ++natPunchThroughServerMemberIterator)
					{
						if (natPunchThroughServerMemberIterator->name == "use")
							mSessionConfig.natPunchThroughServerConfig.use = natPunchThroughServerMemberIterator->value.GetBool();
						else if (natPunchThroughServerMemberIterator->name == "ip")
							mSessionConfig.natPunchThroughServerConfig.ip = natPunchThroughServerMemberIterator->value.GetString();
						else if (natPunchThroughServerMemberIterator->name == "port")
							mSessionConfig.natPunchThroughServerConfig.port = natPunchThroughServerMemberIterator->value.GetString();
					}

				}
				else if (sessionMemberIterator->name == "lobbyServer")
				{
					rapidjson::Value& lobbyServer = jsonDocument["session"]["lobbyServer"];
					for (rapidjson::Value::MemberIterator lobbyServerMemberIterator =
						lobbyServer.MemberBegin();
						lobbyServerMemberIterator != lobbyServer.MemberEnd(); ++lobbyServerMemberIterator)
					{
						if (lobbyServerMemberIterator->name == "ip")
							mSessionConfig.lobbyServerConfig.ip = lobbyServerMemberIterator->value.GetString();
						else if (lobbyServerMemberIterator->name == "port")
							mSessionConfig.lobbyServerConfig.port = lobbyServerMemberIterator->value.GetString();
						else if (lobbyServerMemberIterator->name == "sessionName")
							mSessionConfig.lobbyServerConfig.sessionName = lobbyServerMemberIterator->value.GetString();
						else if (lobbyServerMemberIterator->name == "use")
							mSessionConfig.lobbyServerConfig.use = lobbyServerMemberIterator->value.GetBool();
					}
				}
				else if (sessionMemberIterator->name == "resourceLocations")
				{
					for (auto& resourceLocation : jsonDocument["session"]["resourceLocations"].GetArray())
					{
						std::stringstream resourceLocationPath;
						resourceLocationPath << APE_SOURCE_DIR << resourceLocation.GetString();
						mSessionConfig.resourceLocations.push_back(resourceLocationPath.str());
					}
				}
				else if (sessionMemberIterator->name == "localNetwork")
				{
					rapidjson::Value& natPunchThroughServer = jsonDocument["session"]["localNetwork"];
					for (rapidjson::Value::MemberIterator natPunchThroughServerMemberIterator =
						natPunchThroughServer.MemberBegin();
						natPunchThroughServerMemberIterator != natPunchThroughServer.MemberEnd(); ++natPunchThroughServerMemberIterator)
					{
						if (natPunchThroughServerMemberIterator->name == "use")
							mSessionConfig.natPunchThroughServerConfig.use = natPunchThroughServerMemberIterator->value.GetBool();
						else if (natPunchThroughServerMemberIterator->name == "ip")
							mSessionConfig.natPunchThroughServerConfig.ip = natPunchThroughServerMemberIterator->value.GetString();
						else if (natPunchThroughServerMemberIterator->name == "port")
							mSessionConfig.natPunchThroughServerConfig.port = natPunchThroughServerMemberIterator->value.GetString();
					}

				}
			}
			rapidjson::Value& plugins = jsonDocument["plugins"];
			for (auto& plugin : jsonDocument["plugins"].GetArray())
			{
				mPluginNames.push_back(plugin.GetString());
			}
		}
		fclose(apeSystemConfigFile);
	}
}

ape::SystemConfigImpl::~SystemConfigImpl()
{

}

ape::SessionConfig ape::SystemConfigImpl::getSessionConfig()
{
	return mSessionConfig;
}

ape::WindowConfig& ape::SystemConfigImpl::getWindowConfig()
{
	return mWindowConfig;
}

std::vector<std::string> ape::SystemConfigImpl::getPluginNames()
{
	return mPluginNames;
}

std::string ape::SystemConfigImpl::getConfigFolderPath()
{
	return mConfigFolderPath;
}






