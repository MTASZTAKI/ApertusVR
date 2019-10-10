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

#ifndef APE_JSBIND_CLONEGEOMETRYIMPL_H
#define APE_JSBIND_CLONEGEOMETRYIMPL_H

#include "ape.h"
#include "apeICloneGeometry.h"
#include "nbind/nbind.h"
#include "nbind/api.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeJsBindNodeImpl.h"
#include "apeManualMaterialJsBind.h"
#include "ApeJsBindIndexedFaceSetGeometryImpl.h"

#ifdef NBIND_CLASS

class CloneGeometryJsPtr
{
private:
	ape::CloneGeometryWeakPtr mPtr;

public:
	CloneGeometryJsPtr(ape::CloneGeometryWeakPtr ptr)
	{
		mPtr = ptr;
	}

	CloneGeometryJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::ICloneGeometry>(ptr.lock());
	}

	/// Pointers

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

	ape::CloneGeometryWeakPtr getCloneGeometryWeakPtr()
	{
		return mPtr;
	}

	ape::CloneGeometrySharedPtr getCloneGeometrySharedPtr()
	{
		return std::static_pointer_cast<ape::ICloneGeometry>(mPtr.lock());
	}

	/// ParentNode

	ape::NodeWeakPtr getParentNodeWeakPtr()
	{
		return mPtr.lock()->getParentNode();
	}

	ape::NodeSharedPtr getParentNodeShared()
	{
		return mPtr.lock()->getParentNode().lock();
	}

	NodeJsPtr getParentNodeJsPtr()
	{
		return NodeJsPtr(this->getParentNodeWeakPtr());
	}

	void setParentNodeJsPtr(NodeJsPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode.getNodeWeakPtr());
	}

	/// Entity

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const ape::Entity::Type getType()
	{
		return mPtr.lock()->getType();
	}

	/// SourceGeometry

	ape::GeometryWeakPtr getSourceGeometryWeakPtr()
	{
		return mPtr.lock()->getSourceGeometry();
	}

	ape::GeometrySharedPtr getSourceGeometrySharedPtr()
	{
		return mPtr.lock()->getSourceGeometry().lock();
	}

	ape::IndexedFaceSetGeometryWeakPtr getSourceIndexedFaceSetGeometryWeakPtr()
	{
		return std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mPtr.lock()->getSourceGeometry().lock());
	}

	ape::IndexedFaceSetGeometrySharedPtr getSourceIndexedFaceSetGeometrySharedPtr()
	{
		return std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(mPtr.lock()->getSourceGeometry().lock());
	}

	IndexedFaceSetJsPtr getSourceIndexedFaceSetJsPtr()
	{
		return IndexedFaceSetJsPtr(getSourceIndexedFaceSetGeometryWeakPtr());
	}

	void setSourceIndexedFaceSetJsPtr(IndexedFaceSetJsPtr parentIfs)
	{
		mPtr.lock()->setSourceGeometry(parentIfs.getIndexedFaceSetGeometryWeakPtr());
	}

	/// SourceGeometryGroup

	void setSourceGeometryGroupName(std::string geometryGroupName)
	{
		mPtr.lock()->setSourceGeometryGroupName(geometryGroupName);
	}

	std::string getSourceGeometryGroupName()
	{
		return mPtr.lock()->getSourceGeometryGroupName();
	}

};

using namespace ape;
NBIND_CLASS(CloneGeometryJsPtr)
{
	construct<ape::CloneGeometryWeakPtr>();
	construct<ape::EntityWeakPtr>();

	/// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getGeometryWeakPtr);
	method(getGeometrySharedPtr);
	method(getCloneGeometryWeakPtr);
	method(getCloneGeometrySharedPtr);

	/// ParentNode

	method(getParentNodeWeakPtr);
	method(getParentNodeShared);
	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	/// Entity

	method(getName);
	method(getType);

	/// SourceGeometry

	method(getSourceGeometryWeakPtr);
	method(getSourceGeometrySharedPtr);
	method(getSourceIndexedFaceSetGeometryWeakPtr);
	method(getSourceIndexedFaceSetGeometrySharedPtr);
	method(getSourceIndexedFaceSetJsPtr);
	method(setSourceIndexedFaceSetJsPtr);

	/// SourceGeometryGroup

	method(setSourceGeometryGroupName);
	method(getSourceGeometryGroupName);

}

#endif

#endif

