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

#ifndef APE_INDEXEDLINESETGEOMETRYJSBIND_H
#define APE_INDEXEDLINESETGEOMETRYJSBIND_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ape.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeJsBindNodeImpl.h"

#ifdef NBIND_CLASS

class IndexedLineSetJsPtr
{
private:
	ape::IndexedLineSetGeometryWeakPtr mPtr;

public:
	IndexedLineSetJsPtr(ape::IndexedLineSetGeometryWeakPtr ptr)
	{
		mPtr = ptr;
	}

	IndexedLineSetJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(ptr.lock());
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

	ape::IndexedLineSetGeometrySharedPtr getIndexedLineSetGeometrySharedPtr()
	{
		return std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mPtr.lock());
	}

	ape::IndexedLineSetGeometryWeakPtr getIndexedLineSetGeometryWeakPtr()
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

	// IIndexedLineSetGeometry

	ape::GeometryIndexedLineSetParameters getParameters()
	{
		return mPtr.lock()->getParameters();
	}

	void setParameters(ape::GeometryCoordinates coordinates, ape::GeometryIndices indices, ape::Color color)
	{
		mPtr.lock()->setParameters(coordinates, indices, color);
	}
};

using namespace ape;
NBIND_CLASS(GeometryIndexedLineSetParameters)
{
	construct<>();
	construct<GeometryCoordinates, GeometryIndices, Color>();

	method(getCoordinates);
	method(getIndices);
	method(getColor);

	method(toString);
}

NBIND_CLASS(IndexedLineSetJsPtr)
{
	construct<ape::IndexedLineSetGeometryWeakPtr>();
	construct<ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getGeometryWeakPtr);
	method(getGeometrySharedPtr);
	method(getIndexedLineSetGeometryWeakPtr);
	method(getIndexedLineSetGeometrySharedPtr);

	// ParentNode

	method(getParentNodeWeakPtr);
	method(setParentNodeWeakPtr);

	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	// Entity

	method(getName);
	method(getType);

	// IIndexedLineSetGeometry

	method(getParameters);
	method(setParameters);
}

#endif

#endif
