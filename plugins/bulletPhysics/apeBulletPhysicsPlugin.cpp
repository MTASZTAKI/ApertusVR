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
	
	
	m_dynamicsWorld->setGravity(btVector3(0., -10., 0.));

	APE_LOG_FUNC_LEAVE();
}

ape::BulletPhysicsPlugin::~BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	/// diconnecting from events
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));

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
		if (event.type == ape::Event::Type::GEOMETRY_BOX_CREATE)
		{

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

				/// creating a sphere shape in the dynamics world
				btCollisionShape* colShape = new btSphereShape(btScalar(radius));
				m_collisionShapes[geometryName] = colShape;
				
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					/// ...
					btQuaternion rotate(parentNode->getOrientation().x,
										parentNode->getOrientation().y,
										parentNode->getOrientation().z,
										parentNode->getOrientation().w
										);

					btVector3 translate(parentNode->getPosition().x,
										parentNode->getPosition().y,
										parentNode->getPosition().z
										);

					btTransform trans = btTransform(rotate, translate);

					btScalar mass = 1.0f;

					bool isDynamic = (mass != 0.f);

					btVector3 localInertia(0, 0, 0);
					if (isDynamic)
						colShape->calculateLocalInertia(mass, localInertia);

					btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
					btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
					btRigidBody* body = new btRigidBody(rbInfo);
					
					m_collisionObjects[geometryName] = body;
					
					m_dynamicsWorld->addRigidBody(body);
					
				}
			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS)
			{
				/// creating new collision shape
				float radius = primitive->getParameters().radius;
				
				btCollisionShape* colShape = new btSphereShape(btScalar(radius));

				if (m_collisionShapes[geometryName])
					delete m_collisionShapes[geometryName];
				
				m_collisionShapes[geometryName] = colShape;

				m_collisionObjects[geometryName]->setCollisionShape(colShape);	

			}
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_DELETE)
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
			else if (event.type == ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE)
			{
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{

					btQuaternion rotate(parentNode->getOrientation().x,
						parentNode->getOrientation().y,
						parentNode->getOrientation().z,
						parentNode->getOrientation().w
					);
					btVector3 translate(parentNode->getPosition().x,
						parentNode->getPosition().y,
						parentNode->getPosition().z
					);

					btTransform trans = btTransform(rotate, translate);

					//btTransform trans(fromApe(parentNode->getOrientation()),
					//				  fromApe(parentNode->getPosition()));
					
					btDefaultMotionState* motionState = new btDefaultMotionState(trans);

					btRigidBody* body = btRigidBody::upcast(m_collisionObjects[geometryName]);
					if (body && body->getMotionState())
					{
						delete body->getMotionState();
						body->setMotionState(motionState);
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
					btTransform trans = btTransform(fromApe(parentNode->getOrientation()),
													fromApe(parentNode->getPosition()));

					/// plane is static, so it's mass should be 0
					btScalar mass = 0.0f; 

					bool isDynamic = (mass != 0.f);

					btVector3 localInertia(0, 0, 0);
					if (isDynamic)
						colShape->calculateLocalInertia(mass, localInertia);

					btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
					btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
					btRigidBody* body = new btRigidBody(rbInfo);

					m_collisionObjects[geometryName] = body;

					m_dynamicsWorld->addRigidBody(body);
				}
			}
			else if (event.type == ape::Event::GEOMETRY_PLANE_DELETE)
			{
				// TODO: make a function for deleting objects
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
			else if (event.type == ape::Event::GEOMETRY_PLANE_PARAMETERS)
			{
		
			}
			else if (event.type == ape::Event::GEOMETRY_BOX_PARENTNODE)
			{

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
	while (true)
	{
		/// takes one step of the simulation in the physics engine
		m_dynamicsWorld->stepSimulation(1.f / 60.f, 1);

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

			/// do some rendering with pos and rotate...?
		}


		std::this_thread::sleep_for (std::chrono::milliseconds(20)); 
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
