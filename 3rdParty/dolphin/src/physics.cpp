/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "physics.h"
#include "scenecomponents.h"
#include "Ogre.h"
#include "renderogre.h"

Dolphin::Physics::Physics()
{
}

Dolphin::Physics::~Physics()
{
}

void Dolphin::Physics::Initialize()
{
	int i;
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	
	///-----initialization_end-----

	physicsworldimporter = new btBulletWorldImporter(dynamicsWorld);

	physicsworldimporter->loadFile("../../media/models/physics.bullet");

	mDebugDrawer = new CDebugDraw(RenderOgre::GetSceneManager(), dynamicsWorld);
	mDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamicsWorld->setDebugDrawer(mDebugDrawer);

	//dynamicsWorld->getDebugDrawer();
}

void Dolphin::Physics::GenerateTerrainPhysics(int size, float* data,
	const Ogre::Real& minHeight,
	const Ogre::Real& maxHeight,
	const Ogre::Vector3& position,
	const Ogre::Real& scale)
{
	// Convert height data in a format suitable for the physics engine
	mTerrainHeights = new float[size * size];
	assert(mTerrainHeights != 0);
	for (int i = 0; i < size; i++)
		memcpy(mTerrainHeights + size * i, data + size * (size - i - 1), sizeof(float) * size);

	btScalar heightScale = 1.f;
	btVector3 localScaling(scale, heightScale, scale);

	mTerrainShape = new btHeightfieldTerrainShape(
		size, size, mTerrainHeights, 1 /*ignore*/, minHeight, maxHeight, 1, PHY_FLOAT, true);
	mTerrainShape->setUseDiamondSubdivision(true);
	mTerrainShape->setLocalScaling(localScaling);
	collisionShapes.push_back(mTerrainShape);

	mGroundBody = new btRigidBody(0, new btDefaultMotionState(), mTerrainShape);
	mGroundBody->setFriction(0.8f);
	mGroundBody->setHitFraction(0.8f);
	mGroundBody->setRestitution(0.6f);
	mGroundBody->getWorldTransform().setOrigin(btVector3(position.x, position.z , position.y + 60 ) + btVector3(0, heightScale, 0));
	mGroundBody->getWorldTransform().setRotation(
		btQuaternion(0.7071f, Ogre::Quaternion::IDENTITY.z, Ogre::Quaternion::IDENTITY.y, 0.7071f));
	mGroundBody->setCollisionFlags(mGroundBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	// Add the body to dynamics world
	dynamicsWorld->addRigidBody(mGroundBody);
}

void Dolphin::Physics::StepSimulation(float deltatime)
{
	///-----stepsimulation_start-----
	//dynamicsWorld->stepSimulation(1.f / 60.f, 10);
	//dynamicsWorld->stepSimulation(deltatime*3., 3, deltatime );
	dynamicsWorld->stepSimulation(1.f / 60.f);
	//print positions of all objects
	for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		btQuaternion orientation;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = obj->getWorldTransform();
			//body->getMotionState()->getWorldTransform(trans);
		}
		orientation = trans.getRotation();
		void *userPointer = body->getUserPointer();
		if (userPointer)
		{
			Node* scenenode = static_cast<Ogre::SceneNode *>(userPointer);
			//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			scenenode->setPosition(Ogre::Vector3(trans.getOrigin().getX(), trans.getOrigin().getZ(), -trans.getOrigin().getY()));
			scenenode->setOrientation(Ogre::Quaternion(orientation.getW(), orientation.getX(), orientation.getZ(), -orientation.getY()));
		}
	}
	///-----stepsimulation_end-----
}


void Dolphin::Physics::Clean()
{
	//cleanup in the reverse order of creation/initialization

	///-----cleanup_start-----

	//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j<collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	///-----cleanup_end-----
}

void Dolphin::Physics::CreateBasicRigidBody()
{
	///create a few basic rigid bodies
	groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -56, 0));

	{
		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

}

void Dolphin::Physics::CreateDynamicRigidBody()
{
	//create a dynamic rigidbody

	//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	btCollisionShape* colShape = new btSphereShape(btScalar(1.));
	collisionShapes.push_back(colShape);
	
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar	mass(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(2, 10, 0));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	
	dynamicsWorld->addRigidBody(body);
}

btBulletWorldImporter* Dolphin::Physics::GetPhysicsWorldImporter()
{
	return physicsworldimporter;
}
