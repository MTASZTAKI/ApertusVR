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

#ifndef APE_CEFCLIENTIMPL_H
#define APE_CEFCLIENTIMPL_H

#include <iostream>
#include <string>
#include <thread> 
#include "cef_app.h"
#include "cef_client.h"
#include "cef_render_handler.h"
#include "cef_life_span_handler.h"
#include "cef_keyboard_handler.h"
#include "ApeCefRenderHandlerImpl.h"
#include "ApeCefLifeSpanHandlerImpl.h"
#include "ApeCefKeyboardHandlerImpl.h"

namespace ape
{
	class CefClientImpl : public CefClient
	{
	private:
		CefRefPtr<CefRenderHandler> mCefRenderHandlerImpl;

		CefRefPtr<CefLifeSpanHandler> mCefLifeSpanHandlerImpl;

		CefRefPtr<CefKeyboardHandler> mCefKeyboardHandlerImpl;

	public:
		CefClientImpl(ape::CefRenderHandlerImpl *cefRenderHandlerImpl, ape::CefLifeSpanHandlerImpl* cefLifeSpanHandlerImpl, ape::CefKeyboardHandlerImpl* cefKeyboardHandlerImpl);

		~CefClientImpl();

		virtual CefRefPtr<CefRenderHandler> GetRenderHandler();

		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler();

		virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler();

		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

		IMPLEMENT_REFCOUNTING(CefClientImpl);
	};
}

#endif
