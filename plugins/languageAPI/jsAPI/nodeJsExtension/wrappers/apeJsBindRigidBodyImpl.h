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

#ifndef APE_JSBIND_RIGIDBODYIMPL_H
#define APE_JSBIND_RIGIDBODYIMPL_H

#include "ape.h"
#include "apeIRigidBody.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeICloneGeometry.h"
#include "nbind/nbind.h"
#include "nbind/api.h"
#include "apeJsBindNodeImpl.h"
#include "apeJsBindCloneGeometryImpl.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "apeManualMaterialJsBind.h"

#ifdef NBIND_CLASS

class RigidBodyJsPtr
{
private:
	ape::RigidBodyWeakPtr mPtr;

public:

	RigidBodyJsPtr(ape::RigidBodyWeakPtr ptr)
	{
		mPtr = ptr;
	}

	RigidBodyJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::IRigidBody>(ptr.lock());
	}

	/// Pointers

	const ape::EntityWeakPtr getEntityWeakPtr()
	{
		return std::static_pointer_cast<ape::Entity>(mPtr.lock());
	}

	const ape::EntitySharedPtr getEntitySharedPtr()
	{
		return this->getEntityWeakPtr().lock();
	}

	ape::RigidBodyWeakPtr getRigidBodyWeakPtr()
	{
		return mPtr;
	}

	ape::RigidBodySharedPtr getRigidBodySharedPtr()
	{
		return this->getRigidBodyWeakPtr().lock();
	}

	/// ParentNode

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
		return NodeJsPtr(getParentNodeWeakPtr());
	}

	void setParentNodeJsPtr(NodeJsPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode.getNodeWeakPtr());
	}

	/// IndexedFaceSetGeometry

	ape::IndexedFaceSetGeometryWeakPtr getIndexedFaceSetGeometryWeakPtr()
	{
		return std::static_pointer_cast<IIndexedFaceSetGeometry>(mPtr.lock()->getGeometry().lock());
	}

	void setIndexedFaceSetGeometryWeakPtr(ape::IndexedFaceSetGeometryWeakPtr geometry)
	{
		mPtr.lock()->setGeometry(geometry);
	}

	IndexedFaceSetJsPtr getIndexedFaceSetGeometryJsPtr()
	{
		return IndexedFaceSetJsPtr(mPtr.lock()->getGeometry());
	}

	void setIndexedFaceSetGeometryJsPtr(IndexedFaceSetJsPtr geometry)
	{
		mPtr.lock()->setGeometry(geometry.getGeometryWeakPtr());
	}

	/// CloneGeometry

	ape::CloneGeometryWeakPtr getCloneGeometryWeakPtr()
	{
		return std::static_pointer_cast<ICloneGeometry>(mPtr.lock()->getGeometry().lock());
	}

	void setCloneGeometryWeakPtr(ape::CloneGeometryWeakPtr geometry)
	{
		mPtr.lock()->setGeometry(geometry);
	}

	CloneGeometryJsPtr getCloneGeometryJsPtr()
	{
		return CloneGeometryJsPtr(mPtr.lock()->getGeometry());
	}

	void setCloneGeometryJsPtr(CloneGeometryJsPtr geometry)
	{
		mPtr.lock()->setGeometry(geometry.getGeometryWeakPtr());
	}
	/// Entity

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const ape::Entity::Type getType()
	{
		return mPtr.lock()->getType();
	}

	/// IRigidBody getter

	float getMass()
	{
		return mPtr.lock()->getMass();
	}

	float getLinearFriction()
	{
		return mPtr.lock()->getLinearFriction();
	}

	float getRollingFriction()
	{
		return mPtr.lock()->getRollingFriction();
	}

	float getSpinningFriction()
	{
		return mPtr.lock()->getSpinningFriction();
	}

	float getLinearDamping()
	{
		return mPtr.lock()->getLinearDamping();
	}

	float getAngularDamping()
	{
		return mPtr.lock()->getAngularDamping();
	}

	float getRestitution()
	{
		return mPtr.lock()->getRestitution();
	}

	RigidBodyType getRBType()
	{
		return mPtr.lock()->getRBType();
	}

	bool isStatic()
	{
		return mPtr.lock()->isStatic();
	}

	bool isKinematic()
	{
		return mPtr.lock()->isKinematic();
	}

	bool bouyancyEnabled()
	{
		return mPtr.lock()->bouyancyEnabled();
	}

	/// IRigidBody setter

	void setMass(float mass)
	{
		mPtr.lock()->setMass(mass);
	}

	void setFriction(float linearFric, float rollingFric, float spinningFric)
	{
		mPtr.lock()->setFriction(linearFric, rollingFric, spinningFric);
	}

	void setDamping(float linearDamping, float angularDamping)
	{
		mPtr.lock()->setDamping(linearDamping, angularDamping);
	}

	void setRestitution(float rest)
	{
		mPtr.lock()->setRestitution(rest);
	}

	void setToDynamic(float mass)
	{
		mPtr.lock()->setToDynamic(mass);
	}

	void setToStatic()
	{
		mPtr.lock()->setToStatic();
	}

	void setToKinematic()
	{
		mPtr.lock()->setToKinematic();
	}

	void setBouyancy(bool enable)
	{
		mPtr.lock()->setBouyancy(enable);
	}
};

using namespace ape;
NBIND_CLASS(RigidBodyJsPtr)
{
	construct<ape::RigidBodyWeakPtr>();
	construct<ape::EntityWeakPtr>();

	/// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getRigidBodyWeakPtr);
	method(getRigidBodySharedPtr);

	/// ParentNode

	method(getParentNodeWeakPtr);
	method(setParentNodeWeakPtr);

	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	/// Geoemtry

	method(getIndexedFaceSetGeometryWeakPtr);
	method(setIndexedFaceSetGeometryWeakPtr);
	method(getIndexedFaceSetGeometryJsPtr);
	method(setIndexedFaceSetGeometryJsPtr);

	method(getCloneGeometryWeakPtr);
	method(setCloneGeometryWeakPtr);
	method(getCloneGeometryJsPtr);
	method(setCloneGeometryJsPtr);

	/// Entity

	method(getName);
	method(getType);

	/// IRigidBody getters

	method(getMass);
	method(getLinearFriction);
	method(getRollingFriction);
	method(getSpinningFriction);
	method(getLinearDamping);
	method(getAngularDamping);
	method(getRestitution);
	method(getRBType);
	method(isStatic);
	method(isKinematic);
	method(bouyancyEnabled);

	/// IRigidBody setters

	method(setMass);
	method(setFriction);
	method(setDamping);
	method(setRestitution);
	method(setToDynamic);
	method(setToStatic);
	method(setToKinematic);
	method(setBouyancy);
}



#endif

#endif
