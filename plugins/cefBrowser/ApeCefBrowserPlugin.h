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

#ifndef APE_CEFBROWSERPLUGIN_H
#define APE_CEFBROWSERPLUGIN_H

#include <iostream>
#include <string>
#include <thread>
#include "system/ApeIMainWindow.h"
#include "plugin/ApeIPlugin.h"
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "managers/ApeISystemConfig.h"
#include "datatypes/ApeGeometry.h"
#include "sceneelements/ApeIBrowser.h"
#include "sceneelements/ApeIFileGeometry.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeIManualPass.h"
#include "sceneelements/ApeIManualTexture.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeIPlaneGeometry.h"
#include "sceneelements/ApeIPlaneGeometry.h"
#include "sceneelements/ApeIRayGeometry.h"
#include "sceneelements/ApeITextGeometry.h"
#include "utils/ApeDoubleQueue.h"
#include "cef_app.h"
#include "cef_client.h"
#include "cef_render_handler.h"
#include "views/cef_browser_view.h"
#include "ApeCefRenderHandlerImpl.h"
#include "ApeCefLifeSpanHandlerImpl.h"
#include "ApeCefKeyboardHandlerImpl.h"
#include "ApeCefClientImpl.h"

#define THIS_PLUGINNAME "ApeCefBrowserPlugin"

namespace Ape
{
	class CefBrowserPlugin : public Ape::IPlugin
	{
	public:
		CefBrowserPlugin();

		~CefBrowserPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

	private:

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IEventManager* mpEventManager;

		Ape::CefRenderHandlerImpl* mpApeCefRenderHandlerImpl;

		Ape::CefLifeSpanHandlerImpl* mpApeCefLifeSpanHandlerImpl;

		Ape::CefKeyboardHandlerImpl* mpApeCefKeyboardHandlerImpl;
		
		int mBrowserCounter;

		Ape::NodeWeakPtr mRayOverlayNode;

		CefRefPtr<Ape::CefClientImpl> mApeCefClientImpl;

		bool mCefIsInintialzed;

		CefBrowserSettings mBrowserSettings;

		Ape::DoubleQueue<Event> mEventDoubleQueue;

		std::map<std::string, int> mBrowserIDNames;

		void processEventDoubleQueue();

		void processEvent(Ape::Event event);

		void eventCallBack(const Ape::Event& event);

		void createBrowser(Ape::BrowserSharedPtr browser);
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateCefBrowserPlugin()
	{
		return new Ape::CefBrowserPlugin;
	}

	APE_PLUGIN_FUNC void DestroyCefBrowserPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::CefBrowserPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateCefBrowserPlugin, DestroyCefBrowserPlugin);
		return 0;
	}
}

#endif
