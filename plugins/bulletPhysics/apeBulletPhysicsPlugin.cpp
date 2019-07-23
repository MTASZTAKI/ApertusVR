#include <fstream>
#include "apeBulletPhysicsPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include <time.h>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265359
#endif // !M_PI


ape::BulletPhysicsPlugin::BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();

	/// event connecting
	/// we only need NODE, GEOMETRY_BOX, GEOMETRY_PLANE
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));


	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();

	/// init physics simulator (mpDynamicsWorld)
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);


	m_dynamicsWorld->setGravity(btVector3(0., -15., 0.));

	APE_LOG_FUNC_LEAVE();
}

ape::BulletPhysicsPlugin::~BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	/// diconnecting from events
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));

	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::eventCallBack(const ape::Event& event)
{

	if (event.group == ape::Event::Group::NODE)
	{
		if (event.type == ape::Event::Type::NODE_CREATE)
		{
			/*if (auto node = mpSceneManager->getNode(event.subjectName).lock())
			{

			}*/

		}
		
	}
	else if (event.group == ape::Event::Group::GEOMETRY_BOX)
	{
		if(auto primitive = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			std::string geometryName = primitive->getName();
			std::string parentNodeName = "";

			if (auto parentNode = primitive->getParentNode().lock())
				parentNodeName = parentNode->getName();

			if (event.type == ape::Event::Type::GEOMETRY_BOX_CREATE)
			{
				ape::Vector3 size = primitive->getParameters().dimensions * 0.5f;

				btCollisionShape* colShape = new btBoxShape(fromApe(size));
				m_collisionShapes[geometryName] = colShape;

				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					m_parentNodes[geometryName] = parentNode;

					btQuaternion quat;
					quat.setEulerZYX(45.f, 0.f, 0.f);

					btTransform trans = btTransform(fromApe(parentNode->getOrientation())*quat,
													fromApe(parentNode->getPosition()));

					createRigidBody(geometryName, trans, 1.0f, colShape);
				}
				else
				{
					/// making the object with default transformation
					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, 0, 0));

					createRigidBody(geometryName, trans, 1.0f, colShape);
				}
			}
			else if (event.type == ape::Event::Type::GEOMETRY_BOX_DELETE)
			{
				deleteObject(geometryName);
			}
			else if (event.type == ape::Event::Type::GEOMETRY_BOX_PARAMETERS)
			{
				/// creating new collision shape
				ape::Vector3 size = primitive->getParameters().dimensions * 0.5f;

				btCollisionShape* colShape = new btBoxShape(fromApe(size));

				setCollisionShape(geometryName, colShape);
			}
			else if (event.type == ape::Event::Type::GEOMETRY_BOX_PARENTNODE)
			{
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
										
					m_parentNodes[geometryName] = parentNode;

					//for testing
					btQuaternion quat; //quat.setEulerZYX(M_PI/8.f, 0, 0);

					setTransform(geometryName,
								fromApe(parentNode->getOrientation()),//*quat,
								fromApe(parentNode->getPosition()));
					
				}
			}
		}
	}
	else if (event.group == ape::Event::Group::GEOMETRY_SPHERE)
	{
		if (auto primitive = std::static_pointer_cast<ape::ISphereGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			std::string geometryName = primitive->getName();
			std::string parentNodeName = "";

			if (auto parentNode = primitive->getParentNode().lock())
				parentNodeName = parentNode->getName();

			if (event.type == ape::Event::Type::GEOMETRY_SPHERE_CREATE)
			{
				float radius = primitive->getParameters().radius;

				btCollisionShape* colShape = new btSphereShape(radius);
				m_collisionShapes[geometryName] = colShape;

				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					m_parentNodes[geometryName] = parentNode;

					btTransform trans = btTransform(fromApe(parentNode->getOrientation()),
													fromApe(parentNode->getPosition()));
					btScalar mass = 1.f;

					createRigidBody(geometryName, trans, mass, colShape);
				}
				else
				{
					/// making the object with default transformation
					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, 0, 0));
					btScalar mass = 1.f;

					createRigidBody(geometryName, trans, mass, colShape);
				}
			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS)
			{
				/// creating new collision shape
				float radius = primitive->getParameters().radius;

				btCollisionShape* colShape = new btSphereShape(btScalar(radius));
				
				setCollisionShape(geometryName, colShape);
			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_DELETE)
			{
				deleteObject(geometryName);

			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE)
			{
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					
					m_parentNodes[geometryName] = parentNode;

					setTransform(geometryName,
								fromApe(parentNode->getOrientation()),
								fromApe(parentNode->getPosition()));
					
				}
			}
		}

	}
	else if (event.group == ape::Event::GEOMETRY_PLANE)
	{
		if (auto primitive = std::static_pointer_cast<ape::IPlaneGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			std::string geometryName = primitive->getName();
			std::string parentNodeName = "";

			if (auto parentNode = primitive->getParentNode().lock())
				parentNodeName = parentNode->getName();

			if (event.type == ape::Event::GEOMETRY_PLANE_CREATE)
			{
				ape::Vector2 size = primitive->getParameters().size;


				btCollisionShape* colShape = new btBoxShape(btVector3(size.x * 0.5f, 1.0f, size.y * 0.5f));
				m_collisionShapes[geometryName] = colShape;

				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					m_parentNodes[geometryName] = parentNode;

					btTransform trans = btTransform(fromApe(parentNode->getOrientation()),
													fromApe(parentNode->getPosition())-btVector3(0,0.5,0));

					btScalar mass = 0.0f;

					createRigidBody(geometryName, trans, mass, colShape);
					//createCollisionObject(geometryName, trans, colShape);
				}
				else
				{
					/// making the object with default transformation
					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, -0.5f, 0));
					btScalar mass = 0.0f;

					createRigidBody(geometryName, trans, mass, colShape);
					//createCollisionObject(geometryName, trans, colShape);
				}
			}
			else if (event.type == ape::Event::GEOMETRY_PLANE_DELETE)
			{
				deleteObject(geometryName);
			}
			else if (event.type == ape::Event::GEOMETRY_PLANE_PARAMETERS)
			{
				/// set new shape for the plane
				ape::Vector2 size = primitive->getParameters().size;

				btCollisionShape* colShape = new btBoxShape(btVector3(size.x *0.5,1.0f,size.y * 0.5));

				setCollisionShape(geometryName, colShape);
			}
			else if (event.type == ape::Event::GEOMETRY_PLANE_PARENTNODE)
			{
				/// set new transform for the plane
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					m_parentNodes[geometryName] = parentNode;

					setTransform(geometryName,
								 fromApe(parentNode->getOrientation()),
								 fromApe(parentNode->getPosition())-btVector3(0,0.5f,0));
				}
			}
		}
	}
}



