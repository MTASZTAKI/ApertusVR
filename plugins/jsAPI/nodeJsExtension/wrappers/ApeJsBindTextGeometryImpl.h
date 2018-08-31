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

#ifndef APE_JSBIND_TEXTGEOMETRYIMPL_H
#define APE_JSBIND_TEXTGEOMETRYIMPL_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "Ape.h"
#include "ApeNodeImpl.h"
#include "ApeITextGeometry.h"
#include "ApeTextGeometryImpl.h"
#include "ApeJsBindNodeImpl.h"
#include <typeinfo>

#ifdef NBIND_CLASS

class TextJsPtr
{
private:
	Ape::TextGeometryWeakPtr mPtr;

public:
	TextJsPtr(Ape::TextGeometryWeakPtr ptr)
	{
		mPtr = ptr;
	}

	TextJsPtr(Ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<Ape::ITextGeometry>(ptr.lock());
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

	Ape::TextGeometryWeakPtr getTextGeometryWeakPtr()
	{
		return mPtr;
	}

	Ape::TextGeometrySharedPtr getTextGeometrySharedPtr()
	{
		return std::static_pointer_cast<Ape::ITextGeometry>(mPtr.lock());
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

	// ITextGeometry

	std::string getCaption()
	{
		return mPtr.lock()->getCaption();
	}

	void setCaption(std::string caption)
	{
		mPtr.lock()->setCaption(caption);
	}

	bool isVisible()
	{
		return mPtr.lock()->isVisible();
	}

	void setVisible(bool enabled)
	{
		mPtr.lock()->setVisible(enabled);
	}

};

NBIND_CLASS(TextJsPtr)
{
	construct<Ape::TextGeometryWeakPtr>();
	construct<Ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getGeometryWeakPtr);
	method(getGeometrySharedPtr);
	method(getTextGeometryWeakPtr);
	method(getTextGeometrySharedPtr);

	// ParentNode

	method(getParentNodeWeakPtr);
	method(setParentNodeWeakPtr);

	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	// Entity

	method(getName);
	method(getType);

	// ITextGeometry

	method(getCaption, nbind::Strict());
	method(setCaption);

	method(isVisible);
	method(setVisible);
}

#endif

#endif
