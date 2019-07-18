#include <fstream>
#include "apeBulletPhysicsPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"


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


	m_dynamicsWorld->setGravity(btVector3(0., -100., 0.));

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

					btTransform trans = btTransform(fromApe(parentNode->getOrientation()),
													fromApe(parentNode->getPosition()));

					createObject(geometryName, trans, 0.0f, colShape);
				}
				else
				{
					/// making the object with default transformation
					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, 0, 0));

					createObject(geometryName, trans, 1.0f, colShape);
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

				setShape(geometryName, colShape);
			}
			else if (event.type == ape::Event::Type::GEOMETRY_BOX_PARENTNODE)
			{
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					/// set new transform for the plane
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
					createObject(geometryName, trans, 0.0f, colShape);
				}
				else
				{
					/// making the object with default transformation
					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, 0, 0));

					createObject(geometryName, trans, 1.0f, colShape);
				}
			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS)
			{
				/// creating new collision shape
				float radius = primitive->getParameters().radius;

				btCollisionShape* colShape = new btSphereShape(btScalar(radius));
				
				setShape(geometryName, colShape);
			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_DELETE)
			{
				deleteObject(geometryName);

			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE)
			{
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					/// set new transform for the plane
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
													fromApe(parentNode->getPosition()));
					createObject(geometryName, trans, 0.0f, colShape);
				}
				else
				{
					/// making the object with default transformation
					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, 0, 0));

					createObject(geometryName, trans, 0.0f, colShape);
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

				btCollisionShape* colShape = new btBoxShape(btVector3(size.x,1.0f,size.y));

				setShape(geometryName, colShape);
			}
			else if (event.type == ape::Event::GEOMETRY_BOX_PARENTNODE)
			{
				/// set new transform for the plane
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
}



/// sets the transform of a geometry in the physics world
void ape::BulletPhysicsPlugin::setTransform(std::string geometryName, btQuaternion new_orientation, btVector3 new_position)
{
	btTransform trans(new_orientation,new_position);

	if (auto obj = m_collisionObjects[geometryName])
	{
		btDefaultMotionState* motionState = new btDefaultMotionState(trans);
		btRigidBody* body = btRigidBody::upcast(m_collisionObjects[geometryName]);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
			body->setMotionState(motionState);
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
void ape::BulletPhysicsPlugin::setShape(std::string geometryName, btCollisionShape* colShape)
{
	if (m_collisionShapes[geometryName])
		delete m_collisionShapes[geometryName];

	m_collisionShapes[geometryName] = colShape;

	if (m_collisionObjects[geometryName])
		m_collisionObjects[geometryName]->setCollisionShape(colShape);
}

/// creates objects in the physics world
void ape::BulletPhysicsPlugin::createObject(std::string geometryName, btTransform trans, btScalar mass, btCollisionShape* shape)
{

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.0f)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	m_collisionObjects[geometryName] = body;

	m_dynamicsWorld->addRigidBody(body);
}



void ape::BulletPhysicsPlugin::Init()
{
	APE_LOG_FUNC_ENTER();

	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		/// takes one step of the simulation in the physics engine
		m_dynamicsWorld->stepSimulation(1.f / 60.f, 1);

		for (auto it = m_collisionObjects.begin(); it != m_collisionObjects.end(); it++)
		{
			std::string geometryName = it->first;
			btRigidBody* body = btRigidBody::upcast(it->second);

			btTransform trans;

			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
				trans = (it->second)->getWorldTransform();

			/*printf("%s position: %f,%f,%f \n", geometryName, trans.getOrigin().getX(),
				trans.getOrigin().getY(),
				trans.getOrigin().getZ());*/

			if (auto parentNode = m_parentNodes[geometryName].lock())
			{
				
				parentNode->setPosition(fromBullet(trans.getOrigin()));
				parentNode->setOrientation(fromBullet(trans.getRotation()));
			}

		}


		/*
		for (int i = 0; i < m_dynamicsWorld->getNumCollisionObjects(); i++)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			
			btTransform trans;

			if (body && body->getMotionState())
				body->getMotionState()->getWorldTransform(trans);
			else
				trans = obj->getWorldTransform();

			btVector3 pos = trans.getOrigin();
			btQuaternion rotate = trans.getRotation();


			printf("%d position: %f,%f,%f \n", i, trans.getOrigin().getX(),
				trans.getOrigin().getY(),
				trans.getOrigin().getZ());
			/// do some rendering with pos and rotate...?
		}*/
		

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
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