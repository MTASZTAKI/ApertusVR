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

#ifndef APE_CSHARPPLUGIN_H
#define APE_CSHARPPLUGIN_H

#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <thread>

#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeILogManager.h"
#include "apeICoreConfig.h"
#include "apeISceneManager.h"
#include "apeIEventManager.h"
#include "apeDoubleQueue.h"
#include "apeILight.h"


#define THIS_PLUGINNAME "apeCSharpPlugin"

namespace ape
{
	class CSharpPlugin : public ape::IPlugin
	{
	public:
		CSharpPlugin();

		~CSharpPlugin() {}
		
		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		static CSharpPlugin* getPluginPtr();

		ape::ISceneManager* getSceneManager();

		ape::ICoreConfig* getCoreConfig();

        ape::DoubleQueue<Event>* getEventDoubleQueue();

        std::map<ape::Event::Type, int> getEventNumberMap();

	private:
		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::IEventManager* mpEventManager;

		ape::DoubleQueue<Event> mEventDoubleQueue;

		void eventCallBack(const ape::Event& event);

		void processEventDoubleQueue();

        static CSharpPlugin* mpThisPlugin;

		std::map<ape::Event::Type,int> mEventNumberMap;

		std::map<ape::Event::Type,std::string> mEventTypeNameMap;
    };

	APE_PLUGIN_FUNC ape::IPlugin* CreateCSharpPlugin()
	{
		return new ape::CSharpPlugin;
	}

	APE_PLUGIN_FUNC void DestroyCSharpPlugin(ape::IPlugin *plugin)
	{
		delete (ape::CSharpPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		//APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateCSharpPlugin, DestroyCSharpPlugin);
		return 0;
	}

}

#endif 