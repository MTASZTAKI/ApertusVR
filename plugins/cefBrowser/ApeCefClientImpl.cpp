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

#include "ApeCefClientImpl.h"

Ape::CefClientImpl::CefClientImpl(Ape::CefRenderHandlerImpl* cefRenderHandlerImpl, Ape::CefLifeSpanHandlerImpl* cefLifeSpanHandlerImpl, Ape::CefKeyboardHandlerImpl* cefKeyboardHandlerImpl) 
	: mCefRenderHandlerImpl(cefRenderHandlerImpl)
	, mCefLifeSpanHandlerImpl(cefLifeSpanHandlerImpl)
	, mCefKeyboardHandlerImpl(cefKeyboardHandlerImpl)
{

}

Ape::CefClientImpl::~CefClientImpl()
{

}

CefRefPtr<CefRenderHandler> Ape::CefClientImpl::GetRenderHandler()
{
	return mCefRenderHandlerImpl;
}

CefRefPtr<CefLifeSpanHandler> Ape::CefClientImpl::GetLifeSpanHandler()
{
	return mCefLifeSpanHandlerImpl;
}

CefRefPtr<CefKeyboardHandler> Ape::CefClientImpl::GetKeyboardHandler()
{
	return mCefKeyboardHandlerImpl;
}

bool Ape::CefClientImpl::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	const std::string& message_name = message->GetName();
	LOG(LOG_TYPE_DEBUG, "message name: " << message_name);
	return true;
}
