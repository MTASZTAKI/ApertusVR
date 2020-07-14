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

#ifndef APE_INODE_H
#define APE_INODE_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ape.h"
#include "apeQuaternion.h"
#include "apeVector3.h"
#include "apeMatrix4.h"

namespace ape
{
	namespace Node
	{
		enum TransformationSpace
		{
			LOCAL,
			PARENT,
			WORLD,
			INVALID
		};
	}

	class INode
	{
	protected:
		virtual ~INode() {};

	public:
		virtual std::string getName() const = 0;

		virtual Vector3 getPosition() const = 0;

		virtual Vector3 getDerivedPosition() const = 0;

		virtual Quaternion getOrientation() const = 0;

		virtual Quaternion getDerivedOrientation() const = 0;

		virtual Vector3 getScale() const = 0;

		virtual Vector3 getDerivedScale() const = 0;

//		virtual Matrix4 getModelMatrix() const = 0;
//
//		virtual Matrix4 getDerivedModelMatrix() const = 0;

		virtual bool getChildrenVisibility() = 0;

		virtual bool isVisible() = 0;

		virtual bool isFixedYaw() = 0;

		virtual void setParentNode(ape::NodeWeakPtr parentNode) = 0;

		virtual void detachFromParentNode() = 0;

		virtual ape::NodeWeakPtr getParentNode() = 0;

		virtual std::vector<ape::NodeWeakPtr> getChildNodes() = 0;

		virtual bool hasChildNode() = 0;

		virtual bool isChildNode(ape::NodeWeakPtr childNode) = 0;

		virtual void setPosition(Vector3 position) = 0;

		virtual void setOrientation(Quaternion orientation) = 0;

		virtual void setScale(Vector3 scale) = 0;

		virtual void translate(Vector3 transformVector, ape::Node::TransformationSpace nodeTransformSpace) = 0;

		virtual void rotate(Radian angle, Vector3 axis, ape::Node::TransformationSpace nodeTransformSpace) = 0;

		virtual void setChildrenVisibility(bool visible) = 0;

		virtual void setVisible(bool visible) = 0;

		virtual void setFixedYaw(bool fix) = 0;

		virtual void showBoundingBox(bool show) = 0;

		virtual void setInheritOrientation(bool enable) = 0;

		virtual bool isInheritOrientation() = 0;

		virtual void setInitalState() = 0;

		virtual void revertToInitalState() = 0;

		virtual bool isReplicated() = 0;

		virtual void setOwner(std::string ownerID) = 0;

		virtual std::string getOwner() = 0;

		virtual std::string getCreator() = 0;
	};
}

#endif
