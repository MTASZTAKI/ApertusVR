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

#ifndef APE_JNIPLUGIN_H
#define APE_JNIPLUGIN_H

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

#include <jni.h>
#include <android/log.h>


#define THIS_PLUGINNAME "apeJNIPlugin"

namespace ape
{
	class JNIPlugin : public ape::IPlugin
	{
	public:
		JNIPlugin();
		~JNIPlugin() {}
		
		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		static JNIPlugin* getPluginPtr();

		ape::ISceneManager* getSceneManager();

		ape::ICoreConfig* getCoreConfig();

        ape::DoubleQueue<Event>* getEventDoubleQueue();

        std::map<ape::Event::Type, int> getEventNumberMap();

//        std::map<std::string,ape::GeometryWeakPtr>* getNodeGeometryMap();

        const char* NA_STR = "";

	private:
		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::IEventManager* mpEventManager;

		ape::DoubleQueue<Event> mEventDoubleQueue;

		void eventCallBack(const ape::Event& event);

		void processEventDoubleQueue();

        static JNIPlugin* mpThisPlugin;

		std::map<ape::Event::Type,int> mEventNumberMap;

		std::map<ape::Event::Type,std::string> mEventTypeNameMap;

//		std::map<std::string,ape::GeometryWeakPtr> mNodeGeometryMap;

		void initEventNumberMap();

		jclass jEventManagerCls;

		jmethodID jFireEventMethodID;
    };

#ifdef APE_JNIPLUGIN_CPP

	APE_PLUGIN_FUNC ape::IPlugin* CreateJNIPlugin()
	{
		return new ape::JNIPlugin;
	}

	APE_PLUGIN_FUNC void DestroyJNIPlugin(ape::IPlugin *plugin)
	{
		delete (ape::JNIPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		//APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateJNIPlugin, DestroyJNIPlugin);
		return 0;
	}

#endif // !APE_JNIPLUGIN_CPP
}


#endif // !APE_JNIPLUGIN_H