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

#include "apeRigidBodyImpl.h"

ape::RigidBodyImpl::RigidBodyImpl(std::string name)
: ape::IRigidBody(name)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mMass = 1.0f;
	mLinearFriction = 0.5f;
	mRollingFriction = 0.0f;
	mSpinningFriction = 0.0f;
	mLinearDamping = 0.0f;
	mAngularDamping = 0.0f;
	mRestitution = 0.0f;
	mRBType = ape::RigidBodyType::DYNAMIC;
	mGeometry = ape::GeometryWeakPtr();
	mGeometryName = std::string();
	mBouyancyEnabled = false;
	mColliderType = ape::RigidBodyColliderType::AUTO;
}

ape::RigidBodyImpl::~RigidBodyImpl()
{
}

/// Physics parameter setters

void ape::RigidBodyImpl::setMass(float mass)
{
	if (mRBType == ape::RigidBodyType::DYNAMIC)
		mMass = mass > 0.0f ? mass : 1.0f;
	else if (mRBType == ape::RigidBodyType::STATIC ||
			mRBType == ape::RigidBodyType::KINEMATIC)
	{
		mMass = 0.0f;
	}

	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::RIGIDBODY_MASS));
}

void ape::RigidBodyImpl::setFriction(float linearFric, float rollingFric, float spinningFric)
{
	mLinearFriction = linearFric;
	mRollingFriction = rollingFric;
	mSpinningFriction = spinningFric;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::RIGIDBODY_FRICTION));
}

void ape::RigidBodyImpl::setDamping(float linearDamping, float angularDamping)
{
	mLinearDamping = linearDamping;
	mAngularDamping = angularDamping;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::RIGIDBODY_DAMPING));
}

void ape::RigidBodyImpl::setRestitution(float rest)
{
	mRestitution = rest;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::RIGIDBODY_RESTITUTION));
}

void ape::RigidBodyImpl::setToDynamic(float mass)
{
	mRBType = ape::RigidBodyType::DYNAMIC;
	this->setMass(mass);
}

void ape::RigidBodyImpl::setToStatic()
{
	mRBType = ape::RigidBodyType::STATIC;
	this->setMass(0.0f);
}

void ape::RigidBodyImpl::setToKinematic()
{
	mRBType = ape::RigidBodyType::KINEMATIC;
	this->setMass(0.0f);
}

void ape::RigidBodyImpl::setBouyancy(bool enable)
{
	mBouyancyEnabled = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::RIGIDBODY_BOUYANCY));
}

bool ape::RigidBodyImpl::bouyancyEnabled()
{
	return mBouyancyEnabled;
}

/// Physics parameter getters

float ape::RigidBodyImpl::getMass()
{
	return mMass;
}

float ape::RigidBodyImpl::getLinearFriction()
{
	return mLinearFriction;
}

float ape::RigidBodyImpl::getRollingFriction()
{
	return mRollingFriction;
}

float ape::RigidBodyImpl::getSpinningFriction()
{
	return mSpinningFriction;
}

float ape::RigidBodyImpl::getLinearDamping()
{
	return mLinearDamping;
}

float ape::RigidBodyImpl::getAngularDamping()
{
	return mAngularDamping;
}

float ape::RigidBodyImpl::getRestitution()
{
	return mRestitution;
}

ape::RigidBodyType ape::RigidBodyImpl::getRBType()
{
	return mRBType;
}

bool ape::RigidBodyImpl::isStatic()
{
	return mRBType == ape::RigidBodyType::STATIC;
}

bool ape::RigidBodyImpl::isKinematic()
{
	return mRBType == ape::RigidBodyType::KINEMATIC;
}

ape::RigidBodyColliderType ape::RigidBodyImpl::getColliderType()
{
	return mColliderType;
}

/// Parent node and geometry

void ape::RigidBodyImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::RIGIDBODY_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

ape::NodeWeakPtr ape::RigidBodyImpl::getParentNode()
{
	return mParentNode;
}

void ape::RigidBodyImpl::setGeometry(ape::GeometryWeakPtr geometry, ape::RigidBodyColliderType colliderType = ape::RigidBodyColliderType::AUTO)
{
	if (auto geometrySP = geometry.lock())
	{
		mGeometry = geometry;
		mGeometryName = geometrySP->getName();
		mColliderType = colliderType;
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::RIGIDBODY_SHAPE));
	}
	else
		mGeometry = ape::GeometryWeakPtr();
}

ape::GeometryWeakPtr ape::RigidBodyImpl::getGeometry()
{
	return mGeometry;
}

std::string ape::RigidBodyImpl::getGeometryName()
{
	return mGeometryName;
}