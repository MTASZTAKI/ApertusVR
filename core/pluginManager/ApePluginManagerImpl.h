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
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>
#include <thread>
#include "managers/ApeILogManager.h"
#include "managers/ApeIPluginManager.h"
#include "managers/ApeISystemConfig.h"
#include "ApeInternalPluginManager.h"
#include "ApePluginFactory.h"

namespace Ape
{ 
	class APE_PLUGINMANAGER_DLL_EXPORT PluginManagerImpl : public IPluginManager
	{	
	private:
		std::vector<std::thread> mPluginThreadVector;

		std::vector<Ape::IPlugin*> mPluginVector;
		
		InternalPluginManager* mpInternalPluginManager;

		void CreatePlugin(std::string pluginname);

		void InitAndRunPlugin(Ape::IPlugin* plugin);

		Ape::ISystemConfig* mpSystemConfig;

		unsigned int mPluginCount;

	public:
		PluginManagerImpl();

		~PluginManagerImpl();

		void CreatePlugins();

		void InitAndRunPlugins();

		void joinPluginThreads();

		void detachPluginThreads();
	};
}
#endif