void ape::BulletPhysicsPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	time_t t = clock();
	
	printf("\nNum of collision objects: %d\n", m_dynamicsWorld->getNumCollisionObjects());
	while (true)
	{
		/// takes one step of the simulation in the physics engine
		m_dynamicsWorld->stepSimulation(1.f / 60.f, 10);
		
		int i = 0;
		for (auto it = m_collisionObjects.begin(); it != m_collisionObjects.end(); it++)
		{
			std::string geometryName = it->first;
			btRigidBody* body = btRigidBody::upcast(it->second);
			
			btTransform trans;

			if (body && body->getMotionState() && body->getInvMass() != 0)
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
				trans = (it->second)->getWorldTransform();

			if (auto parentNode = m_parentNodes[geometryName].lock())
			{
				btQuaternion rotation = trans.getRotation();

				parentNode->setPosition(fromBullet(trans.getOrigin()));
				parentNode->setOrientation(fromBullet(trans.getRotation()));
			}
			
			

			if (float(clock() - t) > 2000)
				printf("%s %f, %f, %f, mass: %f\n", geometryName, trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
		}

		if (float(clock() - t) > 2000)
			t = clock();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
	}
	APE_LOG_FUNC_LEAVE();
}


/// conversion between bullet3 and ape

ape::Vector3 ape::BulletPhysicsPlugin::fromBullet(const btVector3& btVec)
{
	return ape::Vector3(btVec.getX(), btVec.getY(), btVec.getZ());
}

ape::Quaternion ape::BulletPhysicsPlugin::fromBullet(const btQuaternion& btQuat)
{
	return ape::Quaternion(btQuat.getW(), btQuat.getX(), btQuat.getY(), btQuat.getZ());
}

