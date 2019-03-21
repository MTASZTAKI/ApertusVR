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

#include "ApePluginManagerImpl.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::PluginManagerImpl::PluginManagerImpl()
{
	msSingleton = this;
	mpSystemConfig = ape::ISystemConfig::getSingletonPtr();
	mPluginThreadVector = std::vector<std::thread>();
	mPluginVector = std::vector<ape::IPlugin*>();
	mPluginCount = 0;
}

ape::PluginManagerImpl::~PluginManagerImpl()
{
	
}

void ape::PluginManagerImpl::CreatePlugin(std::string pluginname)
{
	mPluginVector.push_back(ape::PluginFactory::CreatePlugin(pluginname));
}

void ape::PluginManagerImpl::CreatePlugins()
{
	mpInternalPluginManager = &ape::InternalPluginManager::GetInstance();
	ape::PluginManagerConfig pluginManagerConfig = mpSystemConfig->getPluginManagerConfig();
	std::vector<std::string> pluginNames = pluginManagerConfig.pluginnames;
	mPluginCount = pluginManagerConfig.pluginnames.size();
	for (std::vector<std::string>::iterator it = pluginNames.begin(); it != pluginNames.end(); ++it)
	{
		if (mpInternalPluginManager->Load((*it)))
		{
			CreatePlugin(*it);
			APE_LOG_DEBUG("Plugin loaded: " << *it);
		}
		else
		{
			mPluginCount--;
			APE_LOG_ERROR("Can not load plugin: " << *it);
		}
	}
}

void ape::PluginManagerImpl::InitAndRunPlugin(ape::IPlugin* plugin)
{
	plugin->Init();
	plugin->Run();
	//TODO_CORE name
	//ape::PluginFactory::UnregisterPlugin(pluginname, plugin);
}

void ape::PluginManagerImpl::InitAndRunPlugins()
{
	for (std::vector<ape::IPlugin*>::iterator it = mPluginVector.begin(); it != mPluginVector.end(); ++it)
	{
		mPluginThreadVector.push_back(std::thread(&PluginManagerImpl::InitAndRunPlugin, this, (*it)));
	}
}

void ape::PluginManagerImpl::joinPluginThreads()
{
	std::for_each(mPluginThreadVector.begin(), mPluginThreadVector.end(), std::mem_fn(&std::thread::join));
}

void ape::PluginManagerImpl::detachPluginThreads()
{
	std::for_each(mPluginThreadVector.begin(), mPluginThreadVector.end(), std::mem_fn(&std::thread::detach));
}


