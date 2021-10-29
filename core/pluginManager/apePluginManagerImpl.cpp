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

#include "apePluginManagerImpl.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

ape::PluginManagerImpl::PluginManagerImpl()
{
	msSingleton = this;
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mThreadVector = std::vector<std::thread>();
	mPluginMap = std::map<std::string,ape::IPlugin*>();
	mPluginCount = 0;
}

ape::PluginManagerImpl::~PluginManagerImpl()
{
	
}

void ape::PluginManagerImpl::loadPlugin(std::string name)
{
	if (mpInternalPluginManager->Load(name))
	{
		ape::IPlugin* plugin = ape::PluginFactory::CreatePlugin(name);
		mPluginMap[name] = plugin;
		mPluginCount++;
		APE_LOG_DEBUG("Plugin loaded: " << name);
		std::thread pluginThread = std::thread(&PluginManagerImpl::InitAndRunPlugin, this, plugin);
		//TODO detach or join based on the ape::System::Start isBlocked param
		//mThreadVector.push_back(pluginThread);
		pluginThread.detach();
	}
	else
	{
		APE_LOG_ERROR("Can not load plugin: " << name);
	}
}

void ape::PluginManagerImpl::stopPlugin(std::string name)
{
	mPluginMap[name]->Stop();
	ape::PluginFactory::UnregisterPlugin(name, mPluginMap[name]);
	mPluginMap.erase(name);
	mPluginCount--;
}

void ape::PluginManagerImpl::CreatePlugin(std::string pluginname)
{
	mPluginMap[pluginname]= ape::PluginFactory::CreatePlugin(pluginname);
}

void ape::PluginManagerImpl::CreatePlugins()
{
	mpInternalPluginManager = &ape::InternalPluginManager::GetInstance();
	auto pluginNames = mpCoreConfig->getPluginNames();
	mPluginCount = pluginNames.size();
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

void ape::PluginManagerImpl::callStepFunc(){
    for (auto const& plugin : mPluginMap)
    {
            plugin.second->Step();
    }
    
}
//callStepFunc
//for plugin : step


//delete
//void ape::PluginManagerImpl::runMainThreadFunctions(){
//    for (auto const& plugin : mPluginVector)
//    {
//        if(plugin->mMainThreadFunction)
//            plugin->mMainThreadFunction();
//    }
//}

void ape::PluginManagerImpl::InitAndRunPlugin(ape::IPlugin* plugin)
{
	plugin->Init();
	plugin->Run();
	//TODO_CORE name
	//ape::PluginFactory::UnregisterPlugin(pluginname, plugin);
}

void ape::PluginManagerImpl::StopPlugins()
{
	for (auto const& plugin : mPluginMap)
	{
		plugin.second->Stop();
	}
}

void ape::PluginManagerImpl::InitAndRunPlugins()
{
	for (std::map<std::string, ape::IPlugin*>::iterator it = mPluginMap.begin(); it != mPluginMap.end(); ++it)
	{
		mThreadVector.push_back(std::thread(&PluginManagerImpl::InitAndRunPlugin, this, (it->second)));
	}
}

void ape::PluginManagerImpl::registerUserThreadFunction(std::function<void()> userThreadFunction)
{
	mThreadVector.push_back(std::thread(userThreadFunction));
}

void ape::PluginManagerImpl::joinThreads()
{
	std::for_each(mThreadVector.begin(), mThreadVector.end(), std::mem_fn(&std::thread::join));
}

void ape::PluginManagerImpl::detachThreads()
{
	std::for_each(mThreadVector.begin(), mThreadVector.end(), std::mem_fn(&std::thread::detach));
}

unsigned int ape::PluginManagerImpl::getPluginCount()
{
	return mPluginCount;
}


