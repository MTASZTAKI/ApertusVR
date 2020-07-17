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
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));

	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();

	/// init physics simulator
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	/// default values for config variables
	m_userProps.userExists = false;
	m_fixedTimeStep = 0.16;
	m_maxSubSteps = 1;
	m_gravity = btVector3(0, -10, 0);
	m_waveDirection = btVector3(0, 0, 0);
	m_waveFreq = 5.0f;
	m_waveDuration = 1.0f;
	m_forceScale = 10.0f;
	m_liquidHeight = 300.0f;
	m_liquidDensity = 1.0f;
	m_balanceInLiquid = false;

	m_eventDoubleQueue = ape::DoubleQueue<Event>();

	mApeRigidBodies = std::map<const btCollisionObject*, ape::RigidBodyWeakPtr>();

	mCollisionDetecionEnable = false;

	mCollisionDetecionSelf = false;

	APE_LOG_FUNC_LEAVE();
}

ape::BulletPhysicsPlugin::~BulletPhysicsPlugin()
{
	APE_LOG_FUNC_ENTER();
	/// diconnecting from events
	mpEventManager->disconnectEvent(ape::Event::Group::PHYSICS, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&BulletPhysicsPlugin::eventCallBack, this, std::placeholders::_1));

	APE_LOG_FUNC_LEAVE();
}

void ape::BulletPhysicsPlugin::eventCallBack(const ape::Event& event)
{
	m_eventDoubleQueue.push(event);
}

