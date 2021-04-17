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

#include <thread>

#include "apeSystem.h"
#include "apeICoreConfig.h"
#include <filesystem>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
void* gpMainWindow;

namespace 
{
	HWINEVENTHOOK g_WindowDestructionHook = NULL;
}

inline void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (gpMainWindow == hwnd) 
	{
		ape::System::Stop();
	}
}

inline void RegisterWindowDestructionHook() 
{
	g_WindowDestructionHook = ::SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
}

inline void UnregisterHook()
{
	::UnhookWinEvent(g_WindowDestructionHook);
}

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{ 
	ape::System::Start(lpszArgument, false);
	while (ape::ICoreConfig::getSingletonPtr()->getWindowConfig().handle == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	gpMainWindow = ape::ICoreConfig::getSingletonPtr()->getWindowConfig().handle;
	::CoInitialize(NULL);
	RegisterWindowDestructionHook();
	MSG msg = {};
	while (::GetMessageW(&msg, nullptr, 0, 0) > 0) {
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
	ape::System::Stop();
	UnregisterHook();
	::CoUninitialize();
	return 0;
}
#else
#include "CoreFoundation/CoreFoundation.h"

void my_handler(int s) {
    ape::System::Stop();
    exit(1);
}

int main(int argc, const char * argv[]) {
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);
    
//    std::string separator = "/Resources";
//    std::string pathStr = path;
//    auto found = std::find_end(pathStr.begin(), pathStr.end(), separator.begin(), separator.end());
//    size_t foundPos = found - pathStr.begin();
//    auto subPath = pathStr.substr(0, foundPos);
    std::string subPath = path;
    subPath += "/characterGLB";
    auto asd = chdir(subPath.c_str());
    std::cout << "Current Path: " << subPath << " "<< asd << std::endl;
    
    
//
//    auto path2 = std::filesystem::current_path(); //getting path
//    std::cout << "Current Path: " << path2 <<std::endl;
//    std::filesystem::current_path(path);
//    path2 = std::filesystem::current_path(); //getting path
//    std::cout << "Current Path: " << path2 <<std::endl;
    
//    if (argc > 1)
//    {
//        signal(SIGINT, my_handler);
//        ape::System::Start(argv[1], true);
//    }
//    else{
            std::string separator = "/Resources";
            std::string pathStr = path;
            auto found = std::find_end(pathStr.begin(), pathStr.end(), separator.begin(), separator.end());
            size_t foundPos = found - pathStr.begin();
            auto inPath = pathStr.substr(0, foundPos)+"/samples/virtualLearningFactory";
        
        //std::string inPath = "../../samples/virtualLearningFactory";
        if(inPath.length()>0)
        {
           
            signal(SIGINT, my_handler);
            ape::System::Start(inPath.c_str(), true);
        }
//    while (ape::ICoreConfig::getSingletonPtr()->getWindowConfig().handle == nullptr)
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ape::System::Stop();
    return 0;
}

#endif