btVector3 ape::BulletPhysicsPlugin::fromApe(const ape::Vector3& apeVec)
{
	return btVector3(apeVec.x, apeVec.y, apeVec.z);
}

btQuaternion ape::BulletPhysicsPlugin::fromApe(const ape::Quaternion& apeQuat)
{
	return btQuaternion(apeQuat.x, apeQuat.y, apeQuat.z, apeQuat.w);
}


/// sets the transform of a geometry in the physics world
void ape::BulletPhysicsPlugin::setTransform(std::string geometryName, btQuaternion new_orientation, btVector3 new_position)
{

	btTransform trans(new_orientation, new_position);

	if (auto obj = m_collisionObjects[geometryName])
	{
		btRigidBody* body = btRigidBody::upcast(m_collisionObjects[geometryName]);
		if (body && body->getMotionState())
		{
			//body->getMotionState()->setWorldTransform(trans);
			body->setCenterOfMassTransform(trans);
			//body->setWorldTransform(trans);
		}
		else
		{
			obj->setWorldTransform(trans);
		}

	}
	/// else: error...?
}


/// delte object from dynamisWorld
void ape::BulletPhysicsPlugin::deleteObject(std::string geometryName)
{
	if (m_collisionShapes[geometryName])
	{
		delete m_collisionShapes[geometryName];
		m_collisionShapes[geometryName] = nullptr;
	}

	if (auto obj = m_collisionObjects[geometryName])
	{
		btRigidBody* body = btRigidBody::upcast(obj);

		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;

		m_collisionObjects[geometryName] = nullptr;
	}
}

/// setting the shape in the physics world of a geometry
void ape::BulletPhysicsPlugin::setCollisionShape(std::string geometryName, btCollisionShape* colShape)
{
	if (m_collisionShapes[geometryName])
		delete m_collisionShapes[geometryName];

	m_collisionShapes[geometryName] = colShape;

	if (auto obj = m_collisionObjects[geometryName])
	{
		obj->setCollisionShape(colShape);

		btRigidBody* body = btRigidBody::upcast(obj);

		if (body)
		{
			btVector3 localInertia;
			btScalar mass = 1.f / body->getInvMass();
			colShape->calculateLocalInertia(mass, localInertia);
			body->setMassProps(mass, localInertia);
		}

	}
	

}

/// creates a rigidbody in the physics world
void ape::BulletPhysicsPlugin::createRigidBody(std::string geometryName, btTransform trans, btScalar mass, btCollisionShape* shape)
{

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.0f)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	
	btRigidBody* body = new btRigidBody(rbInfo);

	if(mass == 0) body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	
	// for testing
	if (geometryName == "plane")
	{
		body->setRestitution(0.3f); 
		//body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		//body->setActivationState(DISABLE_DEACTIVATION);

		//body->setLinearVelocity(btVector3(0, 10, 0));

		
	}
	else if(geometryName == "sphere1111")
	{
		body->setRestitution(0.7f);
		body->setDamping(0.02f, 0.01f);

		//body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		//body->setActivationState(DISABLE_DEACTIVATION);
		//btTransform tr;
		//tr.setIdentity();
		//tr.setOrigin(btVector3(0, 100, 0));
		//body->setCenterOfMassTransform(tr);
		//body->getMotionState()->setWorldTransform(tr);
		
	}
	else if (geometryName == "box")
	{
		body->setRestitution(0.7f);
		
		
	}

	m_collisionShapes[geometryName] = shape;
	m_collisionObjects[geometryName] = body;

	m_dynamicsWorld->addRigidBody(body);
}


void ape::BulletPhysicsPlugin::createCollisionObject(std::string geometryName, btTransform trans, btCollisionShape* shape)
{
	btCollisionObject* obj = new btCollisionObject();
	obj->setWorldTransform(trans);
	obj->setCollisionShape(shape);

	m_collisionShapes[geometryName] = shape;
	m_collisionObjects[geometryName] = obj;
	
	if (geometryName == "plane")
	{
		obj->setRestitution(0.6f);
		obj->setFriction(1.f);
	}

	m_dynamicsWorld->addCollisionObject(obj);
}


/// this functions don't work yet

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