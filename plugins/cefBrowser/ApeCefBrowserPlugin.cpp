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

#include "ApeCefBrowserPlugin.h"

Ape::CefBrowserPlugin::CefBrowserPlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpApeCefRenderHandlerImpl = nullptr;
	mApeCefClientImpl = nullptr;
	mCefBrowser = nullptr;
	mCefBrowserView = nullptr;
	mCefIsInintialzed = false;
	mCefWindowInfo = CefWindowInfo();
	mBrowserSettings = CefBrowserSettings();
	mpEventManager->connectEvent(Ape::Event::Group::CAMERA, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
}

Ape::CefBrowserPlugin::~CefBrowserPlugin()
{
	std::cout << "ApeCefBrowserPlugin dtor" << std::endl;
	mCefBrowser = nullptr;
	mApeCefClientImpl = nullptr;
	delete mpApeCefRenderHandlerImpl;
}

void Ape::CefBrowserPlugin::eventCallBack(const Ape::Event& event)
{
	if (event.type == Ape::Event::Type::CAMERA_CREATE)
	{
		mCefWindowInfo.SetAsWindowless(0);
		CefBrowserHost::CreateBrowser(mCefWindowInfo, mApeCefClientImpl.get(), "http://google.hu", mBrowserSettings, nullptr);
	}
}

void Ape::CefBrowserPlugin::Init()
{
	std::cout << "ApeCefBrowserPlugin::Init" << std::endl;
	CefSettings settings;
	CefString(&settings.browser_subprocess_path).FromASCII("ApeCefSubProcessApp.exe");
#if defined(OS_WIN)
	CefMainArgs main_args(::GetModuleHandle(0));
#endif
	if (CefInitialize(main_args, settings, nullptr, nullptr))
	{
		mpApeCefRenderHandlerImpl = new Ape::CefRenderHandlerImpl();
		mApeCefClientImpl = new Ape::CefClientImpl(mpApeCefRenderHandlerImpl);
		mCefIsInintialzed = true;
	}
}

void Ape::CefBrowserPlugin::Run()
{
	if (mCefIsInintialzed)
	{
		CefRunMessageLoop();
		CefShutdown();
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::CAMERA, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
}

void Ape::CefBrowserPlugin::Step()
{
	
}

void Ape::CefBrowserPlugin::Stop()
{
	
}

void Ape::CefBrowserPlugin::Suspend()
{
	
}

void Ape::CefBrowserPlugin::Restart()
{
	
}