#include "apeSdlWindowPlugin.h"

ape::SdlWindowPlugin::SdlWindowPlugin( )
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::SdlWindowPlugin::~SdlWindowPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::eventCallBack(const ape::Event& event)
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		APE_LOG_DEBUG("SDL_Init OK");
		auto window = SDL_CreateWindow("myWindow", 100, 100, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
		if (window)
		{
			APE_LOG_DEBUG("SDL_CreateWindow OK");
			SDL_SysWMinfo wmi;
			SDL_VERSION(&wmi.version);
			SDL_GetWindowWMInfo(window, &wmi);
			HWND win = (HWND)wmi.info.win.window;
			ape::WindowConfig windowConfig("myWindow", "OGL", (void*)win, 0, 1024, 768);
			mpCoreConfig->setWindowConfig(windowConfig);
		}
		else
		{
			APE_LOG_DEBUG("SDL_CreateWindow FAILED: " << SDL_GetError());
		}
	}
	else
	{
		APE_LOG_DEBUG("SDL_Init FAILED: " << SDL_GetError());
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	bool closed = false;
	while (!closed) 
	{
        constexpr int kMaxEvents = 16;
        SDL_Event events[kMaxEvents];
        int nevents = 0;
        while (nevents < kMaxEvents && SDL_PollEvent(&events[nevents]) != 0) {
            nevents++;
        }
        for (int i = 0; i < nevents; i++) {
            const SDL_Event& event = events[i];
            switch (event.type) {
            case SDL_QUIT:
                closed = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    closed = true;
                }
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
					;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
