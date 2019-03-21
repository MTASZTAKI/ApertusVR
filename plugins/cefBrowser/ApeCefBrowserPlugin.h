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

namespace ape
{
	class CefBrowserPlugin : public ape::IPlugin
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

		ape::ISceneManager* mpSceneManager;

		ape::ISystemConfig* mpSystemConfig;

		ape::IEventManager* mpEventManager;

		ape::CefRenderHandlerImpl* mpApeCefRenderHandlerImpl;

		ape::CefLifeSpanHandlerImpl* mpApeCefLifeSpanHandlerImpl;

		ape::CefKeyboardHandlerImpl* mpApeCefKeyboardHandlerImpl;
		
		int mBrowserCounter;

		ape::NodeWeakPtr mRayOverlayNode;

		CefRefPtr<ape::CefClientImpl> mApeCefClientImpl;

		bool mCefIsInintialzed;

		CefBrowserSettings mBrowserSettings;

		ape::DoubleQueue<Event> mEventDoubleQueue;

		std::map<std::string, int> mBrowserIDNames;

		void processEventDoubleQueue();

		void processEvent(ape::Event event);

		void eventCallBack(const ape::Event& event);

		void createBrowser(ape::BrowserSharedPtr browser);
	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateCefBrowserPlugin()
	{
		return new ape::CefBrowserPlugin;
	}

	APE_PLUGIN_FUNC void DestroyCefBrowserPlugin(ape::IPlugin *plugin)
	{
		delete (ape::CefBrowserPlugin*)plugin;
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
