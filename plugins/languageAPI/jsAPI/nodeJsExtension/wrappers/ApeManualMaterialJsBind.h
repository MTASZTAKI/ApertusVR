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

#ifndef APE_MANUALMATERIALJSBIND_H
#define APE_MANUALMATERIALJSBIND_H

#include "ape.h"
#include "sceneelements/apeIManualMaterial.h"
#include "apeManualPassJsBind.h"
#include "ApeFileTextureJsBind.h"
#include "apePbsPassJsBind.h"
#include "nbind/nbind.h"
#include "nbind/api.h"

#ifdef NBIND_CLASS

class ManualMaterialJsPtr
{
private:
	ape::ManualMaterialWeakPtr mPtr;

public:
	ManualMaterialJsPtr(ape::ManualMaterialWeakPtr ptr)
	{
		mPtr = ptr;
	}

	ManualMaterialJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::IManualMaterial>(ptr.lock());
	}

	// Pointers

	const ape::EntityWeakPtr getEntityWeakPtr()
	{
		return std::static_pointer_cast<ape::Entity>(mPtr.lock());
	}

	const ape::EntitySharedPtr getEntitySharedPtr()
	{
		return this->getEntityWeakPtr().lock();
	}

	ape::MaterialWeakPtr getMaterialWeakPtr()
	{
		return std::static_pointer_cast<ape::Material>(mPtr.lock());
	}

	ape::MaterialSharedPtr getMaterialSharedPtr()
	{
		return this->getMaterialWeakPtr().lock();
	}

	ape::ManualMaterialSharedPtr getManualMaterialSharedPtr()
	{
		return std::static_pointer_cast<ape::IManualMaterial>(mPtr.lock());
	}

	ape::ManualMaterialWeakPtr getManualMaterialWeakPtr()
	{
		return mPtr;
	}

	// Entity

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const ape::Entity::Type getType()
	{
		return mPtr.lock()->getType();
	}

	// IManualMaterial

	void setDiffuseColor(ape::Color diffuse)
	{
		mPtr.lock()->setDiffuseColor(diffuse);
	}

	void setSpecularColor(ape::Color specular)
	{
		mPtr.lock()->setSpecularColor(specular);
	}

	ape::Color getDiffuseColor() { return mPtr.lock()->getDiffuseColor(); };

	ape::Color getSpecularColor() { return mPtr.lock()->getSpecularColor(); };

	void setTexture(FileTextureJsPtr fileTexture) {
		mPtr.lock()->setTexture(fileTexture.getTextureSharedPtr());
	}
};

using namespace ape;

NBIND_CLASS(ManualMaterialJsPtr)
{
	construct<ape::ManualMaterialWeakPtr>();
	construct<ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getMaterialWeakPtr);
	method(getMaterialSharedPtr);
	method(getManualMaterialWeakPtr);
	method(getManualMaterialSharedPtr);

	// Entity

	method(getName);
	method(getType);

	// IManualMaterial
	method(setDiffuseColor);
	method(setSpecularColor);
	method(getDiffuseColor);
	method(getSpecularColor);
	method(setTexture);
}

#endif

#endif
