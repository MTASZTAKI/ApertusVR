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
#include "ApePluginManagerImpl.h"

Ape::PluginManagerImpl::PluginManagerImpl()
{
	msSingleton = this;
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mUniqueUserNodeName = mpSystemConfig->getSceneSessionConfig().generatedUniqueUserNodeName;
	mPluginThreadVector = std::vector<std::thread>();
	mPluginVector = std::vector<Ape::IPlugin*>();
	mConstructedPluginCount = 0;
	mPluginCount = 0;
	mpScene = Ape::IScene::getSingletonPtr();
	isCreateUserBodyNodesFunctionCalled = false;
}

Ape::PluginManagerImpl::~PluginManagerImpl()
{
	
}

void Ape::PluginManagerImpl::CreatePlugin(std::string pluginname)
{
	Ape::IPlugin* plugin = Ape::PluginFactory::CreatePlugin(pluginname);
	mConstructedPluginCount++;
	while (mConstructedPluginCount < mPluginCount)
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	if (!isCreateUserBodyNodesFunctionCalled)
	{
		isCreateUserBodyNodesFunctionCalled = true;
		createUserBodyNodes();
	}
	plugin->Init();
	plugin->Run();
	Ape::PluginFactory::UnregisterPlugin(pluginname, plugin);
}

void Ape::PluginManagerImpl::CreatePlugins()
{
	mpInternalPluginManager = &Ape::InternalPluginManager::GetInstance();
	Ape::PluginManagerConfig pluginManagerConfig = mpSystemConfig->getPluginManagerConfig();
	std::vector<std::string> pluginNames = pluginManagerConfig.pluginnames;
	mPluginCount = pluginManagerConfig.pluginnames.size();
	for (std::vector<std::string>::iterator it = pluginNames.begin(); it != pluginNames.end(); ++it)
	{
		if (mpInternalPluginManager->Load((*it)))
			mPluginThreadVector.push_back(std::thread(&PluginManagerImpl::CreatePlugin, this, (*it)));
	}
}

void Ape::PluginManagerImpl::joinPluginThreads()
{
	std::for_each(mPluginThreadVector.begin(), mPluginThreadVector.end(), std::mem_fn(&std::thread::join));
}

void Ape::PluginManagerImpl::detachPluginThreads()
{
	std::for_each(mPluginThreadVector.begin(), mPluginThreadVector.end(), std::mem_fn(&std::thread::detach));
}

void Ape::PluginManagerImpl::LoadPlugin(std::string name)
{
	//TODO;
}

void Ape::PluginManagerImpl::createUserBodyNodes()
{
	if (auto userNode = mpScene->createNode(mUniqueUserNodeName).lock())
	{
		if (auto hipNode = mpScene->createNode(mUniqueUserNodeName + "_hip").lock())
		{
			hipNode->setParentNode(userNode);
			if (auto torsoNode = mpScene->createNode(mUniqueUserNodeName + "_torso").lock())
			{
				torsoNode->setParentNode(hipNode);
				if (auto leftShoulderNode = mpScene->createNode(mUniqueUserNodeName + "_leftShoulder").lock())
				{
					leftShoulderNode->setParentNode(torsoNode);
					if (auto leftUpperArmNode = mpScene->createNode(mUniqueUserNodeName + "_leftUpperArm").lock())
					{
						leftUpperArmNode->setParentNode(leftShoulderNode);
						if (auto leftForeArmNode = mpScene->createNode(mUniqueUserNodeName + "_leftForeArm").lock())
						{
							leftForeArmNode->setParentNode(leftUpperArmNode);
							if (auto leftHandNode = mpScene->createNode(mUniqueUserNodeName + "_leftHandNode").lock())
							{
								leftHandNode->setParentNode(leftForeArmNode);
							}
						}
					}
				}
				if (auto rightShoulderNode = mpScene->createNode(mUniqueUserNodeName + "_rightShoulder").lock())
				{
					rightShoulderNode->setParentNode(torsoNode);
					if (auto rightUpperArmNode = mpScene->createNode(mUniqueUserNodeName + "_rightUpperArm").lock())
					{
						rightUpperArmNode->setParentNode(rightShoulderNode);
						if (auto rightForeArmNode = mpScene->createNode(mUniqueUserNodeName + "_rightForeArm").lock())
						{
							rightForeArmNode->setParentNode(rightUpperArmNode);
							if (auto rightHandNode = mpScene->createNode(mUniqueUserNodeName + "_rightHandNode").lock())
							{
								rightHandNode->setParentNode(rightForeArmNode);
							}
						}
					}
				}
				if (auto neckNode = mpScene->createNode(mUniqueUserNodeName + "_neck").lock())
				{
					neckNode->setParentNode(torsoNode);
					if (auto headNode = mpScene->createNode(mUniqueUserNodeName + "_head").lock())
					{
						headNode->setParentNode(neckNode);
					}
				}
			}
		}
	}
}

