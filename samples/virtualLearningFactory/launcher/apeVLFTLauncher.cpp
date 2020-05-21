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
#include <windows.h>
#include "apeSystem.h"
#include "apeICoreConfig.h"

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

