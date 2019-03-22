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
#include "apeNodeJsPlugin.h"
#include "node.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

apeNodeJsPlugin::apeNodeJsPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeNodeJsPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

apeNodeJsPlugin::~apeNodeJsPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeNodeJsPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void apeNodeJsPlugin::eventCallBack(const ape::Event& event)
{

}

void apeNodeJsPlugin::parseNodeJsConfig()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeNodeJsPlugin.json";
	FILE* configFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (configFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(configFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject() && jsonDocument.HasMember("httpServer"))
		{
			rapidjson::Value& httpServer = jsonDocument["httpServer"];
			if (httpServer.IsObject() && httpServer.HasMember("port"))
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
	APE_LOG_FUNC_LEAVE();
}

void apeNodeJsPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	parseNodeJsConfig();
	APE_LOG_FUNC_LEAVE();
}

void apeNodeJsPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream port;
	port << mNodeJsPluginConfig.serverPort;
	char *args[] = { "", "server.js", (char*)port.str().c_str() };
	APE_LOG_DEBUG("Initializing Node...");

	int res = -1;
	try
	{
		res = node::Start(3, args);
	}
	catch (...)
	{
		APE_LOG_ERROR("Exception catched from NodeJS");
	}

	APE_LOG_DEBUG("Node server exited with code " << res);
	std::getchar();
	APE_LOG_FUNC_LEAVE();
}

void apeNodeJsPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void apeNodeJsPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void apeNodeJsPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void apeNodeJsPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
