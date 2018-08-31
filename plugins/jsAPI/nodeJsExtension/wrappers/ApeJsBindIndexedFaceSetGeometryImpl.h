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

#ifndef APE_JSBIND_INDEXEDFACESETGEOMETRYIMPL_H
#define APE_JSBIND_INDEXEDFACESETGEOMETRYIMPL_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "Ape.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeIndexedFaceSetGeometryImpl.h"
#include "ApeJsBindNodeImpl.h"
#include "ApeManualMaterialJsBind.h"

#ifdef NBIND_CLASS

class IndexedFaceSetJsPtr
{
private:
	Ape::IndexedFaceSetGeometryWeakPtr mPtr;

public:
	IndexedFaceSetJsPtr(Ape::IndexedFaceSetGeometryWeakPtr ptr)
	{
		mPtr = ptr;
	}

	IndexedFaceSetJsPtr(Ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(ptr.lock());
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

	Ape::GeometryWeakPtr getGeometryWeakPtr()
	{
		return std::static_pointer_cast<Ape::Geometry>(mPtr.lock());
	}

	Ape::GeometrySharedPtr getGeometrySharedPtr()
	{
		return this->getGeometryWeakPtr().lock();
	}

	Ape::IndexedFaceSetGeometrySharedPtr getIndexedFaceSetGeometrySharedPtr()
	{
		return std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mPtr.lock());
	}

	Ape::IndexedFaceSetGeometryWeakPtr getIndexedFaceSetGeometryWeakPtr()
	{
		return mPtr;
	}

	// ParentNode

	Ape::NodeWeakPtr getParentNodeWeakPtr()
	{
		return mPtr.lock()->getParentNode();
	}

	void setParentNodeWeakPtr(Ape::NodeWeakPtr parentNode)
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

	const Ape::Entity::Type getType()
	{
		return mPtr.lock()->getType();
	}

	// IIndexedFaceSetGeometry

	Ape::GeometryIndexedFaceSetParameters getParameters()
	{
		return mPtr.lock()->getParameters();
	}

	void setParametersWithMaterial(std::string groupName, Ape::GeometryCoordinates coordinates, Ape::GeometryIndices indices, Ape::GeometryNormals normals, Ape::GeometryColors colors, ManualMaterialJsPtr manualMaterial)
	{
		mPtr.lock()->setParameters(groupName, coordinates, indices, normals, true, colors, Ape::GeometryTextureCoordinates(), manualMaterial.getManualMaterialSharedPtr());
	}

	void setParameters(std::string groupName, Ape::GeometryCoordinates coordinates, Ape::GeometryIndices indices, Ape::GeometryNormals normals, Ape::GeometryColors colors)
	{
		mPtr.lock()->setParameters(groupName, coordinates, indices, normals, true, colors, Ape::GeometryTextureCoordinates(), Ape::MaterialWeakPtr());
	}
};

using namespace Ape;
NBIND_CLASS(GeometryIndexedFaceSetParameters)
{
	construct<>();
	construct<std::string, GeometryCoordinates, GeometryIndices, GeometryNormals, bool, GeometryColors, GeometryTextureCoordinates, MaterialWeakPtr>();

	method(getCoordinates);
	method(getIndices);
	method(getNormals);
	method(getColors);
	method(getTextureCoordinates);

	method(toString);
}

NBIND_CLASS(IndexedFaceSetJsPtr)
{
	construct<Ape::IndexedFaceSetGeometryWeakPtr>();
	construct<Ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getGeometryWeakPtr);
	method(getGeometrySharedPtr);
	method(getIndexedFaceSetGeometryWeakPtr);
	method(getIndexedFaceSetGeometrySharedPtr);

	// ParentNode

	method(getParentNodeWeakPtr);
	method(setParentNodeWeakPtr);

	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	// Entity

	method(getName);
	method(getType);

	// IIndexedFaceSetGeometry

	method(getParameters);
	method(setParametersWithMaterial);
	method(setParameters);
}

#endif

#endif
