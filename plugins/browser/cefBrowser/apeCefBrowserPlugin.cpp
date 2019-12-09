#include "apeCefBrowserPlugin.h"

ape::CefBrowserPlugin::CefBrowserPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpapeCefRenderHandlerImpl = nullptr;
	mpapeCefLifeSpanHandlerImpl = nullptr;
	mpapeCefKeyboardHandlerImpl = nullptr;
	mapeCefClientImpl = nullptr;
	mBrowserCounter = 0;
	mCefIsInintialzed = false;
	mBrowserSettings = CefBrowserSettings();
	mpEventManager->connectEvent(ape::Event::Group::BROWSER, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mEventDoubleQueue = ape::DoubleQueue<Event>();
	mBrowserIDNames = std::map<std::string, int>();
	mRayOverlayNode = ape::NodeWeakPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::CefBrowserPlugin::~CefBrowserPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::BROWSER, std::bind(&CefBrowserPlugin::eventCallBack, this, std::placeholders::_1));
	mapeCefClientImpl = nullptr;
	delete mpapeCefRenderHandlerImpl;
	delete mpapeCefLifeSpanHandlerImpl;
	APE_LOG_FUNC_LEAVE();
}

void ape::CefBrowserPlugin::processEvent(ape::Event event)
{
	if (event.group == ape::Event::Group::BROWSER)
	{
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			switch (event.type)
			{
			case ape::Event::Type::BROWSER_CREATE:
				break;
			case ape::Event::Type::BROWSER_GEOMETRY:
				{
					createBrowser(browser);
				}
				break;
			case ape::Event::Type::BROWSER_OVERLAY:
				{
					createBrowser(browser);
				}
				break;
			case ape::Event::Type::BROWSER_ZOOM:
				{
					if (mBrowserIDNames[browser->getName()])
						mpapeCefRenderHandlerImpl->setZoomLevel(mBrowserIDNames[browser->getName()], browser->getZoomLevel());
				}
				break;
			case ape::Event::Type::BROWSER_URL:
				{
					if (mBrowserIDNames[browser->getName()])
						mpapeCefRenderHandlerImpl->setURL(mBrowserIDNames[browser->getName()], browser->getURL());
				}
				break;
			case ape::Event::Type::BROWSER_RELOAD:
			{
				if (mBrowserIDNames[browser->getName()])
					mpapeCefRenderHandlerImpl->setURL(mBrowserIDNames[browser->getName()], browser->getURL());
			}
			break;
			case ape::Event::Type::BROWSER_MOUSE_MOVED:
				{
					if (mBrowserIDNames[browser->getName()])
					{
						ape::Browser::MouseState mouseState = browser->getMouseState();
						mpapeCefRenderHandlerImpl->mouseMoved(mBrowserIDNames[browser->getName()], mouseState.position.x, mouseState.position.y);
					}
				}
				break;
			case ape::Event::Type::BROWSER_MOUSE_SCROLL:
				{
					if (mBrowserIDNames[browser->getName()])
					{
						ape::Browser::MouseState mouseState = browser->getMouseState();
						mpapeCefRenderHandlerImpl->mouseScroll(mBrowserIDNames[browser->getName()], mouseState.scrollDelta.x, mouseState.scrollDelta.y);
					}
				}
				break;
			case ape::Event::Type::BROWSER_MOUSE_CLICK:
				{
					if (mBrowserIDNames[browser->getName()])
					{
						ape::Browser::MouseState mouseState = browser->getMouseState();
						if (mouseState.click == ape::Browser::MouseClick::LEFT)
							mpapeCefRenderHandlerImpl->mouseClick(mBrowserIDNames[browser->getName()], mouseState.isClickDown, CefBrowserHost::MouseButtonType::MBT_LEFT);
						else if (mouseState.click == ape::Browser::MouseClick::RIGHT)
							mpapeCefRenderHandlerImpl->mouseClick(mBrowserIDNames[browser->getName()], mouseState.isClickDown, CefBrowserHost::MouseButtonType::MBT_RIGHT);
						else if (mouseState.click == ape::Browser::MouseClick::MIDDLE)
							mpapeCefRenderHandlerImpl->mouseClick(mBrowserIDNames[browser->getName()], mouseState.isClickDown, CefBrowserHost::MouseButtonType::MBT_MIDDLE);
					}
				}
				break;
			case ape::Event::Type::BROWSER_KEY_VALUE:
				{
					if (mBrowserIDNames[browser->getName()])
						mpapeCefRenderHandlerImpl->keyValue(mBrowserIDNames[browser->getName()], browser->getLastKeyASCIIValue());
				}
				break;
			case ape::Event::Type::BROWSER_DELETE:
				;
				break;
			}
		}
	}
	else if (event.type == ape::Event::Type::GEOMETRY_RAY_PARENTNODE)
	{
		if (auto rayGeometry = std::static_pointer_cast<ape::IRayGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
			mRayOverlayNode = rayGeometry->getParentNode();
	}
	/*else if (event.type == ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY)
	{
		if (auto rayOverlayNode = mRayOverlayNode.lock())
			rayOverlayNode->getPosition();
	}*/
}

