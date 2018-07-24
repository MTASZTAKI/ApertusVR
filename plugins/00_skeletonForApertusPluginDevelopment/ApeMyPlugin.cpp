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

#include "ApeMyPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"

//#include "ApeOgreUtilities.h"


Ape::MypPlugin::ApeMyPlugin( )			//constructor of the object
{
	mpScene = Ape::IScene::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeMyPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::LIGHT, std::bind(&ApeMyPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&ApeMyPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_TEXT, std::bind(&ApeMyPlugin::eventCallBack, this, std::placeholders::_1));

	mMyPluginConfig = Ape::MyPluginConfig();
	
}

Ape::MyPlugin::~MyPlugin()			//destructor of the object
{
	std::cout << "MyPlugin dtor" << std::endl;
	delete mpRoot;
}

void Ape::MyPlugin::eventCallBack(const Ape::Event& event) 
{
	mEventDoubleQueue.push(event);
}



void Ape::MyPlugin::Run()
{


}



void Ape::MyPlugin::Init() 
{
	if (auto userNode = mpScene->getNode(mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName).lock())
		mUserNode = userNode;

	if (mpSystemConfig->getSceneSessionConfig().participantType == Ape::SceneSession::ParticipantType::HOST || mpSystemConfig->getSceneSessionConfig().participantType == Ape::SceneSession::ParticipantType::GUEST)
	{
		if (auto userNode = mUserNode.lock())
		{
			if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(mpScene->createEntity(userNode->getName(), Ape::Entity::GEOMETRY_TEXT).lock()))
			{
				userNameText->setCaption(userNode->getName());
				userNameText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
				userNameText->setParentNode(userNode);
			}
		}
	}

	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "\configs\ApeMyPlugin.json";
	FILE* apeMyPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeMyPluginConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeMyPluginConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			
		}
		fclose(apeMyPluginConfigFile);
	}	
	
}
