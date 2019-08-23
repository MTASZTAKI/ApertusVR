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


#ifndef APE_IRIGIDBODY_H
#define APE_IRIGIDBODY_H

#include <array>
#include <memory>
#include <string>
#include <vector>
#include "ape.h"
#include "../datatypes/apeGeometry.h"
#include "../datatypes/apeEntity.h"
#include "../datatypes/apeVector3.h"
#include "apeINode.h"

namespace ape
{
	enum RigidBodyType
	{
		DYNAMIC,
		STATIC,
		KINEMATIC
	};

	enum RigidBodyColliderType
	{
		AUTO,
		TRIANGLE_MESH,
		CONVEX_HULL
	};

    class IRigidBody : public ape::Entity
    {
    protected:
        IRigidBody(std::string name)
		: Entity(name,Entity::Type::RIGIDBODY)
		{}

        virtual ~IRigidBody() {}

    public:
        
		/// Physics parameter setters

		virtual void setMass(float mass) = 0;

		virtual void setFriction(float linearFric, float rollingFric, float spinningFric) = 0;

		virtual void setDamping(float linearDamping, float angularDamping) = 0;

		virtual void setRestitution(float rest) = 0;

		virtual void setToDynamic(float mass) = 0;

		virtual void setToStatic() = 0;

		virtual void setToKinematic() = 0;

		virtual void setBouyancy(bool enable) = 0;

		/// Physics parameter getters

		virtual float getMass() = 0;

		virtual float getLinearFriction() = 0;

		virtual float getRollingFriction() = 0;

		virtual float getSpinningFriction() = 0;

		virtual float getLinearDamping() = 0;

		virtual float getAngularDamping() = 0;

		virtual float getRestitution() = 0;

		virtual RigidBodyType getRBType() = 0;

		virtual bool isStatic() = 0;

		virtual bool isKinematic() = 0;

		virtual bool bouyancyEnabled() = 0;

		virtual RigidBodyColliderType getColliderType() = 0;

		/// Parent node and geometry (for shape)
        
        virtual void setParentNode(ape::NodeWeakPtr parentNode) = 0;

		virtual ape::NodeWeakPtr getParentNode() = 0;
        
        virtual void setGeometry(
			ape::GeometryWeakPtr geometry,
			ape::RigidBodyColliderType colliderType = ape::RigidBodyColliderType::AUTO
		) = 0;

        virtual ape::GeometryWeakPtr getGeometry() = 0; 
	};
}

#endif