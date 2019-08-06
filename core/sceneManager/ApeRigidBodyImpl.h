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

#ifndef APE_RIGIDBODYIMPL_H
#define APE_RIGIDBODYIMPL_H

#include "managers/apeISceneManager.h"
#include "sceneelements/apeIRigidBody.h"
#include "apeEventManagerImpl.h"
#include "apeReplica.h"

namespace ape
{
	class RigidBodyImpl : public ape::IRigidBody, public ape::Replica
	{
	public:
		RigidBodyImpl(std::string name, bool isHostCreated);

		~RigidBodyImpl();

		/// Physics parameter setters

		virtual void setMass(float mass) override;

		virtual void setFriction(float linearFric, float rollingFric, float spinningFric) override;

		virtual void setDamping(float linearDamping, float angularDamping) override;

		virtual void setRestitution(float rest) override;

		virtual void setToDynamic(float mass) override;

		virtual void setToStatic() override;

		virtual void setBouyancy(bool enable, float waterHeight = 0, float liquidDensity = 10.f) override;

		/// Physics parameter getters

		virtual float getMass() override;

		virtual float getLinearFriction() override;

		virtual float getRollingFriction() override;

		virtual float getSpinningFriction() override;

		virtual float getLinearDamping() override;

		virtual float getAngularDamping() override;

		virtual float getRestitution() override;

		virtual ape::RigidBodyType getRBType() override;

		virtual bool isStatic() override;

		virtual bool bouyancyEnabled() override;

		virtual ape::Vector2 getBouyancyProps() override;

		/// Parent node and geometry shape

		void setParentNode(ape::NodeWeakPtr parentNode) override;

		ape::NodeWeakPtr getParentNode() override;

		void setGeometry(ape::GeometryWeakPtr geometry) override;

		ape::GeometryWeakPtr getGeometry() override;

		/// Replica

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		/// Physics parameters

		float mMass;

		float mLinearFriction;

		float mRollingFriction;

		float mSpinningFriction;

		float mLinearDamping;

		float mAngularDamping;

		float mRestitution;

		ape::RigidBodyType mRBType;

		bool mBouyancyEnabled;

		ape::Vector2 mBouyancyProps;

		/// Parent node and geometry shape

		ape::NodeWeakPtr mParentNode;

		std::string mParentNodeName;

		ape::GeometryWeakPtr mGeometry;

		std::string mGeometryName;
	};
}


#endif