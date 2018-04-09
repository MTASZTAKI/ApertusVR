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

#ifndef APE_NODEIMPL_H
#define APE_NODEIMPL_H

#include "ApeINode.h"
#include "ApeEventManagerImpl.h"
#include "ApeIScene.h"
#include "ApeReplica.h"

namespace Ape
{
	class NodeImpl : public Ape::INode, public Ape::Replica
	{
	public:

		NodeImpl(std::string name, bool isHostCreated);

		~NodeImpl();
		
		std::string getName() const override;
		
		Vector3 getPosition() const override;
		
		Vector3 getDerivedPosition() const override;
		
		Quaternion getOrientation() const override;
		
		Quaternion getDerivedOrientation() const override;
		
		Vector3 getScale() const override;
		
		Vector3 getDerivedScale() const override;

		bool getChildrenVisibility() override;

		bool isFixedYaw() override;
		
		void setParentNode(Ape::NodeWeakPtr parentNode) override;

		Ape::NodeWeakPtr getParentNode() override;
		
		void setPosition(Vector3 position) override;
		
		void setOrientation(Quaternion orientation) override;
		
		void setScale(Vector3 scale) override;

		void setChildrenVisibility(bool visible) override;

		void setFixedYaw(bool fix) override;

		void showBoundingBox(bool show) override;
		
		void translate(Vector3 transformVector, Ape::Node::TransformationSpace nodeTransformSpace) override;
		
		void rotate(Radian angle, Vector3 axis, Ape::Node::TransformationSpace nodeTransformSpace) override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;
		
		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;
		
		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		Ape::EventManagerImpl* mpEventManagerImpl;

		Ape::IScene* mpScene;

		std::string mName;

		Ape::NodeWeakPtr mParentNode;

		std::string mParentNodeName;

		Ape::Vector3 mPosition;

		Ape::Quaternion mOrientation;

		Ape::Vector3 mScale;

		bool mChildrenVisibility;

		bool mIsFixedYaw;

		bool mIsBoundingBoxVisible;
	};
}

#endif
