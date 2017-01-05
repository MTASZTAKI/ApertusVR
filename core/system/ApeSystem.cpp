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

#include "ApeSystem.h"
#include "ApeSystemConfigImpl.h"
#include "ApeMainWindowImpl.h"
#include "ApePluginManagerImpl.h"
#include "ApeEventManagerImpl.h"
#include "ApeSceneImpl.h"
#include "ApeSceneSessionImpl.h"
#include "ApeINode.h"
#include "ApeITextGeometry.h"

Ape::PluginManagerImpl* gpPluginManagerImpl;
Ape::EventManagerImpl* gpEventManagerImpl;
Ape::SceneImpl* gpSceneImpl;
Ape::SceneSessionImpl* gpSceneSessionImpl;
Ape::SystemConfigImpl* gpSystemConfigImpl;
Ape::MainWindowImpl* gpMainWindowImpl;

void Ape::System::Start(std::string configFolderPath, bool isBlockingMode)
{
	gpSystemConfigImpl = new SystemConfigImpl(configFolderPath);
	gpMainWindowImpl = new MainWindowImpl();
	gpEventManagerImpl = new EventManagerImpl();
	gpSceneSessionImpl = new SceneSessionImpl();
	gpSceneImpl = new SceneImpl();

	if (gpSystemConfigImpl->getMainWindowConfig().creator == "ApeSystem")
		; //TODO open a paltform specific window

	gpPluginManagerImpl = new PluginManagerImpl();
	gpPluginManagerImpl->LoadPlugins();
	std::stringstream generatedUniqueUserName;
	generatedUniqueUserName << gpSystemConfigImpl->getSceneSessionConfig().uniqueUserNamePrefix << "_" << gpSceneSessionImpl->getGUID();
	gpSystemConfigImpl->setGeneratedUniqueUserName(generatedUniqueUserName.str());
	std::string userNodeName = gpSystemConfigImpl->getSceneSessionConfig().generatedUniqueUserName;
	auto userNode = gpSceneImpl->createNode(userNodeName);
	if (gpSystemConfigImpl->getSceneSessionConfig().participantType == SceneSession::ParticipantType::HOST || gpSystemConfigImpl->getSceneSessionConfig().participantType == SceneSession::ParticipantType::GUEST)
	{
		if (userNode.lock())
		{
			if (auto userNameText = std::static_pointer_cast<Ape::ITextGeometry>(gpSceneImpl->createEntity(userNodeName, Ape::Entity::GEOMETRY_TEXT).lock()))
			{
				userNameText->setCaption(userNodeName);
				userNameText->setOffset(Ape::Vector3(0.0f, 1.0f, 0.0f));
				userNameText->setParentNode(userNode);
			}
		}
		gpSystemConfigImpl->writeSessionGUID(gpSceneSessionImpl->getGUID());
	}
	if (isBlockingMode)
		gpPluginManagerImpl->joinPluginThreads();
	else
		gpPluginManagerImpl->detachPluginThreads();
}

void Ape::System::Stop()
{
	delete gpEventManagerImpl;
	delete gpSceneImpl;
	delete gpSceneSessionImpl;
	delete gpPluginManagerImpl;
	delete gpSystemConfigImpl;
	delete gpMainWindowImpl;
}

