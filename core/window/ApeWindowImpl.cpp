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

#include "ApeWindowImpl.h"

template<> Ape::IWindow* Ape::Singleton<Ape::IWindow>::msSingleton = 0;

Ape::WindowImpl::WindowImpl()
{
	msSingleton = this;
	mpHandle = nullptr;
	mWidth = 0;
	mHeight = 0;
	mIsMain = false;
}

Ape::WindowImpl::~WindowImpl()
{

}

void* Ape::WindowImpl::getHandle()
{
	return mpHandle;
}

unsigned int Ape::WindowImpl::getWidth()
{
	return mWidth;
}

unsigned int Ape::WindowImpl::getHeight()
{
	return mHeight;
}

void Ape::WindowImpl::setHandle(void* handle)
{
	mpHandle = handle;
}

void Ape::WindowImpl::setWidth(unsigned int width)
{
	mWidth = width;
}

void Ape::WindowImpl::setHeight(unsigned int height)
{
	mHeight = height;
}

void Ape::WindowImpl::setMain(bool isMain)
{
	mIsMain = isMain;
}

bool Ape::WindowImpl::isMain()
{
	return mIsMain;
}



