#include "ApeCefBrowserPlugin.h"

Ape::CefBrowserPlugin::CefBrowserPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpApeCefRenderHandlerImpl = nullptr;
	mpApeCefLifeSpanHandlerImpl = nullptr;
	mpApeCefKeyboardHandlerImpl = nullptr;
	mApeCefClientImpl = nullptr;
	mBrowserCounter = 0;
	mCefIsInintialzed = false;
	mBrowserSettings = CefBrowserSettings();
	mpEventManager->connectEvent(Ape::Event::Group::BROWSER, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_RAY, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mEventDoubleQueue = Ape::DoubleQueue<Event>();
	mBrowserIDNames = std::map<std::string, int>();
	mRayOverlayNode = Ape::NodeWeakPtr();
	APE_LOG_FUNC_LEAVE();
}

Ape::CefBrowserPlugin::~CefBrowserPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::BROWSER, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mApeCefClientImpl = nullptr;
	delete mpApeCefRenderHandlerImpl;
	delete mpApeCefLifeSpanHandlerImpl;
	APE_LOG_FUNC_LEAVE();
}

void Ape::CefBrowserPlugin::processEvent(Ape::Event event)
{
	if (event.group == Ape::Event::Group::BROWSER)
	{
		if (auto browser = std::static_pointer_cast<Ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
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
			case Ape::Event::Type::BROWSER_MOUSE_MOVED:
				{
					if (mBrowserIDNames[browser->getName()])
					{
						Ape::Browser::MouseState mouseState = browser->getMouseState();
						mpApeCefRenderHandlerImpl->mouseMoved(mBrowserIDNames[browser->getName()], mouseState.position.x, mouseState.position.y);
					}
				}
				break;
			case Ape::Event::Type::BROWSER_MOUSE_SCROLL:
				{
					if (mBrowserIDNames[browser->getName()])
					{
						Ape::Browser::MouseState mouseState = browser->getMouseState();
						mpApeCefRenderHandlerImpl->mouseScroll(mBrowserIDNames[browser->getName()], mouseState.scrollDelta.x, mouseState.scrollDelta.y);
					}
				}
				break;
			case Ape::Event::Type::BROWSER_MOUSE_CLICK:
				{
					if (mBrowserIDNames[browser->getName()])
					{
						Ape::Browser::MouseState mouseState = browser->getMouseState();
						if (mouseState.click == Ape::Browser::MouseClick::LEFT)
							mpApeCefRenderHandlerImpl->mouseClick(mBrowserIDNames[browser->getName()], mouseState.isClickDown, CefBrowserHost::MouseButtonType::MBT_LEFT);
						else if (mouseState.click == Ape::Browser::MouseClick::RIGHT)
							mpApeCefRenderHandlerImpl->mouseClick(mBrowserIDNames[browser->getName()], mouseState.isClickDown, CefBrowserHost::MouseButtonType::MBT_RIGHT);
						else if (mouseState.click == Ape::Browser::MouseClick::MIDDLE)
							mpApeCefRenderHandlerImpl->mouseClick(mBrowserIDNames[browser->getName()], mouseState.isClickDown, CefBrowserHost::MouseButtonType::MBT_MIDDLE);
					}
				}
				break;
			case Ape::Event::Type::BROWSER_KEY_VALUE:
				{
					if (mBrowserIDNames[browser->getName()])
						mpApeCefRenderHandlerImpl->keyValue(mBrowserIDNames[browser->getName()], browser->getLastKeyASCIIValue());
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
		if (auto rayGeometry = std::static_pointer_cast<Ape::IRayGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			mRayOverlayNode = rayGeometry->getParentNode();
	}
	/*else if (event.type == Ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY)
	{
		if (auto rayOverlayNode = mRayOverlayNode.lock())
			rayOverlayNode->getPosition();
	}*/
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
	if (auto browserMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity(browserName + "_Material", Ape::Entity::MATERIAL_MANUAL).lock()))
	{
		browserMaterial->setEmissiveColor(Ape::Color(1.0f, 1.0f, 1.0f));
		if (auto browserTexture = std::static_pointer_cast<Ape::IManualTexture>(mpSceneManager->createEntity(browserName + "_Texture", Ape::Entity::TEXTURE_MANUAL).lock()))
		{
			browserTexture->setParameters(browser->getResoultion().x, browser->getResoultion().y, Ape::Texture::PixelFormat::A8R8G8B8, Ape::Texture::Usage::DYNAMIC_WRITE_ONLY);
			browserMaterial->setPassTexture(browserTexture);
			browserMaterial->setCullingMode(Ape::Material::CullingMode::CLOCKWISE);
			browserMaterial->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
			mBrowserCounter++;
			mpApeCefLifeSpanHandlerImpl->registerBrowser(mBrowserCounter, browser);
			mpApeCefKeyboardHandlerImpl->registerBrowser(mBrowserCounter, browser);
			mpApeCefRenderHandlerImpl->addTexture(mBrowserCounter, browserTexture);
			mBrowserIDNames[browserName] = mBrowserCounter;
			CefWindowInfo cefWindowInfo;
			cefWindowInfo.SetAsWindowless(0);
			CefBrowserHost::CreateBrowser(cefWindowInfo, mApeCefClientImpl.get(), browser->getURL(), mBrowserSettings, nullptr);
			if (auto browserGeometry = browser->getGeometry().lock())
			{
				if (auto planeGeometry = std::dynamic_pointer_cast<Ape::IPlaneGeometry>(browserGeometry))
				{
					planeGeometry->setMaterial(browserMaterial);
					browserMaterial->setCullingMode(Ape::Material::CullingMode::NONE_CM);
				}
				else if (auto fileGeometry = std::dynamic_pointer_cast<Ape::IFileGeometry>(browserGeometry))
				{
					fileGeometry->setMaterial(browserMaterial);
				}
			}
			else
				browserMaterial->showOnOverlay(true, browser->getZOrder());
		}
	}
}

void Ape::CefBrowserPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	CefSettings settings;
	settings.ignore_certificate_errors = true;
	CefString(&settings.browser_subprocess_path).FromASCII("ApeCefSubProcessApp.exe");
#if defined(OS_WIN)
	CefMainArgs main_args(::GetModuleHandle(0));
#endif
	if (CefInitialize(main_args, settings, nullptr, nullptr))
	{
		mpApeCefRenderHandlerImpl = new Ape::CefRenderHandlerImpl();
		mpApeCefLifeSpanHandlerImpl = new Ape::CefLifeSpanHandlerImpl();
		mpApeCefKeyboardHandlerImpl = new Ape::CefKeyboardHandlerImpl();
		mApeCefClientImpl = new Ape::CefClientImpl(mpApeCefRenderHandlerImpl, mpApeCefLifeSpanHandlerImpl, mpApeCefKeyboardHandlerImpl);
		mCefIsInintialzed = true;
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::CefBrowserPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	if (mCefIsInintialzed)
	{
		processEventDoubleQueue();
		CefRunMessageLoop();
		CefShutdown();
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::CefBrowserPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::CefBrowserPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::CefBrowserPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::CefBrowserPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
