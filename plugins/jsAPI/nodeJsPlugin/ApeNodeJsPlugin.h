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

#ifndef APE_NODEJSPLUGIN_H
#define APE_NODEJSPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <map>
#include <vector>
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeEvent.h"

#define THIS_PLUGINNAME "ApeNodeJsPlugin"

class ApeNodeJsPlugin : public Ape::IPlugin
{
private:
	Ape::IEventManager* mpEventManager;

	Ape::IScene* mpScene;

	Ape::NodeWeakPtr mNodeWeakPtr;

	void nodeEventCallBack(const Ape::Event& event);

public:
	ApeNodeJsPlugin();

	~ApeNodeJsPlugin();

	void Init() override;

	void Run() override;

	void Step() override;

	void Stop() override;

	void Suspend() override;

	void Restart() override;
};

APE_PLUGIN_FUNC Ape::IPlugin* CreateApeNodeJsPlugin()
{
	return new ApeNodeJsPlugin;
}

APE_PLUGIN_FUNC void DestroyApeNodeJsPlugin(Ape::IPlugin* ApeNodeJsPlugin)
{
	delete ApeNodeJsPlugin;
}

APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

APE_PLUGIN_ALLOC()
{
	LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
	ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeNodeJsPlugin, DestroyApeNodeJsPlugin);
	return 0;
}

#endif
