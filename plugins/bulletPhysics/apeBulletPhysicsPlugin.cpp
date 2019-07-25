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
	mpEventManager->connectEvent(ape::Event::Group::PHYSICS, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));

	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();

	/// init physics simulator (m_DynamicsWorld)
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
	mpEventManager->disconnectEvent(ape::Event::Group::PHYSICS, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));

	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::eventCallBack(const ape::Event& event)
{
	if (event.group == ape::Event::Group::PHYSICS)
	{
		if (auto apeBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			std::string apeBodyName = apeBody->getName();
			std::string parentNodeName = "";

			if (auto parentNode = apeBody->getParentNode().lock())
				parentNodeName = parentNode->getName();

			if (event.type == ape::Event::Type::RIGIDBODY_CREATE)
			{
				m_collisionShapes[apeBodyName] = nullptr;

				btTransform trans;
				trans.setIdentity();
				trans.setOrigin(btVector3(0, 0, 0));
				btScalar mass = 1.0f;

				createRigidBody(apeBodyName, trans, mass, nullptr);
			}
			else if (event.type == ape::Event::Type::RIGIDBODY_DELETE)
			{
				deleteCollisionObject(apeBodyName);
			}
			else if (event.type == ape::Event::Type::RIGIDBODY_MASS)
			{
				btScalar mass = apeBody->getMass();
				btRigidBody* btBody = m_rigidBodies[apeBodyName];

				if (mass > 0.0f && btBody->getCollisionShape())
				{
					btVector3 localInertia;
					btBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
					btBody->setMassProps(mass, localInertia);
				}
				else
				{
					btBody->setMassProps(0.0f, btVector3(0, 0, 0));
				}
			}
			else if (event.type == ape::Event::Type::RIGIDBODY_FRICTION)
			{
				btRigidBody* btBody = m_rigidBodies[apeBodyName];

				btBody->setFriction(apeBody->getLinearFriction());
				btBody->setRollingFriction(apeBody->getRollingFriction());
				btBody->setSpinningFriction(apeBody->getSpinningFriction());
			}
			else if (event.type == ape::Event::Type::RIGIDBODY_DAMPING)
			{
				btRigidBody* btBody = m_rigidBodies[apeBodyName];

				btBody->setDamping(apeBody->getLinearDamping(), apeBody->getAngularDamping());
			}
			else if (event.type == ape::Event::Type::RIGIDBODY_RESTITUTION)
			{
				btRigidBody* btBody = m_rigidBodies[apeBodyName];

				btBody->setRestitution(apeBody->getRestitution());
			}
			else if (event.type == ape::Event::Type::RIGIDBODY_SHAPE)
			{
				btRigidBody* btBody = m_rigidBodies[apeBodyName];
				if (auto geometry = apeBody->getGeometry().lock())
				{
					switch (geometry->getType())
					{
					case ape::Entity::Type::GEOMETRY_BOX:
						if (auto box = std::static_pointer_cast<IBoxGeometry>(geometry))
						{
							ape::Vector3 boxDims = box->getParameters().getDimensions();
							btCollisionShape* boxShape = new btBoxShape(fromApe(boxDims) * 0.5f);
							btScalar mass = apeBody->getMass();
							setCollisionShape(apeBodyName, boxShape, mass); 
						}
						break;
					case ape::Entity::Type::GEOMETRY_SPHERE:
						if (auto sphere = std::static_pointer_cast<ISphereGeometry>(geometry))
						{
							btScalar radius = sphere->getParameters().radius;
							btCollisionShape* sphereShape = new btSphereShape(radius);
							btScalar mass = apeBody->getMass();
							setCollisionShape(apeBodyName, sphereShape, mass);
						}
						break;
					case ape::Entity::Type::GEOMETRY_PLANE:
						if (auto plane = std::static_pointer_cast<IPlaneGeometry>(geometry))
						{
							ape::Vector2 planeSize = plane->getParameters().size;
							btCollisionShape* planeShape = new btBoxShape(btVector3(planeSize.x, 1.0f, planeSize.y));
							btScalar mass = apeBody->getMass();
							setCollisionShape(apeBodyName, planeShape, mass);
						}
						break;

					default:
						break;
					}
				}

			}
			else if (event.type == ape::Event::Type::RIGIDBODY_TYPE)
			{

			}
			else if (event.type == ape::Event::Type::RIGIDBODY_PARENTNODE)
			{
				if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
				{
					m_parentNodes[apeBodyName] = parentNode;

					ape::Entity::Type type;
					if (auto geometry = apeBody->getGeometry().lock())
						type = geometry->getType();

					if (type == ape::Entity::Type::GEOMETRY_PLANE)
						setTransform(apeBodyName,
							fromApe(parentNode->getOrientation()),
							fromApe(parentNode->getPosition()) - btVector3(0, -5.0, 0));
					else
						setTransform(apeBodyName,
							fromApe(parentNode->getOrientation()),
							fromApe(parentNode->getPosition()));
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
	clock_t t = clock();

	while (true)
	{
		/// takes one step of the simulation in the physics engine
		m_dynamicsWorld->stepSimulation(1.f / 60.f, 10);
		
		int i = 0;
		for (auto it = m_rigidBodies.begin(); it != m_rigidBodies.end(); it++)
		{
			std::string rbName = it->first;
			btRigidBody* body = it->second;
			
			btTransform trans;

			if (body && body->getMotionState() && !(body->isStaticObject()))
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
				trans = body->getWorldTransform();

			if (auto parentNode = m_parentNodes[rbName].lock())
			{
				btQuaternion rotation = trans.getRotation();

				parentNode->setPosition(fromBullet(trans.getOrigin()));
				parentNode->setOrientation(fromBullet(trans.getRotation()));
			}
			if(float(clock()) - float(t) > 1000)
				printf("%s, mass: %f\nposition: %f, %f,%f\n", rbName.c_str(), 1. / body->getInvMass()),
				trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ();
			
		}
		if (float(clock()) - float(t) > 1000)
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


/// sets the transform of a geometry's btCollisionObject in the physics world
void ape::BulletPhysicsPlugin::setTransform(std::string apeBodyName, btQuaternion new_orientation, btVector3 new_position)
{

	btTransform trans(new_orientation, new_position);

	if (auto body = m_rigidBodies[apeBodyName])
	{
		if (body->getMotionState())
		{
			//body->getMotionState()->setWorldTransform(trans);
			body->setCenterOfMassTransform(trans);
			//body->setWorldTransform(trans);
		}
		else
			body->setWorldTransform(trans);
		printf("%f, %f, %f", body->getWorldTransform().getOrigin().getX(),
			body->getWorldTransform().getOrigin().getY(),
			body->getWorldTransform().getOrigin().getZ());
	}
	
	/// else: error...?
}

/// delte object from dynamisWorld
void ape::BulletPhysicsPlugin::deleteCollisionObject(std::string apeBodyName)
{
	if (m_collisionShapes[apeBodyName])
	{
		delete m_collisionShapes[apeBodyName];
		m_collisionShapes[apeBodyName] = nullptr;
	}

	if (auto body = m_rigidBodies[apeBodyName])
	{
		if (body->getMotionState())
		{
			delete body->getMotionState();
		}

		m_dynamicsWorld->removeRigidBody(body);
		delete body;

		m_rigidBodies[apeBodyName] = nullptr;
	}
}

/// setting the shape in the physics world of a geometry
void ape::BulletPhysicsPlugin::setCollisionShape(std::string apeBodyName, btCollisionShape* colShape, btScalar mass)
{
	if (m_collisionShapes[apeBodyName])
		delete m_collisionShapes[apeBodyName];

	m_collisionShapes[apeBodyName] = colShape;

	if (auto body = m_rigidBodies[apeBodyName])
	{
		body->setCollisionShape(colShape);

		btVector3 localInertia;
		//btScalar mass = (body->getInvMass() == 0.0f) ? 0.0f : 1.0f / body->getInvMass();
		colShape->calculateLocalInertia(mass, localInertia);
		body->setMassProps(mass, localInertia);
	}
}

/// creates a rigidbody in the physics world
void ape::BulletPhysicsPlugin::createRigidBody(std::string apeBodyName, btTransform trans, btScalar mass, btCollisionShape* shape)
{

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.0f && shape)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	
	btRigidBody* body = new btRigidBody(rbInfo);

	m_rigidBodies[apeBodyName] = body;

	if(mass <= 0.0f) body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	
	m_dynamicsWorld->addRigidBody(body);
}


void ape::BulletPhysicsPlugin::createCollisionObject(std::string geometryName, btTransform trans, btCollisionShape* shape)
{
	btCollisionObject* obj = new btCollisionObject();
	obj->setWorldTransform(trans);
	obj->setCollisionShape(shape);

	//m_collisionShapes[geometryName] = shape;
	//m_collisionObjects[geometryName] = obj;
	
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