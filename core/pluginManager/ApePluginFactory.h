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


#ifndef APE_PLUGINFACTORY_H
#define APE_PLUGINFACTORY_H

#include "ApePluginDefines.h"
#include "ApeIPlugin.h"

#include <string>
#include <map>

namespace Ape
{
	class APE_PLUGIN_INTERNAL PluginFactory
	{
	public:
		typedef IPlugin* (*CreateCallback)();
		
		typedef void (*DestroyCallback)(Ape::IPlugin *);

		static void RegisterPlugin(const std::string &type, CreateCallback createCb, DestroyCallback destroyCb);

		static void UnregisterPlugin(const std::string &type, Ape::IPlugin *plugin);

		static IPlugin* CreatePlugin(const std::string &type);
		
	private:
		struct PluginCallbacks {
			CreateCallback createCb;
			DestroyCallback destroyCb;
		};

		typedef std::map<std::string, PluginCallbacks> CallbackMap;

		static CallbackMap mPluginCallbackMap;
	};
}

#endif
