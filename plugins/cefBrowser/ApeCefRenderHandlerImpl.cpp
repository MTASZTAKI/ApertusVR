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
}

Ape::CefRenderHandlerImpl::~CefRenderHandlerImpl()
{
}

bool Ape::CefRenderHandlerImpl::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect & rect)
{
	if (auto texture = mBrowserIDTextures[browser->GetIdentifier()].lock())
		rect = CefRect(0, 0, texture->getParameters().width, texture->getParameters().height);
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
