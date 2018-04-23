/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#include "ApeCefKeyboardHandlerImpl.h"

Ape::CefKeyboardHandlerImpl::CefKeyboardHandlerImpl()
{
	LOG_FUNC_ENTER();
	mBrowserIDs = std::map<int, Ape::BrowserWeakPtr>();
	LOG_FUNC_LEAVE();
}

Ape::CefKeyboardHandlerImpl::~CefKeyboardHandlerImpl()
{

}

void Ape::CefKeyboardHandlerImpl::registerBrowser(int ID, Ape::BrowserWeakPtr browser)
{
	LOG_FUNC_ENTER();
	mBrowserIDs[ID] = browser;
	LOG_FUNC_LEAVE();
}

bool Ape::CefKeyboardHandlerImpl::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent & event, CefEventHandle os_event, bool * is_keyboard_shortcut)
{
	if (auto apeBrowser = mBrowserIDs[browser->GetIdentifier()].lock())
	{
		LOG_TRACE("apeBrowser->setFocusOnEditableField " << event.focus_on_editable_field);
		apeBrowser->setFocusOnEditableField(event.focus_on_editable_field);
	}
	return false;
}

bool Ape::CefKeyboardHandlerImpl::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent & event, CefEventHandle os_event)
{
	LOG_TRACE("event.focus_on_editable_field: " << event.focus_on_editable_field);
	return false;
}
