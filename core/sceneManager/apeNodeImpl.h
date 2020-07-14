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

#ifndef APE_NODEIMPL_H
#define APE_NODEIMPL_H

#include "apeISceneManager.h"
#include "apeEventManagerImpl.h"
#include "apeINode.h"
#include "apeReplica.h"

namespace ape
{
	class NodeImpl : public ape::INode, public ape::Replica
	{
	public:

		NodeImpl(std::string name, bool replicate, std::string ownerID, bool isHost);

		~NodeImpl();
		
		std::string getName() const override;
		
		Vector3 getPosition() const override;
		
		Vector3 getDerivedPosition() const override;
		
		Quaternion getOrientation() const override;
		
		Quaternion getDerivedOrientation() const override;
		
		Vector3 getScale() const override;
		
		Vector3 getDerivedScale() const override;

		bool getChildrenVisibility() override;

		bool isVisible() override;

		bool isFixedYaw() override;
		
		void setParentNode(ape::NodeWeakPtr parentNode) override;

		ape::NodeWeakPtr getParentNode() override;

		void detachFromParentNode() override;

		void addChildNode(ape::NodeWeakPtr node);

		void removeChildNode(ape::NodeWeakPtr childNode);

		std::vector<ape::NodeWeakPtr> getChildNodes() override;

		bool hasChildNode() override;

		bool isChildNode(ape::NodeWeakPtr childNode) override;

		void setPosition(Vector3 position) override;
		
		void setOrientation(Quaternion orientation) override;
		
		void setScale(Vector3 scale) override;

		void setChildrenVisibility(bool visible) override;

		void setVisible(bool visible) override;

		void setFixedYaw(bool fix) override;

		void showBoundingBox(bool show) override;

		void setInheritOrientation(bool enable) override;

		bool isInheritOrientation() override;

		void setInitalState() override;

		void revertToInitalState() override;

		bool isReplicated() override;

		void setOwner(std::string ownerID) override;

		std::string getOwner() override;

		std::string getCreator() override;
		
		void translate(Vector3 transformVector, ape::Node::TransformationSpace nodeTransformSpace) override;
		
		void rotate(Radian angle, Vector3 axis, ape::Node::TransformationSpace nodeTransformSpace) override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;
		
		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;
		
		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:

		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		std::string mName;

		ape::NodeWeakPtr mParentNode;

		ape::NodeWeakPtr mInitParentNode;

		std::vector<ape::NodeWeakPtr> mChildNodes;

		std::string mParentNodeName;

		std::string mInitParentNodeName;

		ape::Vector3 mPosition;

		ape::Quaternion mOrientation;

		ape::Vector3 mScale;

		ape::Vector3 mInitPosition;

		ape::Quaternion mInitOrientation;

		ape::Vector3 mInitScale;

		bool mChildrenVisibility;

		bool mVisibility;

		bool mInitChildVisibility;

		bool mIsFixedYaw;

		bool mIsBoundingBoxVisible;

		bool mIsInheritOrientation;

		bool mIsReplicated;

		std::string mCreatorID;
	};
}

#endif
