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

#ifndef APE_JSBIND_NODEIMPL_H
#define APE_JSBIND_NODEIMPL_H

#include "datatypes/ApeEuler.h"
#include "datatypes/ApeMatrix4.h"
#include "ApeNodeImpl.h"
#include "nbind/nbind.h"
#include "nbind/api.h"

#ifdef NBIND_CLASS

class NodeJsPtr
{
private:
	ape::NodeWeakPtr mPtr;

public:
	NodeJsPtr(ape::NodeWeakPtr ptr)
	{
		mPtr = ptr;
	}

	// Pointers

	const ape::NodeWeakPtr getNodeWeakPtr()
	{
		return mPtr;
	}

	const ape::NodeSharedPtr getNodeSharedPtr()
	{
		return mPtr.lock();
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
		return NodeJsPtr(mPtr.lock()->getParentNode());
	}

	void setParentNodeJsPtr(NodeJsPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode.getNodeWeakPtr());
	}

	// ChildNodes

	std::vector<NodeJsPtr> getChildNodes()
	{
		std::vector<NodeJsPtr> vec;
		if (auto nodeSharedPtr = mPtr.lock())
		{
			for (auto nodeWeakPtr : nodeSharedPtr->getChildNodes())
			{
				vec.push_back(NodeJsPtr(nodeWeakPtr));
			}
		}
		return vec;
	}

	bool hasChildNode()
	{
		return mPtr.lock()->hasChildNode();
	}

	bool isChildNode(NodeJsPtr childNode)
	{
		return mPtr.lock()->isChildNode(childNode.getNodeWeakPtr());
	}

	// INode

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const ape::Vector3 getPosition()
	{
		return mPtr.lock()->getPosition();
	}

	void setPosition(ape::Vector3 position)
	{
		mPtr.lock()->setPosition(position);
	}

	const ape::Vector3 getDerivedPosition()
	{
		return mPtr.lock()->getDerivedPosition();
	}

	const ape::Quaternion getOrientation()
	{
		return mPtr.lock()->getOrientation();
	}

	void setOrientation(ape::Quaternion orientation)
	{
		mPtr.lock()->setOrientation(orientation);
	}

	const ape::Euler getEuler()
	{
		ape::Quaternion q = mPtr.lock()->getOrientation();
		return ape::Euler(q);
	}

	void setEuler(ape::Euler euler)
	{
		mPtr.lock()->setOrientation(euler.toQuaternion());
	}

	const ape::Matrix4 getTransformationMatrix()
	{
		ape::Matrix4 m;
		m.makeTransform(mPtr.lock()->getScale(),
						mPtr.lock()->getOrientation(),
						mPtr.lock()->getPosition());
		return m;
	}

	const ape::Quaternion getDerivedOrientation()
	{
		return mPtr.lock()->getDerivedOrientation();
	}

	const ape::Vector3 getScale()
	{
		return mPtr.lock()->getScale();
	}

	void setScale(ape::Vector3 scale)
	{
		mPtr.lock()->setScale(scale);
	}

	const ape::Vector3 getDerivedScale()
	{
		return mPtr.lock()->getDerivedScale();
	}

	void translate(ape::Vector3 transformVector, ape::Node::TransformationSpace nodeTransformSpace)
	{
		mPtr.lock()->translate(transformVector, nodeTransformSpace);
	}

	void rotate(ape::Radian angle, ape::Vector3 axis, ape::Node::TransformationSpace nodeTransformSpace)
	{
		mPtr.lock()->rotate(angle, axis, nodeTransformSpace);
	}

	void showBoundingBox(bool show)
	{
		mPtr.lock()->showBoundingBox(show);
	}
};

NBIND_CLASS(NodeJsPtr)
{
	construct<ape::NodeWeakPtr>();

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

	method(getEuler);
	method(setEuler);

	method(getTransformationMatrix);

	method(getDerivedOrientation);

	method(getScale);
	method(setScale);

	method(getDerivedScale);

	method(translate);
	method(rotate);
}

#endif

#endif
