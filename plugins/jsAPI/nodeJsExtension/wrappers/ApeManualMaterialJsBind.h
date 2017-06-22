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

#ifndef APE_MANUALMATERIALJSBIND_H
#define APE_MANUALMATERIALJSBIND_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "Ape.h"
#include "ApeIManualMaterial.h"
#include "ApeManualPassJsBind.h"
#include "ApePbsPassJsBind.h"

#ifdef NBIND_CLASS

class ManualMaterialJsPtr
{
private:
	Ape::ManualMaterialWeakPtr mPtr;

public:
	ManualMaterialJsPtr(Ape::ManualMaterialWeakPtr ptr)
	{
		mPtr = ptr;
	}

	ManualMaterialJsPtr(Ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<Ape::IManualMaterial>(ptr.lock());
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

	Ape::MaterialWeakPtr getMaterialWeakPtr()
	{
		return std::static_pointer_cast<Ape::Material>(mPtr.lock());
	}

	Ape::MaterialSharedPtr getMaterialSharedPtr()
	{
		return this->getMaterialWeakPtr().lock();
	}

	Ape::ManualMaterialSharedPtr getManualMaterialSharedPtr()
	{
		return std::static_pointer_cast<Ape::IManualMaterial>(mPtr.lock());
	}

	Ape::ManualMaterialWeakPtr getManualMaterialWeakPtr()
	{
		return mPtr;
	}

	// Pass

	Ape::PassWeakPtr getPassWeakPtr()
	{
		return mPtr.lock()->getPass();
	}

	ManualPassJsPtr getManualPassJsPtr()
	{
		return ManualPassJsPtr(getPassWeakPtr());
	}

	// Entity

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const Ape::Entity::Type getType()
	{
		return mPtr.lock()->getType();
	}

	// IManualMaterial

	void setDiffuseColor(Ape::Color diffuse)
	{
		mPtr.lock()->setDiffuseColor(diffuse);
	}

	void setSpecularColor(Ape::Color specular)
	{
		mPtr.lock()->setSpecularColor(specular);
	}

	Ape::Color getDiffuseColor() { return mPtr.lock()->getDiffuseColor(); };

	Ape::Color getSpecularColor() { return mPtr.lock()->getSpecularColor(); };

	void setManualPass(ManualPassJsPtr manualPass)
	{
		mPtr.lock()->setPass(manualPass.getManualPassSharedPtr());
	}

	void setPbsPass(PbsPassJsPtr pbsPass)
	{
		mPtr.lock()->setPass(pbsPass.getPbsPassSharedPtr());
	}
};

using namespace Ape;

NBIND_CLASS(ManualMaterialJsPtr)
{
	construct<Ape::ManualMaterialWeakPtr>();
	construct<Ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getMaterialWeakPtr);
	method(getMaterialSharedPtr);
	method(getManualMaterialWeakPtr);
	method(getManualMaterialSharedPtr);

	// Pass

	method(getPassWeakPtr);
	method(getManualPassJsPtr);

	// Entity

	method(getName);
	method(getType);

	// IManualMaterial
	method(setDiffuseColor);
	method(setSpecularColor);
	method(getDiffuseColor);
	method(getSpecularColor);
	method(setManualPass);
	method(setPbsPass);
}

#endif

#endif
