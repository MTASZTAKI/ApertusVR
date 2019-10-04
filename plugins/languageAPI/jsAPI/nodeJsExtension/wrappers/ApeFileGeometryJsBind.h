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

#ifndef APE_FILEGEOMETRYJSBIND_H
#define APE_FILEGEOMETRYJSBIND_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ape.h"
#include "apeIFileGeometry.h"
#include "apeJsBindNodeImpl.h"
#include "apeManualMaterialJsBind.h"

#ifdef NBIND_CLASS

class FileGeometryJsPtr
{
private:
	ape::FileGeometryWeakPtr mPtr;

public:
	FileGeometryJsPtr(ape::FileGeometryWeakPtr ptr)
	{
		mPtr = ptr;
	}

	FileGeometryJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::IFileGeometry>(ptr.lock());
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

	ape::GeometryWeakPtr getGeometryWeakPtr()
	{
		return std::static_pointer_cast<ape::Geometry>(mPtr.lock());
	}

	ape::GeometrySharedPtr getGeometrySharedPtr()
	{
		return this->getGeometryWeakPtr().lock();
	}

	ape::FileGeometrySharedPtr getFileGeometrySharedPtr()
	{
		return std::static_pointer_cast<ape::IFileGeometry>(mPtr.lock());
	}

	ape::FileGeometryWeakPtr getFileGeometryWeakPtr()
	{
		return mPtr;
	}

	// ParentNode

	ape::NodeWeakPtr getParentNodeWeakPtr()
	{
		return mPtr.lock()->getParentNode();
	}

	void setParentNodeWeakPtr(ape::NodeWeakPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode);
	}

	NodeJsPtr getParentNodeJsPtr()
	{
		return NodeJsPtr(getParentNodeWeakPtr());
	}

	void setParentNodeJsPtr(NodeJsPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode.getNodeWeakPtr());
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

	// IFileGeometry

	void setFileName(std::string fileName)
	{
		mPtr.lock()->setFileName(fileName);
	}

	void setManualMaterial(ManualMaterialJsPtr manualMaterial)
	{
		mPtr.lock()->setMaterial(manualMaterial.getManualMaterialSharedPtr());
	}
};

using namespace ape;

NBIND_CLASS(FileGeometryJsPtr)
{
	construct<ape::FileGeometryWeakPtr>();
	construct<ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getGeometryWeakPtr);
	method(getGeometrySharedPtr);
	method(getFileGeometryWeakPtr);
	method(getFileGeometrySharedPtr);

	// ParentNode

	method(getParentNodeWeakPtr);
	method(setParentNodeWeakPtr);

	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	// Entity

	method(getName);
	method(getType);

	// IFileGeometry

	method(setFileName);
	method(setManualMaterial);
}

#endif

#endif
