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

#ifndef APE_CEFFOCUSHANDLERIMPL_H
#define APE_CEFFOCUSHANDLERIMPL_H

#include <iostream>
#include <string>
#include <thread> 
#include "cef_app.h"
#include "cef_client.h"
#include "cef_keyboard_handler.h"
#include "Ape.h"
#include "ApeIManualTexture.h"
#include "ApeIBrowser.h"
#include "ApeILogManager.h"

namespace Ape
{
	class CefKeyboardHandlerImpl : public CefKeyboardHandler
	{
	private:
		std::map<int, Ape::BrowserWeakPtr> mBrowserIDs;

	public:
		CefKeyboardHandlerImpl();

		~CefKeyboardHandlerImpl();

		void registerBrowser(int ID, Ape::BrowserWeakPtr browser);

		bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) override;

		bool OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event) override;

		IMPLEMENT_REFCOUNTING(CefKeyboardHandlerImpl);
	};
}

#endif
