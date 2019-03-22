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

#include "apeCefRenderHandlerImpl.h"

ape::CefRenderHandlerImpl::CefRenderHandlerImpl()
{
	mBrowserIDTextures = std::map<int, ape::ManualTextureWeakPtr>();
	mBrowsers = std::map<int, CefRefPtr<CefBrowser>>();
	mMouseCurrentPosition = ape::Vector2();
}

ape::CefRenderHandlerImpl::~CefRenderHandlerImpl()
{

}

bool ape::CefRenderHandlerImpl::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect & rect)
{
	if (auto texture = mBrowserIDTextures[browser->GetIdentifier()].lock())
	{
		rect = CefRect(0, 0, texture->getParameters().width, texture->getParameters().height);
		mBrowsers[browser->GetIdentifier()] = browser;
	}
	return true;
}

void ape::CefRenderHandlerImpl::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList & dirtyRects, const void * buffer, int width, int height)
{
	if (auto texture = mBrowserIDTextures[browser->GetIdentifier()].lock())
		texture->setBuffer(buffer);
}

void ape::CefRenderHandlerImpl::addTexture(int browserID, ape::ManualTextureWeakPtr texture)
{
	mBrowserIDTextures[browserID] = texture;
}

void ape::CefRenderHandlerImpl::setZoomLevel(int browserID, int zoomLevel)
{
	if (mBrowsers.size() && mBrowsers[browserID])
		mBrowsers[browserID]->GetHost()->SetZoomLevel(zoomLevel);
}

void ape::CefRenderHandlerImpl::setURL(int browserID, std::string url)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		mBrowsers[browserID]->GetMainFrame()->LoadURL(url);
		//APE_LOG_DEBUG("browser id: " << mBrowsers[browserID]->GetIdentifier() << " go to:" << url)
	}
}

void ape::CefRenderHandlerImpl::mouseClick(int browserID, bool isClickDown, CefBrowserHost::MouseButtonType mouseButtonType)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		CefMouseEvent cefMouseEvent;
		cefMouseEvent.x = mMouseCurrentPosition.x;
		cefMouseEvent.y = mMouseCurrentPosition.y;
		mBrowsers[browserID]->GetHost()->SendMouseClickEvent(cefMouseEvent, mouseButtonType, !isClickDown, 1);
		//APE_LOG_DEBUG("type:" << mouseButtonType << " isDown: " << isClickDown);
	}
}

void ape::CefRenderHandlerImpl::mouseScroll(int browserID, int deltaX, int deltaY)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		CefMouseEvent cefMouseEvent;
		cefMouseEvent.x = mMouseCurrentPosition.x;
		cefMouseEvent.y = mMouseCurrentPosition.y;
		mBrowsers[browserID]->GetHost()->SendMouseWheelEvent(cefMouseEvent, deltaX, deltaY);
		//APE_LOG_DEBUG("x: " << cefMouseEvent.x << " y: " << cefMouseEvent.y << " deltaY: " << deltaY);
	}
}

void ape::CefRenderHandlerImpl::mouseMoved(int browserID, int x, int y)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		mMouseCurrentPosition.x = x;
		mMouseCurrentPosition.y = y;
		CefMouseEvent cefMouseEvent;
		cefMouseEvent.x = mMouseCurrentPosition.x;
		cefMouseEvent.y = mMouseCurrentPosition.y;
		mBrowsers[browserID]->GetHost()->SendMouseMoveEvent(cefMouseEvent, false);
		//APE_LOG_DEBUG("x: " << cefMouseEvent.x << " y: " << cefMouseEvent.y);
	}
}

void ape::CefRenderHandlerImpl::keyValue(int browserID, int keyASCIIValue)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		CefKeyEvent cefKeyEvent;

		if (
			   keyASCIIValue == 8 // backspace
			|| keyASCIIValue == 9 // tab
			|| keyASCIIValue == 13 // return
			|| keyASCIIValue == 14 // left shift
			|| keyASCIIValue == 35 // end
			|| keyASCIIValue == 36 // home
			|| keyASCIIValue == 37 // arrow left
			|| keyASCIIValue == 38 // arrow up
			|| keyASCIIValue == 39 // arrow right
			|| keyASCIIValue == 40 // arrow up
			|| keyASCIIValue == 46 // del
			)
			cefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYDOWN;
		else
			cefKeyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;

		cefKeyEvent.windows_key_code = keyASCIIValue;
		// exceptions
		if (keyASCIIValue == 1046)
			cefKeyEvent.windows_key_code = 46;

		mBrowsers[browserID]->GetHost()->SendKeyEvent(cefKeyEvent);

		cefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYUP;
		mBrowsers[browserID]->GetHost()->SendKeyEvent(cefKeyEvent);

		APE_LOG_TRACE("windows_key_code: " << cefKeyEvent.windows_key_code << " keyASCIIValue: " << keyASCIIValue);
	}
}
