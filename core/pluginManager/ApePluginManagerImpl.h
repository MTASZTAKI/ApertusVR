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

#ifndef APE_PLUGINMANAGERIMPL_H
#define APE_PLUGINMANAGERIMPL_H

#ifdef _WIN32
#ifdef BUILDING_APE_PLUGINMANAGER_DLL
#define APE_PLUGINMANAGER_DLL_EXPORT __declspec(dllexport)
#else
#define APE_PLUGINMANAGER_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_PLUGINMANAGER_DLL_EXPORT 
#endif

#include <algorithm>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "ApeIPluginManager.h"
#include "ApeInternalPluginManager.h"
#include "ApePluginFactory.h"
#include "ApeISystemConfig.h"

namespace Ape
{ 
	class APE_PLUGINMANAGER_DLL_EXPORT PluginManagerImpl : public IPluginManager
	{	
	private:
		std::vector<std::thread> mPluginThreadVector;

		std::vector<Ape::IPlugin*> mPluginVector;
		
		InternalPluginManager* mpInternalPluginManager;

		void CreatePlugin(std::string pluginname);

		Ape::ISystemConfig* mpSystemConfig;

		std::mutex mConstructedPluginMutex;

		std::condition_variable mConstructedPluginCondition;

		unsigned int mConstructedPluginCount;

		unsigned int mPluginCount;

	public:
		PluginManagerImpl();

		~PluginManagerImpl();

		void CreatePlugins();

		void InitPlugins();

		void RunPlugins();

		void DestroyPlugins();

		void joinPluginThreads();

		void detachPluginThreads();

		virtual void LoadPlugin(std::string name) override;
	};
}
#endif