void ape::CefBrowserPlugin::processEventDoubleQueue()
{
	mEventDoubleQueue.swap();
	while (!mEventDoubleQueue.emptyPop())
	{
		processEvent(mEventDoubleQueue.front());
		mEventDoubleQueue.pop();
	}
}

void ape::CefBrowserPlugin::eventCallBack(const ape::Event& event)
{
	if (!mCefIsInintialzed)
		mEventDoubleQueue.push(event);
	else
		processEvent(event);
}

void ape::CefBrowserPlugin::createBrowser(ape::BrowserSharedPtr browser)
{
	std::string browserName = browser->getName();
	if (auto browserMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(browserName + "_Material", ape::Entity::MATERIAL_MANUAL).lock()))
	{
		browserMaterial->setAmbientColor(ape::Color(1.0f, 1.0f, 1.0f));
		browserMaterial->setDiffuseColor(ape::Color(1.0f, 1.0f, 1.0f));
		browserMaterial->setEmissiveColor(ape::Color(1.0f, 1.0f, 1.0f));
		if (auto browserTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->createEntity(browserName + "_Texture", ape::Entity::TEXTURE_MANUAL).lock()))
		{
			browserTexture->setParameters(browser->getResoultion().x, browser->getResoultion().y, ape::Texture::PixelFormat::A8R8G8B8, ape::Texture::Usage::DYNAMIC_WRITE_ONLY, false, false);
			browserMaterial->setTexture(browserTexture);
			browserMaterial->setSceneBlending(ape::Material::SceneBlendingType::TRANSPARENT_ALPHA);
			mBrowserCounter++;
			mpapeCefLifeSpanHandlerImpl->registerBrowser(mBrowserCounter, browser);
			mpapeCefKeyboardHandlerImpl->registerBrowser(mBrowserCounter, browser);
			mpapeCefRenderHandlerImpl->addTexture(mBrowserCounter, browserTexture);
			mBrowserIDNames[browserName] = mBrowserCounter;
			CefWindowInfo cefWindowInfo;
			cefWindowInfo.SetAsWindowless(0);
			CefBrowserHost::CreateBrowser(cefWindowInfo, mapeCefClientImpl.get(), browser->getURL(), mBrowserSettings, nullptr);
			if (auto browserGeometry = browser->getGeometry().lock())
			{
				if (auto planeGeometry = std::dynamic_pointer_cast<ape::IPlaneGeometry>(browserGeometry))
				{
					planeGeometry->setMaterial(browserMaterial);
					browserMaterial->setCullingMode(ape::Material::CullingMode::NONE_CM);
				}
				else if (auto fileGeometry = std::dynamic_pointer_cast<ape::IFileGeometry>(browserGeometry))
				{
					fileGeometry->setMaterial(browserMaterial);
					browserMaterial->setCullingMode(ape::Material::CullingMode::CLOCKWISE);
				}
			}
			else
				browserMaterial->showOnOverlay(true, browser->getZOrder());
		}
	}
	else if (auto browserMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->getEntity(browserName + "_Material").lock()))
	{
		if (auto browserTexture = std::static_pointer_cast<ape::IManualTexture>(mpSceneManager->getEntity(browserName + "_Texture").lock()))
		{
			mBrowserCounter++;
			mpapeCefLifeSpanHandlerImpl->registerBrowser(mBrowserCounter, browser);
			mpapeCefKeyboardHandlerImpl->registerBrowser(mBrowserCounter, browser);
			mpapeCefRenderHandlerImpl->addTexture(mBrowserCounter, browserTexture);
			mBrowserIDNames[browserName] = mBrowserCounter;
			CefWindowInfo cefWindowInfo;
			cefWindowInfo.SetAsWindowless(0);
			CefBrowserHost::CreateBrowser(cefWindowInfo, mapeCefClientImpl.get(), browser->getURL(), mBrowserSettings, nullptr);
		}
	}
}

void ape::CefBrowserPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	CefSettings settings;
	settings.ignore_certificate_errors = true;
	CefString(&settings.browser_subprocess_path).FromASCII("apeCefSubProcessApp.exe");
#if defined(OS_WIN)
	CefMainArgs main_args(::GetModuleHandle(0));
#endif
	if (CefInitialize(main_args, settings, nullptr, nullptr))
	{
		mpapeCefRenderHandlerImpl = new ape::CefRenderHandlerImpl();
		mpapeCefLifeSpanHandlerImpl = new ape::CefLifeSpanHandlerImpl();
		mpapeCefKeyboardHandlerImpl = new ape::CefKeyboardHandlerImpl();
		mapeCefClientImpl = new ape::CefClientImpl(mpapeCefRenderHandlerImpl, mpapeCefLifeSpanHandlerImpl, mpapeCefKeyboardHandlerImpl);
		mCefIsInintialzed = true;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::CefBrowserPlugin::Run()
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

void ape::CefBrowserPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::CefBrowserPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::CefBrowserPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::CefBrowserPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
