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

void createUserBodyNodes(std::string uniqueUserNodeName)
{
	if (auto userNode = gpSceneImpl->createNode(uniqueUserNodeName).lock())
	{
		if (auto hipNode = gpSceneImpl->createNode(uniqueUserNodeName + "_hip").lock())
		{
			hipNode->setParentNode(userNode);
			if (auto torsoNode = gpSceneImpl->createNode(uniqueUserNodeName + "_torso").lock())
			{
				torsoNode->setParentNode(hipNode);
				if (auto leftShoulderNode = gpSceneImpl->createNode(uniqueUserNodeName + "_leftShoulder").lock())
				{
					leftShoulderNode->setParentNode(torsoNode);
					if (auto leftUpperArmNode = gpSceneImpl->createNode(uniqueUserNodeName + "_leftUpperArm").lock())
					{
						leftUpperArmNode->setParentNode(leftShoulderNode);
						if (auto leftForeArmNode = gpSceneImpl->createNode(uniqueUserNodeName + "_leftForeArm").lock())
						{
							leftForeArmNode->setParentNode(leftUpperArmNode);
							if (auto leftHandNode = gpSceneImpl->createNode(uniqueUserNodeName + "_leftHandNode").lock())
							{
								leftHandNode->setParentNode(leftForeArmNode);
							}
						}
					}
				}
				if (auto rightShoulderNode = gpSceneImpl->createNode(uniqueUserNodeName + "_rightShoulder").lock())
				{
					rightShoulderNode->setParentNode(torsoNode);
					if (auto rightUpperArmNode = gpSceneImpl->createNode(uniqueUserNodeName + "_rightUpperArm").lock())
					{
						rightUpperArmNode->setParentNode(rightShoulderNode);
						if (auto rightForeArmNode = gpSceneImpl->createNode(uniqueUserNodeName + "_rightForeArm").lock())
						{
							rightForeArmNode->setParentNode(rightUpperArmNode);
							if (auto rightHandNode = gpSceneImpl->createNode(uniqueUserNodeName + "_rightHandNode").lock())
							{
								rightHandNode->setParentNode(rightForeArmNode);
							}
						}
					}
				}
				if (auto neckNode = gpSceneImpl->createNode(uniqueUserNodeName + "_neck").lock())
				{
					neckNode->setParentNode(torsoNode);
					if (auto headNode = gpSceneImpl->createNode(uniqueUserNodeName + "_head").lock())
					{
						headNode->setParentNode(neckNode);
					}
				}
			}
		}
	}
}

void Ape::System::Start(std::string configFolderPath, bool isBlockingMode)
{
	gpSystemConfigImpl = new SystemConfigImpl(configFolderPath);
	gpMainWindowImpl = new MainWindowImpl();
	gpEventManagerImpl = new EventManagerImpl();
	gpSceneSessionImpl = new SceneSessionImpl();
	gpSceneImpl = new SceneImpl();

	std::stringstream uniqueUserNodeName;
	uniqueUserNodeName << gpSystemConfigImpl->getSceneSessionConfig().uniqueUserNamePrefix << "_" << gpSceneSessionImpl->getGUID();
	gpSystemConfigImpl->setGeneratedUniqueUserNodeName(uniqueUserNodeName.str());
	gpSystemConfigImpl->writeSessionGUID(gpSceneSessionImpl->getGUID());

	createUserBodyNodes(uniqueUserNodeName.str());
	
	if (gpSystemConfigImpl->getMainWindowConfig().creator == "ApeSystem")
		; //TODO open a paltform specific window

	gpPluginManagerImpl = new PluginManagerImpl();
	gpPluginManagerImpl->CreatePlugins();
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