void ape::BulletPhysicsPlugin::processEventDoubleQueue()
{
	m_eventDoubleQueue.swap();

	while (!m_eventDoubleQueue.emptyPop())
	{
		ape::Event event = m_eventDoubleQueue.front();
		if (event.group == ape::Event::Group::NODE)
		{
			if (auto node = mpSceneManager->getNode(event.subjectName).lock())
			{
				std::string nodeName = node->getName();
				if (event.type == ape::Event::Type::NODE_CREATE)
				{
					if (nodeName == mpUserInputMacro->getUserNode().lock()->getName())
					{
						m_userProps.userNode = node;
					}
				}
				else if (event.type == ape::Event::Type::NODE_PARENTNODE)
				{

				}
			}
		}
		else if (event.group == ape::Event::Group::PHYSICS)
		{
			if (auto apeBody = std::static_pointer_cast<ape::IRigidBody>(mpSceneManager->getEntity(event.subjectName).lock()))
			{
				std::string apeBodyName = apeBody->getName();
				std::string parentNodeName = "";

				if (auto parentNode = apeBody->getParentNode().lock())
					parentNodeName = parentNode->getName();


				if (event.type == ape::Event::Type::RIGIDBODY_CREATE)
				{
					auto sphereShape = m_collisionShapes[apeBodyName] = new btSphereShape(10.f);

					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, 0, 0));
					btScalar mass = 1.0f;
					m_shapeScales[apeBodyName] = btVector3(1, 1, 1);
					m_offsets[apeBodyName] = ape::Vector3(0, 0, 0);
					m_bouyancyEnabled[apeBodyName] = false;


					createRigidBody(apeBodyName, trans, mass, sphereShape, apeBody);
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_DELETE)
				{
					deleteCollisionObject(apeBodyName);
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_SHAPE)
				{

					btCollisionObject* btObj = m_collisionObjects[apeBodyName];
					btRigidBody* btBody = btRigidBody::upcast(btObj);

					if (auto geometry = apeBody->getGeometry().lock())
					{
						switch (geometry->getType())
						{
						case ape::Entity::Type::GEOMETRY_BOX:
							if (auto box = std::static_pointer_cast<ape::IBoxGeometry>(geometry))
							{
								ape::Vector3 boxDims = box->getParameters().getDimensions();
								btCollisionShape* boxShape = new btBoxShape(fromApe(boxDims) * 0.5f);
								btScalar mass = apeBody->getMass();
								m_volumes[apeBodyName] = boxDims.getX() * boxDims.getY() * boxDims.getZ();

								setCollisionShape(apeBodyName, boxShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_SPHERE:
							if (auto sphere = std::static_pointer_cast<ape::ISphereGeometry>(geometry))
							{
								btScalar radius = sphere->getParameters().radius;
								btCollisionShape* sphereShape = new btSphereShape(radius);
								btScalar mass = apeBody->getMass();
								m_volumes[apeBodyName] = radius * radius * radius * M_PI * 4.0f / 3.0f;

								setCollisionShape(apeBodyName, sphereShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_PLANE:
							if (auto plane = std::static_pointer_cast<ape::IPlaneGeometry>(geometry))
							{
								ape::Vector2 planeSize = plane->getParameters().size;
								btCollisionShape* planeShape = new btBoxShape(btVector3(planeSize.x * 0.5f, 0.0f, planeSize.y * 0.5f));
								btScalar mass = apeBody->getMass();
								setCollisionShape(apeBodyName, planeShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_CONE:
							if (auto cone = std::static_pointer_cast<ape::IConeGeometry>(geometry))
							{
								btScalar height = cone->getParameters().height;
								btScalar radius = cone->getParameters().radius;
								btCollisionShape* coneShape = new btConeShape(radius, height);
								setOffsetVector(apeBodyName, ape::Vector3(0, -cone->getParameters().height / 2.0f, 0));
								btScalar mass = apeBody->getMass();
								m_volumes[apeBodyName] = radius * radius * M_PI * height / 3.0f;

								setCollisionShape(apeBodyName, coneShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_CYLINDER:
							if (auto cylinder = std::static_pointer_cast<ape::ICylinderGeometry>(geometry))
							{
								btScalar height = cylinder->getParameters().height;
								btScalar radius = cylinder->getParameters().radius;
								btCollisionShape* cylinderShape = new btCylinderShape(btVector3(radius, height*0.5, radius));
								setOffsetVector(apeBodyName, ape::Vector3(0, -height * 0.5, 0));
								btScalar mass = apeBody->getMass();
								m_volumes[apeBodyName] = radius * radius * M_PI * height;

								setCollisionShape(apeBodyName, cylinderShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_TORUS:
							if (auto torus = std::static_pointer_cast<ape::ITorusGeometry>(geometry))
							{

							}
							break;
						case ape::Entity::Type::GEOMETRY_TUBE:
							if (auto tube = std::static_pointer_cast<ape::ITubeGeometry>(geometry))
							{

							}
							break;
						case ape::Entity::Type::GEOMETRY_INDEXEDFACESET:
							if (auto faceSet = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(geometry))
							{
								if (!(apeBody->isStatic()))
								{
									if (apeBody->getColliderType() == ape::RigidBodyColliderType::AUTO ||
										apeBody->getColliderType() == ape::RigidBodyColliderType::CONVEX_HULL)
									{
										const ape::GeometryCoordinates& coordinates = faceSet->getParameters().getCoordinates();
										m_geometryNameBodyNameMap[apeBody->getGeometryName()] = apeBodyName;
										createConvexHullShape(
											apeBodyName,
											coordinates,
											apeBody->getMass()
										);
									}
								}
								else
								{
									if (apeBody->getColliderType() == ape::RigidBodyColliderType::AUTO ||
										apeBody->getColliderType() == ape::RigidBodyColliderType::TRIANGLE_MESH)
									{
										const ape::GeometryCoordinates& coordinates = faceSet->getParameters().getCoordinates();
										const ape::GeometryIndices& indices = faceSet->getParameters().getIndices();
										m_geometryNameBodyNameMap[apeBody->getGeometryName()] = apeBodyName;
										createTriangleMeshShape(
											apeBodyName,
											coordinates,
											indices,
											apeBody->getMass()
										);
									}
									else if (apeBody->getColliderType() == ape::RigidBodyColliderType::CONVEX_HULL)
									{
										const ape::GeometryCoordinates& coordinates = faceSet->getParameters().getCoordinates();
										m_geometryNameBodyNameMap[apeBody->getGeometryName()] = apeBodyName;
										createConvexHullShape(
											apeBodyName,
											coordinates,
											apeBody->getMass()
										);
									}
								}
							}
							break;
						case ape::Entity::Type::GEOMETRY_CLONE:
							if (auto cloneGeometry = std::static_pointer_cast<ape::ICloneGeometry>(geometry))
							{

								std::string sourceGeometryName = cloneGeometry->getSourceGeometryName();
								std::string apeBodyCopyName = m_geometryNameBodyNameMap[sourceGeometryName];
								if (m_collisionShapes[apeBodyCopyName])
								{
									btCollisionShape* shape = m_collisionShapes[apeBodyCopyName];
									btScalar mass = apeBody->getMass();

									m_shapeScales[apeBodyName] = m_shapeScales[apeBodyCopyName];
									setOffsetVector(apeBodyName, m_offsets[apeBodyCopyName]);
									m_volumes[apeBodyName] = m_volumes[apeBodyCopyName];

									setCollisionShape(apeBodyName, shape, mass);
								}
							}
							break;
						}
					}
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_MASS)
				{
					btScalar mass = apeBody->getMass();
					btCollisionObject* btObj = m_collisionObjects[apeBodyName];
					btRigidBody* btBody = btRigidBody::upcast(btObj);

					if (btBody && btBody->getCollisionShape())
					{
						btVector3 localInertia;
						btBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
						btBody->setMassProps(mass, localInertia);
					}

					if (apeBody->isKinematic())
					{
						/*btBody->setCollisionFlags(btBody->getCollisionFlags() |
							btCollisionObject::CF_KINEMATIC_OBJECT);*/
							/*btBody->setActivationState(DISABLE_DEACTIVATION);*/
					}

				}
				else if (event.type == ape::Event::Type::RIGIDBODY_FRICTION)
				{
					btCollisionObject* btObj = m_collisionObjects[apeBodyName];
					btRigidBody* btBody = btRigidBody::upcast(btObj);

					btBody->setFriction(apeBody->getLinearFriction());
					btBody->setRollingFriction(apeBody->getRollingFriction());
					btBody->setSpinningFriction(apeBody->getSpinningFriction());
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_DAMPING)
				{
					btCollisionObject* btObj = m_collisionObjects[apeBodyName];
					btRigidBody* btBody = btRigidBody::upcast(btObj);
					m_dampings[apeBodyName] = ape::Vector2(apeBody->getLinearDamping(), apeBody->getAngularDamping());


					btBody->setDamping(apeBody->getLinearDamping(), apeBody->getAngularDamping());
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_RESTITUTION)
				{
					btCollisionObject* btObj = m_collisionObjects[apeBodyName];
					btRigidBody* btBody = btRigidBody::upcast(btObj);

					btBody->setRestitution(apeBody->getRestitution());
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_BOUYANCY)
				{
					if (m_bouyancyEnabled[apeBodyName] = apeBody->bouyancyEnabled())
					{

					}
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_PARENTNODE)
				{
					if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
					{
						m_parentNodes[apeBodyName] = parentNode;

						auto offset = -m_offsets[apeBodyName];
						parentNode->translate(-m_offsets[apeBodyName],ape::Node::TransformationSpace::LOCAL);

						m_shapeScales[apeBodyName] = fromApe(parentNode->getDerivedScale());
						updateShapeScale(apeBodyName);

						setTransform(apeBodyName,
							fromApe(parentNode->getDerivedOrientation()),
							fromApe(parentNode->getDerivedPosition()));

						/// user 
						if (parentNode->getName() == "userBodyNode")
						{
							m_userProps.apeBodyName = apeBodyName;
							m_userProps.userPosition = fromApe(parentNode->getDerivedPosition());
							m_userProps.userExists = bool(m_userProps.userNode.lock());
						}
					}
				}
			}
		}
		else if (event.group == ape::Event::Group::GEOMETRY_FILE)
		{
			if (event.type == ape::Event::Type::GEOMETRY_FILE_FILENAME)
			{
				if (auto geometryFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
				{
					geometryFile->getFileName();
				}
			}
		}
		m_eventDoubleQueue.pop();
	}
}

void ape::BulletPhysicsPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeBulletPlugin.json";

	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();

	FILE* apeBulletConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeBulletConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeBulletConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			if (jsonDocument.HasMember("maxSubSteps"))
			{
				rapidjson::Value& input = jsonDocument["maxSubSteps"];
				m_maxSubSteps = input.GetInt();
			}
			if (jsonDocument.HasMember("fixedTimeStep"))
			{
				rapidjson::Value& input = jsonDocument["fixedTimeStep"];
				m_fixedTimeStep = input.GetFloat();
			}
			if (jsonDocument.HasMember("gravity"))
			{
				rapidjson::Value& input = jsonDocument["gravity"];

				m_gravity.setX(input.GetArray()[0].GetFloat());
				m_gravity.setY(input.GetArray()[1].GetFloat());
				m_gravity.setZ(input.GetArray()[2].GetFloat());

				/*for (rapidjson::Value::MemberIterator it = input.MemberBegin(); it != input.MemberEnd(); it++)
				{
					if (it->name == "x")
						m_gravity.setX(it->value.GetFloat());
					else if (it->name == "y")
						m_gravity.setY(it->value.GetFloat());
					else if (it->name == "z")
						m_gravity.setZ(it->value.GetFloat());
				}*/
			}
			if (jsonDocument.HasMember("collisionDetecion"))
			{
				rapidjson::Value& collisionDetecion = jsonDocument["collisionDetecion"];
				for (rapidjson::Value::MemberIterator it = collisionDetecion.MemberBegin(); it != collisionDetecion.MemberEnd(); it++)
				{
					if (it->name == "enable")
					{
						mCollisionDetecionEnable = it->value.GetBool();
					}
					else if (it->name == "frselfeq")
					{
						mCollisionDetecionSelf = it->value.GetBool();
					}
				}
			}
			if (jsonDocument.HasMember("waterWave"))
			{
				rapidjson::Value& input = jsonDocument["waterWave"];

				for (rapidjson::Value::MemberIterator it = input.MemberBegin(); it != input.MemberEnd(); it++)
				{

					if (it->name == "direction")
					{
						m_waveDirection.setX(it->value.GetArray()[0].GetFloat());
						m_waveDirection.setY(it->value.GetArray()[1].GetFloat());
						m_waveDirection.setZ(it->value.GetArray()[2].GetFloat());
					}
					else if(it->name == "freq")
						m_waveFreq = it->value.GetFloat();
					else if (it->name == "duration")
						m_waveDuration = it->value.GetFloat();
				}
			}
			if (jsonDocument.HasMember("forceScale"))
			{
				rapidjson::Value& input = jsonDocument["forceScale"];
				m_forceScale = input.GetFloat();
			}
			if (jsonDocument.HasMember("bouyancy"))
			{
				rapidjson::Value& input = jsonDocument["bouyancy"];

				for (rapidjson::Value::MemberIterator it = input.MemberBegin(); it != input.MemberEnd(); it++)
				{
					if (it->name == "liquidHeight")
						m_liquidHeight = it->value.GetFloat();
					else if (it->name == "liquidDensity")
						m_liquidDensity = it->value.GetFloat();
					else if (it->name == "balance")
						m_balanceInLiquid = it->value.GetBool();
				}
			}
		}
	}

	/// setting gravity with the forceScale parameter
	m_dynamicsWorld->setGravity(m_gravity * m_forceScale);

	APE_LOG_FUNC_LEAVE();
}

clock_t t = clock();
void ape::BulletPhysicsPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	btClock btclock;
	while (true)
	{
		processEventDoubleQueue();

		m_dtSec = btScalar(btclock.getTimeSeconds());

		/// takes one step of the simulation in the physics engine
		///stepTime < maxNumSubSteps * internalTimeStep
		m_dynamicsWorld->stepSimulation(m_dtSec, m_maxSubSteps, m_fixedTimeStep);
		btclock.reset();

		if (mCollisionDetecionEnable)
		{
			int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
			for (int i = 0; i < numManifolds; i++)
			{
				btPersistentManifold* contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
				const btCollisionObject* obA = contactManifold->getBody0();
				const btCollisionObject* obB = contactManifold->getBody1();
				int numContacts = contactManifold->getNumContacts();
				for (int j = 0; j < numContacts; j++)
				{
					btManifoldPoint& pt = contactManifold->getContactPoint(j);
					if (pt.getDistance() < 0.f)
					{
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;
						if (auto apeRigidBodyA = mApeRigidBodies[obA].lock())
						{
							if (auto apeRigidBodyB = mApeRigidBodies[obB].lock())
							{
								if (mCollisionDetecionSelf)
								{
									apeRigidBodyA->setCollision(apeRigidBodyB->getName());
									apeRigidBodyB->setCollision(apeRigidBodyA->getName());
								}
								else
								{
									ape::NodeSharedPtr rootNodeA = ape::NodeSharedPtr();
									if (auto node = apeRigidBodyA->getParentNode().lock())
									{
										while (auto parentNode = node->getParentNode().lock())
										{
											node = parentNode;
										}
										rootNodeA = node;
									}
									ape::NodeSharedPtr rootNodeB = ape::NodeSharedPtr();
									if (auto node = apeRigidBodyB->getParentNode().lock())
									{
										while (auto parentNode = node->getParentNode().lock())
										{
											node = parentNode;
										}
										rootNodeB = node;
									}
									if (rootNodeA.use_count() && rootNodeB.use_count() && rootNodeA != rootNodeB)
									{
										if (apeRigidBodyA->getOwner() != mpCoreConfig->getNetworkGUID())
										{
											apeRigidBodyA->setOwner(mpCoreConfig->getNetworkGUID());
										}
										if (apeRigidBodyB->getOwner() != mpCoreConfig->getNetworkGUID())
										{
											apeRigidBodyB->setOwner(mpCoreConfig->getNetworkGUID());
										}
										apeRigidBodyA->setCollision(apeRigidBodyB->getName());
										apeRigidBodyB->setCollision(apeRigidBodyA->getName());
										APE_LOG_DEBUG("Collision: " << apeRigidBodyA->getName() << " ; " << apeRigidBodyB->getName());
									}
								}
							}
						}
					}
				}
			}
		}

		for (auto it = m_collisionObjects.rbegin(); it != m_collisionObjects.rend(); it++)
		{
			std::string apeBodyName = it->first;
			btCollisionObject* obj = it->second;
			btRigidBody* body = btRigidBody::upcast(obj);

			btTransform trans;

			if (body && body->getMotionState() && body->getInvMass() != 0.0f)
			{
				body->getMotionState()->getWorldTransform(trans);

				if (m_bouyancyEnabled[apeBodyName])
					updateBouyancy(apeBodyName, body, trans);

			}
			else
				trans = body->getWorldTransform();

			if (auto parentNode = m_parentNodes[apeBodyName].lock())
			{
				ape::Quaternion worldOrientation = fromBt(trans.getRotation());

				btVector3 rotated_offset = fromApe(m_offsets[apeBodyName]);

				ape::Vector3 worldPosition = fromBt(trans.getOrigin() + trans.getBasis() * rotated_offset);

				if (body && !body->isStaticObject() && !body->isKinematicObject())
				{
					ape::Vector3 inheritedPosition{ 0, 0, 0 };
					ape::Quaternion inheritedOrientation{ 1, 0, 0, 0 };
					ape::Vector3 inheritedScale{ 1, 1, 1 };
					ape::NodeSharedPtr nodeIt = parentNode;


					while (auto parentParentNode = nodeIt->getParentNode().lock())
					{
						nodeIt = parentParentNode;
						inheritedPosition += Rotate(nodeIt->getPosition() / inheritedScale, inheritedOrientation);
						inheritedOrientation = inheritedOrientation * nodeIt->getOrientation();
						inheritedScale = inheritedScale * nodeIt->getScale();
					}

					ape::Vector3 position = Rotate((worldPosition - inheritedPosition) / inheritedScale, inheritedOrientation);
					ape::Quaternion orientation = inheritedOrientation.Inverse() * worldOrientation;

					parentNode->setOrientation(orientation);
					parentNode->setPosition(position);
				}
				else
				{
					/*ape::Quaternion orientation = parentNode->getOrientation() * parentNode->getDerivedOrientation().Inverse() * worldOrientation;

					ape::Vector3 scale = parentNode->getScale();
					ape::Vector3 pos = parentNode->getPosition();
					ape::Vector3 position = parentNode->getPosition()-parentNode->getDerivedPosition() + worldPosition;

					parentNode->setOrientation(worldOrientation);
					parentNode->setPosition(worldPosition);*/

					btVector3 newPosition = fromApe(parentNode->getDerivedPosition());
					btQuaternion newOrientation = fromApe(parentNode->getDerivedOrientation());

					btTransform newTrans = btTransform(newOrientation, newPosition);

					body->setWorldTransform(newTrans);
				}
			}

			/// debug
			if (float(clock()) - float(t) > 1000.0)
			{
				//printf("%s pos: %s\n", apeBodyName.c_str(), toString(trans.getOrigin()).c_str());
				//printf("%s mass: %f\n", apeBodyName.c_str(), body->getInvMass());
				//btVector3 force = body->getTotalForce();
				//printf("%s total force: %s\n", apeBodyName.c_str(),toString(force).c_str());
				//printf("%s gravity: %s\n", apeBodyName.c_str(), toString(body->getGravity()).c_str());
			}

			/// USER
			if (m_userProps.userExists && m_userProps.apeBodyName == apeBodyName)
			{
				btVector3 translateVec;
				static bool b = false;
				if (auto userNode = m_userProps.userNode.lock())
				{
					btVector3 newUserPosition = fromApe(userNode->getPosition());
					translateVec = newUserPosition - m_userProps.userPosition;
					m_userProps.userPosition = newUserPosition;

					body->translate(translateVec);

					trans = body->getWorldTransform();
					trans.setRotation(fromApe(userNode->getOrientation()));
					body->setWorldTransform(trans);
				}
			}
		}
		if (float(clock()) - float(t) > 1000.0)
			t = clock();

		/*std::this_thread::sleep_for(std::chrono::milliseconds(20));*/

	}
	APE_LOG_FUNC_LEAVE();
}


/// conversion between bullet3 and ape
ape::Vector3 ape::BulletPhysicsPlugin::fromBt(const btVector3& btVec)
{
	return ape::Vector3(btVec.getX(), btVec.getY(), btVec.getZ());
}

ape::Quaternion ape::BulletPhysicsPlugin::fromBt(const btQuaternion& btQuat)
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

btTransform ape::BulletPhysicsPlugin::fromApe(ape::NodeWeakPtr nodeWeak)
{
	btTransform trans;
	if (auto nodeShared = nodeWeak.lock())
	{
		trans.setOrigin(fromApe(nodeShared->getPosition()));
		trans.setRotation(fromApe(nodeShared->getOrientation()));
	}
	return trans;
}

// for debug
std::string ape::BulletPhysicsPlugin::toString(btVector3 vec)
{
	std::ostringstream ss;
	ss << vec.getX() << "," << vec.getY() << "," << vec.getZ();
	return ss.str();
}


/// sets the transform of a geometry's btCollisionObject in the physics world
void ape::BulletPhysicsPlugin::setTransform(std::string apeBodyName, btQuaternion new_orientation, btVector3 new_position)
{

	btTransform trans(new_orientation, new_position);

	auto obj = m_collisionObjects[apeBodyName];

	if (auto body = btRigidBody::upcast(obj))
	{
		if (body->getMotionState())
		{
			//body->getMotionState()->setWorldTransform(trans);
			body->setCenterOfMassTransform(trans);
			//body->setWorldTransform(trans);
		}
		else
			body->setWorldTransform(trans);

		//body->translate(fromApe(-m_offsets[apeBodyName]));
	}

	//printf("%s start pos %s\n", apeBodyName.c_str(),toString(trans.getOrigin()).c_str());

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

	if (auto obj = m_collisionObjects[apeBodyName])
	{
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;

		m_collisionObjects[apeBodyName] = nullptr;
	}
}

/// setting the shape in the physics world of a geometry
void ape::BulletPhysicsPlugin::setCollisionShape(std::string apeBodyName, btCollisionShape* colShape, btScalar mass)
{

	btCollisionShape* oldShape = m_collisionShapes[apeBodyName];

	m_collisionShapes[apeBodyName] = colShape;

	btCollisionObject* obj = m_collisionObjects[apeBodyName];

	/*if (oldShape)
		colShape->setLocalScaling(oldShape->getLocalScaling());*/

	if (auto body = btRigidBody::upcast(obj))
	{
		body->setCollisionShape(colShape);
		updateShapeScale(apeBodyName);

		btVector3 localInertia;
		if (body->getInvMass() > 0.0f)
			colShape->calculateLocalInertia(mass, localInertia);
		body->setMassProps(mass, localInertia);
	}


	if (oldShape)
		delete oldShape;
}

/// creates a rigidbody in the physics world
void ape::BulletPhysicsPlugin::createRigidBody(std::string apeBodyName, btTransform trans, btScalar mass, btCollisionShape* shape, ape::RigidBodyWeakPtr apeBody)
{

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.0f && shape)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);

	m_collisionObjects[apeBodyName] = body;

	mApeRigidBodies[body] = apeBody;

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

btVector3 ape::BulletPhysicsPlugin::calculateCenterOfMass(const ape::GeometryCoordinates& coordinates)
{
	float Mass = coordinates.size() / 3;
	btVector3 centerOfMass(0, 0, 0);
	for (size_t i = 0; i < coordinates.size(); i = i + 3)
	{
		btVector3 vertex(coordinates[i], coordinates[i + 1], coordinates[i + 2]);
		centerOfMass += vertex;
	}
	centerOfMass = centerOfMass / Mass;

	return centerOfMass;
}

void ape::BulletPhysicsPlugin::updateShapeScale(std::string apeBodyName)
{
	btCollisionShape* colShape = m_collisionShapes[apeBodyName];
	btVector3 scale = m_shapeScales[apeBodyName];

	if (colShape->isCompound())
	{
		btCompoundShape* compShape = static_cast<btCompoundShape*>(colShape);
		if (compShape->getNumChildShapes() > 0)
		{
			btCollisionShape* childShape = compShape->getChildShape(0);
			btVector3 scaledCenterOfMass = m_shapeCenterOfMasses[apeBodyName] * scale;
			compShape->setLocalScaling(scale);
			setOffsetVector(apeBodyName, fromBt(-scaledCenterOfMass));
			//m_offsets[apeBodyName] = fromBt(-scaledCenterOfMass);
		}
	}
	else
	{
		colShape->setLocalScaling(scale);
	}
}

/// Function for bouyancy force generating by hand
void ape::BulletPhysicsPlugin::updateBouyancy(std::string apeBodyName, btRigidBody* body, btTransform tr)
{
	btVector3 aabbMin, aabbMax;
	body->getAabb(aabbMin, aabbMax);
	btScalar maxDepth = (aabbMax.getY() - aabbMin.getY()) / 2.0f;
	btScalar depth = tr.getOrigin().getY();

	btScalar volumeScale = m_shapeScales[apeBodyName].getX() * m_shapeScales[apeBodyName].getY() * m_shapeScales[apeBodyName].getZ();
	btScalar volume = m_volumes[apeBodyName] * volumeScale;
	/// pressing the volume (a rude solution to deal with big objects)
	//volume = (2 * atan(volume) / M_PI * 800 + 800) / 50;
	volume = log10(volume) * 5;

	btVector3 bouyancyForce;
	btVector3 forcePos(0, 0, 0);
	btVector3 surfaceTension(0, 0, 0);


	static btClock clock;
	btScalar dTime = btScalar(clock.getTimeSeconds());

	/// out of water
	if (depth >= m_liquidHeight + maxDepth)
	{
		bouyancyForce = btVector3(0, 0, 0);

		/// This is a fully arbitrary solution for contacting with the water surface
		if (abs(depth - m_liquidHeight - maxDepth) < 0.1)
		{
			btVector3 vel = body->getLinearVelocity();

			if (vel.getY() < 0)
				vel.setY(vel.getY() * 0.5f);
			else
				vel.setY(vel.getY() * 0.3f);

			body->setLinearVelocity(vel);
		}
		body->setDamping(m_dampings[apeBodyName].x, m_dampings[apeBodyName].y);
	}
	/// fully submerged
	else if (depth <= m_liquidHeight - maxDepth)
	{
		bouyancyForce = btVector3(0, m_liquidDensity*volume, 0);
		body->setDamping(0.7, 0.7);
	}
	/// partly submerged
	else
	{
		/// hack for balancing
		if (m_balanceInLiquid)
			forcePos = Rotate(btVector3(0, depth - aabbMin.getY() /*-abs(m_liquidHeight - depth)*/, 0), tr.getRotation()) / 80;

		float submerged = m_liquidHeight - depth + maxDepth;
		bouyancyForce = btVector3(0, submerged / (2 * maxDepth) *m_liquidDensity*volume, 0);
		body->setDamping(0.7, 0.7);
		/// waves
		if (dTime > m_waveFreq && dTime <= m_waveFreq + m_waveDuration)
		{
			body->applyForce(m_waveDirection * m_forceScale,forcePos);
		}
		else if (dTime > m_waveFreq + m_waveDuration)
			clock.reset();
	}

	body->applyForce(bouyancyForce * m_forceScale, forcePos);
}

ape::Vector3 ape::BulletPhysicsPlugin::Rotate(ape::Vector3 vec, ape::Quaternion quat)
{
	ape::Quaternion vecAsQuat(
		0,
		vec.x,
		vec.y,
		vec.z
	);

	ape::Quaternion resultAsQuat = quat * vecAsQuat * quat.Inverse();

	return ape::Vector3(resultAsQuat.x, resultAsQuat.y, resultAsQuat.z);
}

btVector3 ape::BulletPhysicsPlugin::Rotate(btVector3 vec, btQuaternion quat)
{
	btQuaternion vecAsQuat(
		0,
		vec.getX(),
		vec.getY(),
		vec.getZ()
	);

	btQuaternion resultAsQuat = quat * vecAsQuat * quat.inverse();

	return btVector3(resultAsQuat.getX(), resultAsQuat.getY(), resultAsQuat.getZ());
}

void ape::BulletPhysicsPlugin::setOffsetVector(std::string apeBodyName, ape::Vector3 offsetVec)
{
	if (auto parentNode = m_parentNodes[apeBodyName].lock())
	{
		auto pNodePos0 = parentNode->getDerivedPosition();
		/*parentNode->translate(-m_offsets[apeBodyName], ape::Node::TransformationSpace::LOCAL);
		parentNode->translate(offsetVec, ape::Node::TransformationSpace::LOCAL);*/
		auto pNodePos1 = parentNode->getPosition();
	}
	m_offsets[apeBodyName] = offsetVec;
}

void ape::BulletPhysicsPlugin::createConvexHullShape(
	std::string apeBodyName,
	const ape::GeometryCoordinates & coordinates,
	btScalar mass)
{
	/// creating convex hull shape
	btConvexHullShape* convexHullShape = new btConvexHullShape();

	for (size_t i = 0; i < coordinates.size(); i = i + 3)
	{
		btVector3 v(coordinates[i], coordinates[i + 1], coordinates[i + 2]);
		convexHullShape->addPoint(v, false);
	}
	convexHullShape->recalcLocalAabb();

	/// setting the volume for bouyancy generator
	btVector3 bSphereCenter;
	btScalar bSphereRadius;
	convexHullShape->getBoundingSphere(bSphereCenter, bSphereRadius);
	m_volumes[apeBodyName] = bSphereRadius * bSphereRadius * bSphereRadius * M_PI * 4.0f / 3.0f;

	if (mass > 0.0f)
	{
		/// positioning it to the center of mass
		btCompoundShape* compShape = new btCompoundShape();
		btVector3 centerOfMass = calculateCenterOfMass(coordinates);
		m_shapeCenterOfMasses[apeBodyName] = centerOfMass;

		btTransform tr;
		tr.setIdentity();
		tr.setOrigin(-centerOfMass);

		compShape->addChildShape(tr, convexHullShape);
		setOffsetVector(apeBodyName, fromBt(-centerOfMass));
		//m_offsets[apeBodyName] = fromBullet(-centerOfMass);

		setCollisionShape(apeBodyName, compShape, mass);
	}
	else
	{
		setCollisionShape(apeBodyName, convexHullShape, mass);
	}
}

void ape::BulletPhysicsPlugin::createTriangleMeshShape(
	std::string apeBodyName,
	const ape::GeometryCoordinates & coordinates,
	const ape::GeometryIndices & indices,
	btScalar mass)
{
	int numVertices = coordinates.size() / 3;
	int numTriangles = indices.size() / 4;
	int vertStride = sizeof(btVector3);
	int indexStride = 3 * sizeof(int);

	btVector3* shapeVertices = new btVector3[numVertices];
	int* triangleIndices = new int[numTriangles * 3];

	for (int i = 0; i < numVertices; i = i++)
	{
		shapeVertices[i].setValue(
			coordinates[i * 3],
			coordinates[i * 3 + 1],
			coordinates[i * 3 + 2]
		);
	}

	int index = 0;
	for (int i = 0; i < indices.size(); i = i + 4)
	{
		triangleIndices[index++] = indices[i];
		triangleIndices[index++] = indices[i + 1];
		triangleIndices[index++] = indices[i + 2];
	}

	/// numTriangles, triangleIndexBase, triangleIndexStride, numVertices, vertexBase, vertexStride
	btTriangleIndexVertexArray* indexVertexArrays = new btTriangleIndexVertexArray(
		numTriangles,
		triangleIndices,
		indexStride,
		numVertices,
		(btScalar*)&shapeVertices[0].x(),
		vertStride
	);

	btCollisionShape* colShape = new btBvhTriangleMeshShape(indexVertexArrays, true);

	colShape->setMargin(0.5f);

	setCollisionShape(apeBodyName, colShape, mass);
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