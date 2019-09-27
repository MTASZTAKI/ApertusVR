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

#ifndef APE_FILETEXTUREJSBIND_H
#define APE_FILETEXTUREJSBIND_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ape.h"
#include "sceneelements/apeIFileTexture.h"
#include "apeJsBindNodeImpl.h"
#include "apeManualMaterialJsBind.h"

#ifdef NBIND_CLASS

class FileTextureJsPtr
{
private:
	ape::FileTextureWeakPtr mPtr;

public:
	FileTextureJsPtr(ape::FileTextureWeakPtr ptr)
	{
		mPtr = ptr;
	}

	FileTextureJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::IFileTexture>(ptr.lock());
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

	ape::TextureWeakPtr getTextureWeakPtr()
	{
		return std::static_pointer_cast<ape::Texture>(mPtr.lock());
	}

	ape::TextureSharedPtr getTextureSharedPtr()
	{
		return this->getTextureWeakPtr().lock();
	}

	ape::FileTextureSharedPtr getFileTextureSharedPtr()
	{
		return std::static_pointer_cast<ape::IFileTexture>(mPtr.lock());
	}

	ape::FileTextureWeakPtr getFileTextureWeakPtr()
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

	// IFileTexture

	void setFileName(std::string fileName)
	{
		mPtr.lock()->setFileName(fileName);
	}

};

using namespace ape;

NBIND_CLASS(FileTextureJsPtr)
{
	construct<ape::FileTextureWeakPtr>();
	construct<ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getTextureWeakPtr);
	method(getTextureSharedPtr);
	method(getFileTextureWeakPtr);
	method(getFileTextureSharedPtr);


	// Entity

	method(getName);
	method(getType);

	// IFileTexture

	method(setFileName);
}

#endif

#endif
