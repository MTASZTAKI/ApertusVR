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
#include "ApeNodeJsPlugin.h"
#include "node.h"

ApeNodeJsPlugin::ApeNodeJsPlugin()
{
	LOG_FUNC_ENTER();
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
	//LOG(LOG_TYPE_DEBUG, "event.subjectName: " << event.subjectName);
	//LOG(LOG_TYPE_DEBUG, "event.group: " << event.group);
}

void ApeNodeJsPlugin::Init()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void ApeNodeJsPlugin::Run()
{
	LOG_FUNC_ENTER();
	char *args[] = { "", "server.js" };
	LOG(LOG_TYPE_DEBUG, "Initializing Node...");

	int res = -1;
	try
	{
		res = node::Start(2, args);
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
