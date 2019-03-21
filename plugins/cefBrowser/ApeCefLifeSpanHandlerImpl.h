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

#ifndef APE_CEFLIFESPANHANDLERIMPL_H
#define APE_CEFLIFESPANHANDLERIMPL_H

#include <iostream>
#include <string>
#include <thread> 
#include "cef_app.h"
#include "cef_client.h"
#include "cef_life_span_handler.h"
#include "Ape.h"
#include "sceneelements/ApeIManualTexture.h"
#include "sceneelements/ApeIBrowser.h"
#include "managers/ApeILogManager.h"

namespace ape
{
	class CefLifeSpanHandlerImpl : public CefLifeSpanHandler
	{
	private:
		std::map<int, ape::BrowserWeakPtr> mBrowserIDs;

	public:
		CefLifeSpanHandlerImpl();

		~CefLifeSpanHandlerImpl();

		void registerBrowser(int ID, ape::BrowserWeakPtr browser);

		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

		bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name,
			WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, 
			CefBrowserSettings& settings, bool* no_javascript_access) override;

		IMPLEMENT_REFCOUNTING(CefLifeSpanHandlerImpl);
	};
}

#endif
