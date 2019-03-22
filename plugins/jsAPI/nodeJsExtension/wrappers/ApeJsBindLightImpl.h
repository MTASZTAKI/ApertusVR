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

#ifndef APE_JSBIND_LIGHTIMPL_H
#define APE_JSBIND_LIGHTIMPL_H

#include "apeLightImpl.h"
#include "nbind/nbind.h"
#include "nbind/api.h"

#ifdef NBIND_CLASS

class LightJsPtr
{
private:
	ape::LightWeakPtr mPtr;

public:
	LightJsPtr(ape::LightWeakPtr ptr)
	{
		mPtr = ptr;
	}

	LightJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::ILight>(ptr.lock());
	}

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const ape::Color getDiffuseColor()
	{
		return mPtr.lock()->getDiffuseColor();
	}

	void setDiffuseColor(ape::Color color)
	{
		mPtr.lock()->setDiffuseColor(color);
	}
};

NBIND_CLASS(LightJsPtr)
{
	construct<ape::LightWeakPtr>();
	construct<ape::EntityWeakPtr>();
	method(getName);
	method(getDiffuseColor);
	method(setDiffuseColor);
}

#endif

#endif
