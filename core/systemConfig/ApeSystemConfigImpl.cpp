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


#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "ApeSystemConfigImpl.h"

template<> Ape::ISystemConfig* Ape::Singleton<Ape::ISystemConfig>::msSingleton = 0;

Ape::SystemConfigImpl::SystemConfigImpl(std::string folderPath)
{
	msSingleton = this;
	mFolderPath = folderPath;
	std::stringstream fileFullPath; 
	fileFullPath << folderPath << "\\ApeSystem.json";
	FILE* apeSystemConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeSystemConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeSystemConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& mainWindow = jsonDocument["mainWindow"];
			for (rapidjson::Value::MemberIterator mainWindowMemberIterator =
				mainWindow.MemberBegin();
				mainWindowMemberIterator != mainWindow.MemberEnd(); ++mainWindowMemberIterator)
			{
				if (mainWindowMemberIterator->name == "name")
					mMainWindowConfig.name = mainWindowMemberIterator->value.GetString();
				else if (mainWindowMemberIterator->name == "creator")
					mMainWindowConfig.creator = mainWindowMemberIterator->value.GetString();
			}
			rapidjson::Value& sceneSession = jsonDocument["sceneSession"];
			for (rapidjson::Value::MemberIterator sceneSessionMemberIterator =
				sceneSession.MemberBegin();
				sceneSessionMemberIterator != sceneSession.MemberEnd(); ++sceneSessionMemberIterator)
			{
				if (sceneSessionMemberIterator->name == "natPunchThroughServer")
				{
					rapidjson::Value& natPunchThroughServer = jsonDocument["sceneSession"]["natPunchThroughServer"];
					for (rapidjson::Value::MemberIterator natPunchThroughServerMemberIterator =
						natPunchThroughServer.MemberBegin();
						natPunchThroughServerMemberIterator != natPunchThroughServer.MemberEnd(); ++natPunchThroughServerMemberIterator)
					{
						if (natPunchThroughServerMemberIterator->name == "ip")
							mSceneSessionConfig.natPunchThroughServerConfig.ip = natPunchThroughServerMemberIterator->value.GetString();
						else if (natPunchThroughServerMemberIterator->name == "port")
							mSceneSessionConfig.natPunchThroughServerConfig.port = natPunchThroughServerMemberIterator->value.GetString();
					}

				}
				else if (sceneSessionMemberIterator->name == "lobbyServer")
				{
					rapidjson::Value& lobbyServer = jsonDocument["sceneSession"]["lobbyServer"];
					for (rapidjson::Value::MemberIterator lobbyServerMemberIterator =
						lobbyServer.MemberBegin();
						lobbyServerMemberIterator != lobbyServer.MemberEnd(); ++lobbyServerMemberIterator)
					{
						if (lobbyServerMemberIterator->name == "ip")
							mSceneSessionConfig.lobbyServerConfig.ip = lobbyServerMemberIterator->value.GetString();
						else if (lobbyServerMemberIterator->name == "port")
							mSceneSessionConfig.lobbyServerConfig.port = lobbyServerMemberIterator->value.GetString();
					}

				}
				else if (sceneSessionMemberIterator->name == "participantType")
				{
					std::string participantType = jsonDocument["sceneSession"]["participantType"].GetString();
					if (participantType == "Host")
						mSceneSessionConfig.participantType = SceneSession::ParticipantType::HOST;
					else if (participantType == "Guest")
						mSceneSessionConfig.participantType = SceneSession::ParticipantType::GUEST;
					else if (participantType == "Local")
						mSceneSessionConfig.participantType = SceneSession::ParticipantType::LOCAL;
					else
						mSceneSessionConfig.participantType = SceneSession::ParticipantType::INVALID;
				}
				else if (sceneSessionMemberIterator->name == "uniqueUserNamePrefix")
					mSceneSessionConfig.uniqueUserNamePrefix = jsonDocument["sceneSession"]["uniqueUserNamePrefix"].GetString();
				else if (sceneSessionMemberIterator->name == "sessionGUID")
					mSceneSessionConfig.sessionGUID = jsonDocument["sceneSession"]["sessionGUID"].GetString();
				else if (sceneSessionMemberIterator->name == "sessionResourceLocation")
				{
					for (auto& resourceLocation : jsonDocument["sceneSession"]["sessionResourceLocation"].GetArray())
					{
						std::stringstream sessionResourceLocation;
						sessionResourceLocation << APE_SOURCE_DIR << resourceLocation.GetString();
						mSceneSessionConfig.sessionResourceLocation.push_back(sessionResourceLocation.str());
					}
				}
			}
			rapidjson::Value& pluginManager = jsonDocument["pluginManager"];
			for (rapidjson::Value::MemberIterator pluginManagerMemberIterator =
				pluginManager.MemberBegin();
				pluginManagerMemberIterator != pluginManager.MemberEnd(); ++pluginManagerMemberIterator)
			{
				if (pluginManagerMemberIterator->name == "plugins")
				{
					for (auto& plugin : pluginManager["plugins"].GetArray())
						mPluginManagerConfig.pluginnames.push_back(plugin.GetString());
				}
			}
		}
		fclose(apeSystemConfigFile);
	}
}

Ape::SystemConfigImpl::~SystemConfigImpl()
{

}

Ape::SceneSessionConfig Ape::SystemConfigImpl::getSceneSessionConfig()
{
	return mSceneSessionConfig;
}

Ape::MainWindowConfig Ape::SystemConfigImpl::getMainWindowConfig()
{
	return mMainWindowConfig;
}

Ape::PluginManagerConfig Ape::SystemConfigImpl::getPluginManagerConfig()
{
	return mPluginManagerConfig;
}

std::string Ape::SystemConfigImpl::getFolderPath()
{
	return mFolderPath;
}

void Ape::SystemConfigImpl::setGeneratedUniqueUserNodeName(std::string generatedUniqueUserName)
{
	mSceneSessionConfig.generatedUniqueUserNodeName = generatedUniqueUserName;
}

void Ape::SystemConfigImpl::writeSessionGUID(Ape::SceneSessionUniqueID sessionGUID)
{
	std::stringstream fileFullPath;
	fileFullPath << mFolderPath << "\\ApeSystem.json";

	FILE* apeSystemConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	rapidjson::Document jsonDocument;
	if (apeSystemConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeSystemConfigFile, readBuffer, sizeof(readBuffer));
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& sceneSession = jsonDocument["sceneSession"];
			for (rapidjson::Value::MemberIterator sceneSessionMemberIterator =
				sceneSession.MemberBegin();
				sceneSessionMemberIterator != sceneSession.MemberEnd(); ++sceneSessionMemberIterator)
			{
				if (sceneSessionMemberIterator->name == "sessionGUID")
					jsonDocument["sceneSession"]["sessionGUID"].SetString(rapidjson::StringRef(sessionGUID.c_str()));
			}
		}
		fclose(apeSystemConfigFile);
	}

	rapidjson::StringBuffer writeBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(writeBuffer);
	jsonDocument.Accept(writer);

	std::stringstream contentSS;
	contentSS << writeBuffer.GetString() << std::endl;
	std::string content = contentSS.str();
	std::ofstream apeSystemConfigFileOut(fileFullPath.str().c_str(), std::ios::binary | std::ios::out);
	apeSystemConfigFileOut.write(content.c_str(), content.size());
	apeSystemConfigFileOut.flush();
	apeSystemConfigFileOut.close();
}





