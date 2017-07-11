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

template<> Ape::IPluginManager* Ape::Singleton<Ape::IPluginManager>::msSingleton = 0;

Ape::PluginManagerImpl::PluginManagerImpl()
{
	msSingleton = this;
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mPluginThreadVector = std::vector<std::thread>();
	mPluginVector = std::vector<Ape::IPlugin*>();
	mConstructedPluginCount = 0;
	mPluginCount = 0;
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

