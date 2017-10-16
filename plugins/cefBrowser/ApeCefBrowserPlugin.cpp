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
	mBrowserCounter = 0;
	mCefIsInintialzed = false;
	mBrowserSettings = CefBrowserSettings();
	mpEventManager->connectEvent(Ape::Event::Group::BROWSER, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mEventDoubleQueue = Ape::DoubleQueue<Event>();
	mBrowserIDNames = std::map<std::string, int>();
	mRayOverlayNode = Ape::NodeWeakPtr();
}

Ape::CefBrowserPlugin::~CefBrowserPlugin()
{
	std::cout << "ApeCefBrowserPlugin dtor" << std::endl;
	mApeCefClientImpl = nullptr;
	delete mpApeCefRenderHandlerImpl;
}

void Ape::CefBrowserPlugin::processEvent(Ape::Event event)
{
	if (event.group == Ape::Event::Group::BROWSER)
	{
		if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpScene->getEntity(event.subjectName).lock()))
		{
			switch (event.type)
			{
			case Ape::Event::Type::BROWSER_CREATE:
				break;
			case Ape::Event::Type::BROWSER_GEOMETRY:
				{
					createBrowser(browser);
				}
				break;
			case Ape::Event::Type::BROWSER_OVERLAY:
				{
					createBrowser(browser);
				}
				break;
			case Ape::Event::Type::BROWSER_ZOOM:
				{
					if (mBrowserIDNames[browser->getName()])
						mpApeCefRenderHandlerImpl->setZoomLevel(mBrowserIDNames[browser->getName()], browser->getZoomLevel());
				}
				break;
			case Ape::Event::Type::BROWSER_URL:
				{
					if (mBrowserIDNames[browser->getName()])
						mpApeCefRenderHandlerImpl->setURL(mBrowserIDNames[browser->getName()], browser->getURL());
				}
				break;
			case Ape::Event::Type::BROWSER_DELETE:
				;
				break;
			}
		}
	}
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_PARENTNODE)
	{
		if (auto rayGeometry = std::static_pointer_cast<Ape::IRayGeometry>(mpScene->getEntity(event.subjectName).lock()))
			mRayOverlayNode = rayGeometry->getParentNode();
	}
	else if (event.type == Ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY)
	{
		if (auto rayOverlayNode = mRayOverlayNode.lock())
			rayOverlayNode->getPosition();//send it to cef
	}
}

void Ape::CefBrowserPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
	while (!mEventDoubleQueue.emptyPop())
	{
		processEvent(mEventDoubleQueue.front());
		mEventDoubleQueue.pop();
	}
}

void Ape::CefBrowserPlugin::eventCallBack(const Ape::Event& event)
{
	if (!mCefIsInintialzed)
		mEventDoubleQueue.push(event);
	else
		processEvent(event);
}

void Ape::CefBrowserPlugin::createBrowser(Ape::BrowserSharedPtr browser)
{
	std::string browserName = browser->getName();
	if (auto browserMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity(browserName + "_Material", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		browserMaterial->setEmissiveColor(Ape::Color(1.0f, 1.0f, 1.0f));
		if (auto browserTexture = std::static_pointer_cast<Ape::IManualTexture>(mpScene->createEntity(browserName + "_Texture", Ape::Entity::TEXTURE_MANUAL).lock()))
		{
			browserTexture->setParameters(browser->getResoultion().x, browser->getResoultion().y, Ape::Texture::PixelFormat::A8R8G8B8, Ape::Texture::Usage::DYNAMIC_WRITE_ONLY);
			browserMaterial->setPassTexture(browserTexture);
			browserMaterial->setCullingMode(Ape::Material::CullingMode::NONE_CM);
			browserMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
			mBrowserCounter++;
			mpApeCefRenderHandlerImpl->addTexture(mBrowserCounter, browserTexture);
			mBrowserIDNames[browserName] = mBrowserCounter;
			CefWindowInfo cefWindowInfo;
			cefWindowInfo.SetAsWindowless(0);
			CefBrowserHost::CreateBrowser(cefWindowInfo, mApeCefClientImpl.get(), browser->getURL(), mBrowserSettings, nullptr);
		}
		if (auto browserGeometry = browser->getGeometry().lock())
			std::static_pointer_cast<Ape::IPlaneGeometry>(browserGeometry)->setMaterial(browserMaterial);
		else
			browserMaterial->showOnOverlay(true, browser->getZOrder());
	}
}

void Ape::CefBrowserPlugin::Init()
{
	std::cout << "ApeCefBrowserPlugin::Init" << std::endl;
	CefSettings settings;
	settings.ignore_certificate_errors = true;
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
		processEventDoubleQueue();
		CefRunMessageLoop();
		CefShutdown();
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::BROWSER, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
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