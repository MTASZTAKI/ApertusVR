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

#ifndef APE_MANUALPASSJSBIND_H
#define APE_MANUALPASSJSBIND_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "Ape.h"
#include "ApeColor.h"
#include "ApeIManualPass.h"

#ifdef NBIND_CLASS

class ManualPassJsPtr
{
private:
	Ape::ManualPassWeakPtr mPtr;

public:
	ManualPassJsPtr(Ape::ManualPassWeakPtr ptr)
	{
		mPtr = ptr;
	}

	ManualPassJsPtr(Ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<Ape::IManualPass>(ptr.lock());
	}

	// Pointers

	const Ape::EntityWeakPtr getEntityWeakPtr()
	{
		return std::static_pointer_cast<Ape::Entity>(mPtr.lock());
	}

	const Ape::EntitySharedPtr getEntitySharedPtr()
	{
		return this->getEntityWeakPtr().lock();
	}

	Ape::PassWeakPtr getPassWeakPtr()
	{
		return std::static_pointer_cast<Ape::IManualPass>(mPtr.lock());
	}

	Ape::PassSharedPtr getPassSharedPtr()
	{
		return this->getManualPassWeakPtr().lock();
	}

	Ape::ManualPassSharedPtr getManualPassSharedPtr()
	{
		return std::static_pointer_cast<Ape::IManualPass>(mPtr.lock());
	}

	Ape::ManualPassWeakPtr getManualPassWeakPtr()
	{
		return mPtr;
	}

	// Pass

	Ape::Color getDiffuseColor() { return mPtr.lock()->getDiffuseColor(); };

	Ape::Color getSpecularColor() { return mPtr.lock()->getSpecularColor(); };

	Ape::Color getAmbientColor() { return mPtr.lock()->getAmbientColor(); };

	Ape::Color getEmissiveColor() { return mPtr.lock()->getEmissiveColor(); };

	// Entity

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const Ape::Entity::Type getType()
	{
		return mPtr.lock()->getType();
	}

	// IManualPass

	void setDiffuseColor(Ape::Color diffuse)
	{
		mPtr.lock()->setDiffuseColor(diffuse);
	}

	void setSpecularColor(Ape::Color specular)
	{
		mPtr.lock()->setSpecularColor(specular);
	}

	void setAmbientColor(Ape::Color ambient)
	{
		mPtr.lock()->setAmbientColor(ambient);
	}

	void setEmissiveColor(Ape::Color emissive)
	{
		mPtr.lock()->setEmissiveColor(emissive);
	}

	void setShininess(float shininess) 
	{
		mPtr.lock()->setShininess(shininess);
	}
};

using namespace Ape;

NBIND_CLASS(ManualPassJsPtr)
{
	construct<Ape::ManualPassWeakPtr>();
	construct<Ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getPassWeakPtr);
	method(getPassSharedPtr);
	method(getManualPassWeakPtr);
	method(getManualPassSharedPtr);

	// Pass

	method(getDiffuseColor);
	method(getSpecularColor);
	method(getAmbientColor);
	method(getEmissiveColor);

	// Entity

	method(getName);
	method(getType);

	// IManualPass
	method(setDiffuseColor);
	method(setSpecularColor);
	method(setAmbientColor);
	method(setEmissiveColor);
}

#endif

#endif
