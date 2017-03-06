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
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeIScene.h"
#include "ApeINode.h"
//#include "ApeIJsEventManager.h"

#include <map>
#include <vector>
#include "ApeEvent.h"

class ApeNodeJsPlugin : public Ape::IPlugin
{
private:
	Ape::IEventManager* mpEventManager;

	Ape::IScene* mpScene;

	//Ape::IJsEventManager* mpJsEventManager;

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

APE_PLUGIN_DISPLAY_NAME("ApeNodeJsPlugin");

APE_PLUGIN_ALLOC()
{
	std::cout << "ApeNodeJsPlugin_CREATE" << std::endl;
	ApeRegisterPlugin("ApeNodeJsPlugin", CreateApeNodeJsPlugin, DestroyApeNodeJsPlugin);
	std::cout << "ApeRegisterPlugin ApeNodeJsPlugin" << std::endl;
	return 0;
}

#endif
