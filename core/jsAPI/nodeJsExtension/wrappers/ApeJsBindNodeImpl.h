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

#ifndef APE_JSBIND_NODEIMPL_H
#define APE_JSBIND_NODEIMPL_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ApeNodeImpl.h"

#ifdef NBIND_CLASS

class NodeJsPtr
{
private:
	Ape::NodeWeakPtr mPtr;

public:
	NodeJsPtr(Ape::NodeWeakPtr ptr)
	{
		mPtr = ptr;
	}

	// Pointers

	const Ape::NodeWeakPtr getNodeWeakPtr()
	{
		return mPtr;
	}

	const Ape::NodeSharedPtr getNodeSharedPtr()
	{
		return mPtr.lock();
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
		return NodeJsPtr(mPtr.lock()->getParentNode());
	}

	void setParentNodeJsPtr(NodeJsPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode.getNodeWeakPtr());
	}

	// INode

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const Ape::Vector3 getPosition()
	{
		return mPtr.lock()->getPosition();
	}

	void setPosition(Ape::Vector3 position)
	{
		mPtr.lock()->setPosition(position);
	}

	const Ape::Vector3 getDerivedPosition()
	{
		return mPtr.lock()->getDerivedPosition();
	}

	const Ape::Quaternion getOrientation()
	{
		return mPtr.lock()->getOrientation();
	}

	void setOrientation(Ape::Quaternion orientation)
	{
		mPtr.lock()->setOrientation(orientation);
	}

	const Ape::Quaternion getDerivedOrientation()
	{
		return mPtr.lock()->getDerivedOrientation();
	}

	const Ape::Vector3 getScale()
	{
		return mPtr.lock()->getScale();
	}

	void setScale(Ape::Vector3 scale)
	{
		mPtr.lock()->setScale(scale);
	}

	const Ape::Vector3 getDerivedScale()
	{
		return mPtr.lock()->getDerivedScale();
	}

	void translate(Ape::Vector3 transformVector, Ape::Node::TransformationSpace nodeTransformSpace)
	{
		mPtr.lock()->translate(transformVector, nodeTransformSpace);
	}

	void rotate(Ape::Radian angle, Ape::Vector3 axis, Ape::Node::TransformationSpace nodeTransformSpace)
	{
		mPtr.lock()->rotate(angle, axis, nodeTransformSpace);
	}
};

NBIND_CLASS(NodeJsPtr)
{
	construct<Ape::NodeWeakPtr>();

	// Pointers

	method(getNodeWeakPtr);
	method(getNodeSharedPtr);

	// ParentNode

	method(getParentNodeWeakPtr);
	method(setParentNodeWeakPtr);

	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	// INode

	method(getName);

	method(getPosition);
	method(setPosition);

	method(getDerivedPosition);

	method(getOrientation);
	method(setOrientation);

	method(getDerivedOrientation);

	method(getScale);
	method(setScale);

	method(getDerivedScale);

	method(translate);
	method(rotate);
}

#endif

#endif
