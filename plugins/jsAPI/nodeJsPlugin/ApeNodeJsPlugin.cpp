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

#include <iostream>
#include <string>
#include <fstream>
#include "ApeNodeJsPlugin.h"
#include "node.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ApeNodeJsPlugin::ApeNodeJsPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeNodeJsPlugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

ApeNodeJsPlugin::~ApeNodeJsPlugin()
{

}

void ApeNodeJsPlugin::nodeEventCallBack(const Ape::Event& event)
{

}

void ApeNodeJsPlugin::parseNodeJsConfig()
{
	LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "/ApeNodeJsPlugin.json";
	FILE* configFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (configFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(configFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& httpServer = jsonDocument["httpServer"];
			if (httpServer.IsObject())
			{
				rapidjson::Value& port = httpServer["port"];
				if (port.IsNumber())
				{
					mNodeJsPluginConfig.serverPort = port.GetInt();
				}
			}
		}
		fclose(configFile);
	}
	LOG_FUNC_LEAVE();
}

void ApeNodeJsPlugin::Init()
{
	LOG_FUNC_ENTER();
	parseNodeJsConfig();
	LOG_FUNC_LEAVE();
}

void ApeNodeJsPlugin::Run()
{
	LOG_FUNC_ENTER();
	std::stringstream port;
	port << mNodeJsPluginConfig.serverPort;
	char *args[] = { "", "server.js", (char*)port.str().c_str() };
	LOG(LOG_TYPE_DEBUG, "Initializing Node...");

	int res = -1;
	try
	{
		res = node::Start(3, args);
	}
	catch (...)
	{
		LOG(LOG_TYPE_ERROR, "Exception catched from NodeJS");
	}

	LOG(LOG_TYPE_DEBUG, "Node server exited with code " << res);
	std::getchar();
	LOG_FUNC_LEAVE();
}

void ApeNodeJsPlugin::Step()
{

}

void ApeNodeJsPlugin::Stop()
{

}

void ApeNodeJsPlugin::Suspend()
{

}

void ApeNodeJsPlugin::Restart()
{

}
