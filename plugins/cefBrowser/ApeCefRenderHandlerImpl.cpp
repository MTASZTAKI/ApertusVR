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

#include "ApeCefRenderHandlerImpl.h"

Ape::CefRenderHandlerImpl::CefRenderHandlerImpl()
{
	mBrowserIDTextures = std::map<int, Ape::ManualTextureWeakPtr>();
	mBrowsers = std::map<int, CefRefPtr<CefBrowser>>();
	mMouseCurrentPosition = Ape::Vector2();
}

Ape::CefRenderHandlerImpl::~CefRenderHandlerImpl()
{

}

bool Ape::CefRenderHandlerImpl::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect & rect)
{
	if (auto texture = mBrowserIDTextures[browser->GetIdentifier()].lock())
	{
		rect = CefRect(0, 0, texture->getParameters().width, texture->getParameters().height);
		mBrowsers[browser->GetIdentifier()] = browser;
	}
	return true;
}

void Ape::CefRenderHandlerImpl::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList & dirtyRects, const void * buffer, int width, int height)
{
	if (auto texture = mBrowserIDTextures[browser->GetIdentifier()].lock())
		texture->setBuffer(buffer);
}

void Ape::CefRenderHandlerImpl::addTexture(int browserID, Ape::ManualTextureWeakPtr texture)
{
	mBrowserIDTextures[browserID] = texture;
}

void Ape::CefRenderHandlerImpl::setZoomLevel(int browserID, int zoomLevel)
{
	if (mBrowsers.size() && mBrowsers[browserID])
		mBrowsers[browserID]->GetHost()->SetZoomLevel(zoomLevel);
}

void Ape::CefRenderHandlerImpl::setURL(int browserID, std::string url)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		mBrowsers[browserID]->GetMainFrame()->LoadURL(url);
		//std::cout << "Ape:::CefRenderHandlerImpl::setURL " << mBrowsers[browserID]->GetIdentifier() << " go to:" << url << std::endl;
	}
}

void Ape::CefRenderHandlerImpl::mouseClick(int browserID, bool isClickDown, CefBrowserHost::MouseButtonType mouseButtonType)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		CefMouseEvent cefMouseEvent;
		cefMouseEvent.x = mMouseCurrentPosition.x;
		cefMouseEvent.y = mMouseCurrentPosition.y;
		mBrowsers[browserID]->GetHost()->SendMouseClickEvent(cefMouseEvent, mouseButtonType, !isClickDown, 1);
		//std::cout << "Ape::CefRenderHandlerImpl::mouseClick " << " type:" << mouseButtonType << "isDown:" << isClickDown << std::endl;
	}
}

void Ape::CefRenderHandlerImpl::mouseScroll(int browserID, int deltaX, int deltaY)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		CefMouseEvent cefMouseEvent;
		cefMouseEvent.x = mMouseCurrentPosition.x;
		cefMouseEvent.y = mMouseCurrentPosition.y;
		mBrowsers[browserID]->GetHost()->SendMouseWheelEvent(cefMouseEvent, deltaX, deltaY);
		//std::cout << "Ape::CefRenderHandlerImpl::mouseScroll " << "x:" << cefMouseEvent.x << " y:" << cefMouseEvent.y << " deltaY:" << deltaY << std::endl;
	}
}

void Ape::CefRenderHandlerImpl::mouseMoved(int browserID, int x, int y)
{
	if (mBrowsers.size() && mBrowsers[browserID])
	{
		mMouseCurrentPosition.x = x;
		mMouseCurrentPosition.y = y;
		CefMouseEvent cefMouseEvent;
		cefMouseEvent.x = mMouseCurrentPosition.x;
		cefMouseEvent.y = mMouseCurrentPosition.y;
		mBrowsers[browserID]->GetHost()->SendMouseMoveEvent(cefMouseEvent, false);
		//std::cout << "Ape::CefRenderHandlerImpl::mouseMoved " << "x:" << cefMouseEvent.x << " y:" << cefMouseEvent.y << std::endl;
	}
}

void Ape::CefRenderHandlerImpl::keyValue(int browserID, int keyASCIIValue)
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

		std::cout << "Ape::CefRenderHandlerImpl::keyValue " << "windows_key_code:" << cefKeyEvent.windows_key_code << "keyASCIIValue:" << keyASCIIValue << std::endl;
	}
}
