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

#include <iostream>
#include <string>
#include "ApeNodeJsPlugin.h"
#include "node.h"

using namespace Ape;

ApeNodeJsPlugin::ApeNodeJsPlugin()
{
	std::cout << "ApeNodeJsPlugin::ApeNodeJsPlugin" << std::endl;

	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeNodeJsPlugin::nodeEventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeNodeJsPlugin::~ApeNodeJsPlugin()
{

}

void ApeNodeJsPlugin::nodeEventCallBack(const Ape::Event& event)
{
	/*std::cout << "ApeNodeJsPlugin::nodeEventCallBack: event.subjectName: " << event.subjectName << std::endl;
	std::cout << "ApeNodeJsPlugin::nodeEventCallBack: event.group: " << event.group << std::endl;*/
}

void ApeNodeJsPlugin::Init()
{
	std::cout << "ApeHelloWorldPlugin::init" << std::endl;
}

void ApeNodeJsPlugin::Run()
{
	std::cout << "ApeNodeJsPlugin::run" << std::endl;

	char *args[] = { "", "server.js" };
	std::cout << "Initializing Node..." << std::endl;

	int res = -1;
	try
	{
		res = node::Start(2, args);
	}
	catch (...)
	{
		std::cout << "Exception catched from NodeJS" << std::endl;
	}
	std::cout << "Node server exited with code " << res << std::endl;
	std::getchar();
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
