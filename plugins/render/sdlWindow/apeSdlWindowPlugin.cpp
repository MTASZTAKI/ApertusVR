#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "apeSdlWindowPlugin.h"

ape::SdlWindowPlugin::SdlWindowPlugin( )
{
	APE_LOG_FUNC_ENTER();
    mSDLClosed=false;
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mSdlWindowPluginConfig = ape::SdlWindowPluginConfig();
    parseConfigJSON();
    initSDL();
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

void ape::SdlWindowPlugin::initSDL(){

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        APE_LOG_DEBUG("SDL_Init OK");
       
        for (int i = 0; i < mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList.size(); i++)
        {
            auto window = SDL_CreateWindow(mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList[i].name.c_str(), 0, 0,
                mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList[i].width, mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList[i].height,
                SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
            if (window)
            {
                SDL_Window* ads;
                APE_LOG_DEBUG("SDL_CreateWindow OK");
                SDL_SysWMinfo wmi;
                SDL_VERSION(&wmi.version);
                SDL_GetWindowWMInfo(window, &wmi);
                
                #ifdef _WIN32
                ape::WindowConfig windowConfig("myWindow", "OGL", (void*)wmi.info.win.window, 0, mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList[i].width,       mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList[i].height);
                #else
                ape::WindowConfig windowConfig("myWindow", "OGL", (void*)wmi.info.cocoa.window, 0, mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList[i].width, mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList[i].height);
                #endif
                mpCoreConfig->setWindowConfig(windowConfig);
                
            }
            else
            {
                APE_LOG_DEBUG("SDL_CreateWindow FAILED: " << SDL_GetError());
            }
        }
        
    }
    else
    {
        APE_LOG_DEBUG("SDL_Init FAILED: " << SDL_GetError());
    }
    
}

void ape::SdlWindowPlugin::initAndRunSDL(){
    initSDL();
    runSDL();
}

void ape::SdlWindowPlugin::runSDL(){
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
}

void ape::SdlWindowPlugin::parseConfigJSON(){
    std::stringstream fileFullPath;
    fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeSdlWindowPlugin.json";
    FILE* apeSdlWindowPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
    char readBuffer[65536];
    if (apeSdlWindowPluginConfigFile)
    {
        rapidjson::FileReadStream jsonFileReaderStream(apeSdlWindowPluginConfigFile, readBuffer, sizeof(readBuffer));
        rapidjson::Document jsonDocument;
        jsonDocument.ParseStream(jsonFileReaderStream);
        if (jsonDocument.IsObject())
        {
            rapidjson::Value& windows = jsonDocument["windows"];
            for (auto& window : windows.GetArray())
            {
                ape::SdlWindowPluginWindowConfig sdlWindowPluginWindowConfig;
                for (rapidjson::Value::MemberIterator windowMemberIterator =
                    window.MemberBegin(); windowMemberIterator != window.MemberEnd(); ++windowMemberIterator)
                {
                    if (windowMemberIterator->name == "resolution")
                    {
                        for (rapidjson::Value::MemberIterator resolutionMemberIterator =
                            window[windowMemberIterator->name].MemberBegin();
                            resolutionMemberIterator != window[windowMemberIterator->name].MemberEnd(); ++resolutionMemberIterator)
                        {
                            if (resolutionMemberIterator->name == "width")
                                sdlWindowPluginWindowConfig.width = resolutionMemberIterator->value.GetInt();
                            else if (resolutionMemberIterator->name == "height")
                                sdlWindowPluginWindowConfig.height = resolutionMemberIterator->value.GetInt();
                            else if (resolutionMemberIterator->name == "fullScreen")
                                sdlWindowPluginWindowConfig.fullScreen = resolutionMemberIterator->value.GetBool();
                        }
                    }
                }
                mSdlWindowPluginConfig.sdlWindowPluginWindowConfigList.push_back(sdlWindowPluginWindowConfig);
            }
        }
        fclose(apeSdlWindowPluginConfigFile);
    }
}

void ape::SdlWindowPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
    
	
    //dispatch_after( DISPATCH_TIME_NOW,dispatch_get_main_queue(), ^(void){
	
    //});
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
//    while(true)
//        std::this_thread::sleep_for(std::chrono::milliseconds(20));
	APE_LOG_FUNC_LEAVE();
}

void ape::SdlWindowPlugin::Step()
{
    
	APE_LOG_FUNC_ENTER();
    if(!mSDLClosed)
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
                mSDLClosed = true;
                
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    mSDLClosed = true;
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
    }
    
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
