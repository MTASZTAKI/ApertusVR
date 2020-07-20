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
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "apeCoreConfigImpl.h"
#ifdef ANDROID
#include "../../androidx/assetOpen/apeAAssetOpen.h"
#endif

ape::CoreConfigImpl::CoreConfigImpl(std::string configFolderPath)
{
	struct stat info;
	msSingleton = this;
	mConfigFolderPath = configFolderPath;
	mNetworkGUID = std::string();

#ifndef ANDROID
	if (stat(mConfigFolderPath.c_str(), &info) != 0)
	{
		std::cout << "CoreConfigImpl: cannot access to " << mConfigFolderPath << std::endl;
	}
	else if (info.st_mode & S_IFDIR)
	{
		std::cout << "CoreConfigImpl: loading config files from " << mConfigFolderPath << std::endl;
	}
	else
	{
		std::cout << "CoreConfigImpl: no directory at " << mConfigFolderPath << std::endl;
	}
#endif

	std::stringstream fileFullPath; 
	fileFullPath << mConfigFolderPath << "/apeCore.json";

#ifndef __ANDROID__
	FILE* apeCoreConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
#else
	//FILE* apeCoreConfigFile = android_fopen(fileFullPath.str().c_str(),"r");
	FILE* apeCoreConfigFile = ape::AAssetOpen::open(fileFullPath.str().c_str(),"r");
#endif

	char readBuffer[65536];
	if (apeCoreConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeCoreConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& network = jsonDocument["network"];
			for (rapidjson::Value::MemberIterator networkMemberIterator =
				network.MemberBegin();
				networkMemberIterator != network.MemberEnd(); ++networkMemberIterator)
			{
				if (networkMemberIterator->name == "user")
					mNetworkConfig.userName = jsonDocument["network"]["user"].GetString();
				else if (networkMemberIterator->name == "participant")
				{
					std::string participant = jsonDocument["network"]["participant"].GetString();
					if (participant == "" || participant == "none")
					{
						mNetworkConfig.participant = ape::SceneNetwork::ParticipantType::NONE;
					}
					else if (participant == "guest")
					{
						mNetworkConfig.participant = ape::SceneNetwork::ParticipantType::GUEST;
					}
					else if (participant == "host")
					{
						mNetworkConfig.participant = ape::SceneNetwork::ParticipantType::HOST;
					}
				}
				else if (networkMemberIterator->name == "internet")
				{
					rapidjson::Value& natPunchThrough = jsonDocument["network"]["internet"]["natPunchThrough"];
					for (rapidjson::Value::MemberIterator natPunchThroughMemberIterator =
						natPunchThrough.MemberBegin();
						natPunchThroughMemberIterator != natPunchThrough.MemberEnd(); ++natPunchThroughMemberIterator)
					{
						if (natPunchThroughMemberIterator->name == "ip")
							mNetworkConfig.natPunchThroughConfig.ip = natPunchThroughMemberIterator->value.GetString();
						else if (natPunchThroughMemberIterator->name == "port")
							mNetworkConfig.natPunchThroughConfig.port = natPunchThroughMemberIterator->value.GetString();
					}
					rapidjson::Value& lobby = jsonDocument["network"]["internet"]["lobby"];
					for (rapidjson::Value::MemberIterator lobbyMemberIterator =
						lobby.MemberBegin();
						lobbyMemberIterator != lobby.MemberEnd(); ++lobbyMemberIterator)
					{
						if (lobbyMemberIterator->name == "ip")
							mNetworkConfig.lobbyConfig.ip = lobbyMemberIterator->value.GetString();
						else if (lobbyMemberIterator->name == "port")
							mNetworkConfig.lobbyConfig.port = lobbyMemberIterator->value.GetString();
						else if (lobbyMemberIterator->name == "room")
							mNetworkConfig.lobbyConfig.roomName = lobbyMemberIterator->value.GetString();
					}

				}
				else if (networkMemberIterator->name == "resourceZipUrl")
				{
					mNetworkConfig.resourceZipUrl = networkMemberIterator->value.GetString();
				}
				else if (networkMemberIterator->name == "resourceMd5Url")
				{
					mNetworkConfig.resourceMd5Url = networkMemberIterator->value.GetString();
				}
				else if (networkMemberIterator->name == "resourceDownloadLocation")
				{
					mNetworkConfig.resourceDownloadLocation = networkMemberIterator->value.GetString();
				}
				else if (networkMemberIterator->name == "resourceLocations")
				{
					for (auto& resourceLocation : jsonDocument["network"]["resourceLocations"].GetArray())
					{
						std::string resourceLocationStr = resourceLocation.GetString();
						std::size_t found = resourceLocationStr.find(":");
						if (found != std::string::npos)
						{
							mNetworkConfig.resourceLocations.push_back(resourceLocationStr);
						}
						else
						{
							std::string separator = "../";
							found = resourceLocationStr.find(separator);
							if (found != std::string::npos)
							{
								struct stat info;
								if (stat(resourceLocationStr.c_str(), &info) != 0)
								{
									auto found_it = std::find_end(resourceLocationStr.begin(), resourceLocationStr.end(), separator.begin(), separator.end());
									size_t foundPos = found_it - resourceLocationStr.begin();
									std::stringstream resourceLocationPath;
									resourceLocationPath << APE_SOURCE_DIR << resourceLocationStr.substr(foundPos + 2);
									mNetworkConfig.resourceLocations.push_back(resourceLocationPath.str());
								}
								else if (info.st_mode & S_IFDIR)
								{
									mNetworkConfig.resourceLocations.push_back(resourceLocationStr);
								}
								else
								{
									auto found_it = std::find_end(resourceLocationStr.begin(), resourceLocationStr.end(), separator.begin(), separator.end());
									size_t foundPos = found_it - resourceLocationStr.begin();
									std::stringstream resourceLocationPath;
									resourceLocationPath << APE_SOURCE_DIR << resourceLocationStr.substr(foundPos + 2);
									mNetworkConfig.resourceLocations.push_back(resourceLocationPath.str());
								}
							}
							else
							{
								std::stringstream resourceLocationPath;
								resourceLocationPath << APE_SOURCE_DIR << resourceLocation.GetString();
								mNetworkConfig.resourceLocations.push_back(resourceLocationPath.str());
							}
						}
					}
				}
				else if (networkMemberIterator->name == "lan")
				{
					rapidjson::Value& lan = jsonDocument["network"]["lan"];
					for (rapidjson::Value::MemberIterator lanMemberIterator =
						lan.MemberBegin();
						lanMemberIterator != lan.MemberEnd(); ++lanMemberIterator)
					{
						if (lanMemberIterator->name == "hostReplicaIP")
							mNetworkConfig.lanConfig.hostReplicaIP = lanMemberIterator->value.GetString();
						else if (lanMemberIterator->name == "hostReplicaPort")
							mNetworkConfig.lanConfig.hostReplicaPort = lanMemberIterator->value.GetString();
						else if (lanMemberIterator->name == "hostStreamIP")
							mNetworkConfig.lanConfig.hostStreamIP = lanMemberIterator->value.GetString();
						else if (lanMemberIterator->name == "hostStreamPort")
							mNetworkConfig.lanConfig.hostStreamPort = lanMemberIterator->value.GetString();
					}

				}
				else if (networkMemberIterator->name == "selected")
				{
					std::string selectedNetwork = jsonDocument["network"]["selected"].GetString();
					if (selectedNetwork == "" || selectedNetwork == "none")
					{
						mNetworkConfig.selected = ape::NetworkConfig::Selected::NONE;
					}
					else if (selectedNetwork == "internet")
					{
						mNetworkConfig.selected = ape::NetworkConfig::Selected::INTERNET;
					}
					else if (selectedNetwork == "lan")
					{
						mNetworkConfig.selected = ape::NetworkConfig::Selected::LAN;
					}
				}
			}
			rapidjson::Value& plugins = jsonDocument["plugins"];
			for (auto& plugin : jsonDocument["plugins"].GetArray())
			{
				mPluginNames.push_back(plugin.GetString());
			}
		}
		fclose(apeCoreConfigFile);
	}
}

ape::CoreConfigImpl::~CoreConfigImpl()
{

}

ape::NetworkConfig ape::CoreConfigImpl::getNetworkConfig()
{
	return mNetworkConfig;
}

ape::WindowConfig ape::CoreConfigImpl::getWindowConfig()
{
	return mWindowConfig;
}

void ape::CoreConfigImpl::setWindowConfig(WindowConfig windowConfig)
{
	mWindowConfig = windowConfig;
}

void ape::CoreConfigImpl::setNetworkGUID(std::string networkGUID)
{
	mNetworkGUID = networkGUID;
}

std::string ape::CoreConfigImpl::getNetworkGUID()
{
	return mNetworkGUID;
}

std::vector<std::string> ape::CoreConfigImpl::getPluginNames()
{
	return mPluginNames;
}

std::string ape::CoreConfigImpl::getConfigFolderPath()
{
	return mConfigFolderPath;
}






