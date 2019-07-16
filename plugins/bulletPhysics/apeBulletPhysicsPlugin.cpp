#include <fstream>
#include "apeBulletPhysicsPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"


ape::BulletPhysicsPlugin::BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();

	/// init physics simulator (dynamicsWorld)
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	

	dynamicsWorld->setGravity(btVector3(0., -10., 0.));

	APE_LOG_FUNC_LEAVE();
}

ape::BulletPhysicsPlugin::~BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::eventCallBack(const ape::Event& event)
{
	
}

void ape::BulletPhysicsPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	/// ** init rigid body obejcts here ?**

	/// i.e. making a ground shape and a sphere
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));

		btScalar mass(0.);

		/// rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		/// using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		/// add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

	{
		///create a dynamic rigidbody

		///btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		///rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		/// init postion for the phere
		startTransform.setOrigin(btVector3(1, 2, 1));

		/// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		/// add to the dynamicsWorld, which take care of the sphere's transform (translation, rotation)
		dynamicsWorld->addRigidBody(body);
	}

	{
		btCollisionShape* colShape = new btBoxShape(btVector3(1., 1., 1.));
		collisionShapes.push_back(colShape);
		
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.0)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(1, 4, 1));

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		/// add to the dynamicsWorld, which take care of the sphere's transform (translation, rotation)
		dynamicsWorld->addRigidBody(body);
	}

	/// creating a constaint for the two dynamic body
	{
		btRigidBody* body1 = btRigidBody::upcast(dynamicsWorld->getCollisionObjectArray()[1]);
		btRigidBody* body2 = btRigidBody::upcast(dynamicsWorld->getCollisionObjectArray()[2]);
		
		btVector3 pivotIn1(0.f, -1.f, 0.f);
		btVector3 pivotIn2(0.f,  1.f, 0.f);

		btVector3 axisIn1(1, 0, 0);
		btVector3 axisIn2(1, 0, 0);

		btHingeConstraint* hinge = new btHingeConstraint(*body1, *body2,
														pivotIn1, pivotIn2,
														axisIn1, axisIn2);

		btJointFeedback* fb = new btJointFeedback();
		hinge->setJointFeedback(fb);

		dynamicsWorld->addConstraint(hinge, true);
	}

	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		/// takes one step of the simulation in the physics engine
		dynamicsWorld->stepSimulation(1.f / 60.f, 1);

		for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			
			if (body && body->getMotionState())
				body->getMotionState()->getWorldTransform(trans);
			else
				trans = obj->getWorldTransform();

			btVector3 pos = trans.getOrigin();
			btQuaternion rotate = trans.getRotation();

			/// do some rendering with pos and rotate...?
		}


		std::this_thread::sleep_for (std::chrono::milliseconds(20)); 
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
