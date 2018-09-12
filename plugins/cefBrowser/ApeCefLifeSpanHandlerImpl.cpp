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

#include "ApeCefLifeSpanHandlerImpl.h"

Ape::CefLifeSpanHandlerImpl::CefLifeSpanHandlerImpl()
{
	mBrowserIDs = std::map<int, Ape::BrowserWeakPtr>();
}

Ape::CefLifeSpanHandlerImpl::~CefLifeSpanHandlerImpl()
{

}

void Ape::CefLifeSpanHandlerImpl::registerBrowser(int ID, Ape::BrowserWeakPtr browser)
{
	mBrowserIDs[ID] = browser;
}

void Ape::CefLifeSpanHandlerImpl::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{

}

bool Ape::CefLifeSpanHandlerImpl::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString & target_url,
	const CefString & target_frame_name, WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures & popupFeatures,
	CefWindowInfo & windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings & settings, bool * no_javascript_access)
{
	if (auto apeBrowser = mBrowserIDs[browser->GetIdentifier()].lock())
		apeBrowser->setURL(target_url);
	//APE_LOG_DEBUG("url: " << turl << " id:" << browser->GetIdentifier());
	return true;
}
