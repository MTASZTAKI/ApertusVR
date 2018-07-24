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

#ifndef APE_MYPLUGIN_H
#define APE_MYPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <fstream>


#include "Ape.h"
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"

#define THIS_PLUGINNAME "ApeMyPlugin"

namespace Ape
{
	class MyPlugin : public Ape::IPlugin
	{
	public:
		MyPlugin();

		~MyPlugin();

		void Init() override;

		void Run() override;

	private:
		
		Ape::IScene* mpScene;

		Ape::IEventManager* mpEventManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::MyPluginConfig mMyPluginConfig;

		void eventCallBack(const Ape::Event& event);

	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateMyPlugin()
	{
		return new MyPlugin;
	}

	APE_PLUGIN_FUNC void DestroyMyPlugin(Ape::IPlugin *plugin)
	{
		delete (MyPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateMyPlugin, DestroyMyPlugin);
		return 0;
	}
}

#endif
