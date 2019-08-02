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

	m_eventDoubleQueue = ape::DoubleQueue<Event>();

	plainHeight = 10.0f;
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
	m_eventDoubleQueue.push(event);
}

void ape::BulletPhysicsPlugin::processEventDoubleQueue()
{
	m_eventDoubleQueue.swap();

	while (!m_eventDoubleQueue.emptyPop())
	{
		ape::Event event = m_eventDoubleQueue.front();
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
					auto sphereShape = m_collisionShapes[apeBodyName] = new btSphereShape(10.f);

					btTransform trans;
					trans.setIdentity();
					trans.setOrigin(btVector3(0, 0, 0));
					btScalar mass = 1.0f;

					m_shapeScales[apeBodyName] = btVector3(1, 1, 1);

					createRigidBody(apeBodyName, trans, mass, sphereShape);

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
								btCollisionShape* planeShape = new btBoxShape(btVector3(planeSize.x * 0.5f, 30.0f, planeSize.y * 0.5f));
								btScalar mass = apeBody->getMass();
								m_offsets[apeBodyName] = ape::Vector3(0, 30, 0);
								setCollisionShape(apeBodyName, planeShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_CONE:
							if (auto cone = std::static_pointer_cast<IConeGeometry>(geometry))
							{
								
								btCollisionShape* coneShape = new btConeShape(cone->getParameters().radius,
																			 cone->getParameters().height);
								m_offsets[apeBodyName] = ape::Vector3(0,-cone->getParameters().height/2.0f,0);
								btScalar mass = apeBody->getMass();
								setCollisionShape(apeBodyName, coneShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_CYLINDER:
							if (auto cylinder = std::static_pointer_cast<ICylinderGeometry>(geometry))
							{
								btScalar height = cylinder->getParameters().height;
								btScalar radius = cylinder->getParameters().radius;
								btCollisionShape* cylinderShape = new btCylinderShape(btVector3(radius, height*0.5, radius));
								m_offsets[apeBodyName] = ape::Vector3(0, -height * 0.5, 0);
								btScalar mass = apeBody->getMass();
								setCollisionShape(apeBodyName, cylinderShape, mass);
							}
							break;
						case ape::Entity::Type::GEOMETRY_TORUS:
							if (auto torus = std::static_pointer_cast<ITorusGeometry>(geometry))
							{
								
							}
							break;
						case ape::Entity::Type::GEOMETRY_TUBE:
							if (auto tube = std::static_pointer_cast<ITubeGeometry>(geometry))
							{
								
							}
							break;
						case ape::Entity::Type::GEOMETRY_INDEXEDFACESET:
							if (auto faceSet = std::static_pointer_cast<IIndexedFaceSetGeometry>(geometry))
							{
								if (!(apeBody->isStatic()))
								{
									const ape::GeometryCoordinates& coordinates = faceSet->getParameters().getCoordinates();

									btVector3 centerOfMass = calculateCenterOfMass(coordinates);

									btConvexHullShape* convexHullShape = new btConvexHullShape();

									for (size_t i = 0; i < coordinates.size(); i = i + 3)
									{
										btVector3 coordinateVec(coordinates[i], coordinates[i + 1], coordinates[i + 2]);
										convexHullShape->addPoint(coordinateVec, false);
									}
									convexHullShape->recalcLocalAabb();

									btCompoundShape* compShape = new btCompoundShape();

									btTransform tr;
									tr.setIdentity();
									tr.setOrigin(-centerOfMass);

									compShape->addChildShape(tr, convexHullShape);

									m_offsets[apeBodyName] = fromBullet(-centerOfMass);

									setCollisionShape(apeBodyName, compShape, apeBody->getMass());
								}
								else
								{
									ape::GeometryCoordinates& coordinates = faceSet->getParameters().getCoordinates();
									ape::GeometryIndices& indices = faceSet->getParameters().getIndices();

									btVector3* shapeVertices = new btVector3[coordinates.size()/3];
									int* triangleIndices = new int[(indices.size()*3)/4];

									for (size_t i = 0; i < coordinates.size()/3; i = i++)
									{
										shapeVertices[i].setValue(
											coordinates[i * 3],
											coordinates[i * 3 + 1],
											coordinates[i * 3 + 2]
										);
									}

									int index = 0;
									for (size_t i = 0; i < indices.size(); i = i+4)
									{
										triangleIndices[index++] = indices[i];
										triangleIndices[index++] = indices[i+1];
										triangleIndices[index++] = indices[i+2];
									}


									

									btTriangleIndexVertexArray* indexVertexArrays = new btTriangleIndexVertexArray(
										index/3,
										triangleIndices,
										3*sizeof(int),
										coordinates.size(),
										(btScalar*)&shapeVertices[0].x(),
										sizeof(Vector3)
									);
									
									btCollisionShape* colShape = new btBvhTriangleMeshShape(indexVertexArrays, true);

									colShape->setLocalScaling(btVector3(2,2,2));

									btCompoundShape* compShape = new btCompoundShape();

									btVector3 centerOfMass = calculateCenterOfMass(coordinates);

									btTransform tr;
									tr.setIdentity();
									tr.setOrigin(-centerOfMass);

									compShape->addChildShape(tr, colShape);

									m_offsets[apeBodyName] = fromBullet(-centerOfMass);

									setCollisionShape(apeBodyName, compShape, apeBody->getMass());
								}
							}
							break;

						default:
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

					btBody->setDamping(apeBody->getLinearDamping(), apeBody->getAngularDamping());
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_RESTITUTION)
				{
					btCollisionObject* btObj = m_collisionObjects[apeBodyName];
					btRigidBody* btBody = btRigidBody::upcast(btObj);

					btBody->setRestitution(apeBody->getRestitution());
				}
				else if (event.type == ape::Event::Type::RIGIDBODY_PARENTNODE)
				{
					if (auto parentNode = mpSceneManager->getNode(parentNodeName).lock())
					{
						m_parentNodes[apeBodyName] = parentNode;

						m_shapeScales[apeBodyName] = fromApe(parentNode->getDerivedScale());
						updateShapeScale(apeBodyName);

						setTransform(apeBodyName,
							fromApe(parentNode->getDerivedOrientation()),
							fromApe(parentNode->getDerivedPosition()));

						while (auto parentParentNode = parentNode->getParentNode().lock())
							parentNode = parentParentNode;

						m_parentNodes[apeBodyName] = parentNode; //!
					}
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
		
				for (rapidjson::Value::MemberIterator it = input.MemberBegin(); it != input.MemberEnd(); it++)
				{
					if (it->name == "x")
						m_gravity.setX(it->value.GetFloat());
					else if (it->name == "y")
						m_gravity.setY(it->value.GetFloat());
					else if (it->name == "z")
						m_gravity.setZ(it->value.GetFloat());
				}
			}
		}
	}

	m_dynamicsWorld->setGravity(m_gravity);
	APE_LOG_FUNC_LEAVE();
}

clock_t t = clock();
void ape::BulletPhysicsPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	btClock btclock;
	while (true)
	{
		/// takes one step of the simulation in the physics engine
		
		processEventDoubleQueue();

		btScalar dtSec = btScalar(btclock.getTimeSeconds());
		

		///stepTime < maxNumSubSteps * internalTimeStep

		m_dynamicsWorld->stepSimulation(dtSec,100);
		btclock.reset();


		for (auto it = m_collisionObjects.begin(); it != m_collisionObjects.end(); it++)
		{
			std::string rbName = it->first;
			btCollisionObject* obj = it->second;
			btRigidBody* body = btRigidBody::upcast(obj);

			btTransform trans;

			if (body && body->getMotionState() && body->getInvMass() != 0.0f)
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
				trans = body->getWorldTransform();

			if (auto parentNode = m_parentNodes[rbName].lock())
			{
				ape::Quaternion orientation = fromBullet(trans.getRotation());

				btVector3 rotated_offset = fromApe(m_offsets[rbName]);

				ape::Vector3 position = fromBullet(trans.getOrigin() + (trans.getBasis() * rotated_offset));

				parentNode->setOrientation(orientation);
				parentNode->setPosition(position);

				if (float(clock()) - float(t) > 1000.0)
				{
					/*btVector3 parentTr = fromApe(parentNode->getParentNode().lock()->getPosition());
					btVector3 parentParentTr = fromApe(parentNode->getParentNode().lock()->getParentNode().lock()->getPosition());*/

					printf("%s pos: %s\n", rbName.c_str(), toString(trans.getOrigin()).c_str());
					/*printf("%s pos: %s\n", "parent ", toString(parentTr).c_str());
					printf("%s pos: %s\n", "parent parent ", toString(parentParentTr).c_str());*/
				}
			}
		}
		if (float(clock()) - float(t) > 1000.0)
			t = clock();

		//std::this_thread::sleep_for(std::chrono::milliseconds(10));

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


	if (auto body = btRigidBody::upcast(obj))
	{
		body->setCollisionShape(colShape);

		btVector3 localInertia;
		//btScalar mass = (body->getInvMass() == 0.0f) ? 0.0f : 1.0f / body->getInvMass();
		colShape->calculateLocalInertia(mass, localInertia);
		body->setMassProps(mass, localInertia);

		updateShapeScale(apeBodyName);

		m_dynamicsWorld->updateSingleAabb(body);
	}

	if (oldShape)
		delete oldShape;
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

	m_collisionObjects[apeBodyName] = body;

	m_dynamicsWorld->addRigidBody(body);
}


void ape::BulletPhysicsPlugin::createCollisionObject(std::string geometryName, btTransform trans, btCollisionShape* shape)
{
	btCollisionObject* obj = new btCollisionObject();
	obj->setWorldTransform(trans);
	obj->setCollisionShape(shape);

	m_collisionShapes[geometryName] = shape;
	m_collisionObjects[geometryName] = obj;

	m_dynamicsWorld->addCollisionObject(obj);
}

/// calculate center of mass for indexed facesets

btVector3 ape::BulletPhysicsPlugin::calculateCenterOfMass(const ape::GeometryCoordinates& coordinates)
{
	float M = coordinates.size() / 3;
	btVector3 com(0, 0, 0);
	for (size_t i = 0; i < coordinates.size(); i = i+3)
	{
		btVector3 v(coordinates[i], coordinates[i + 1], coordinates[i + 2]);
		com += v;
	}
	com = com / M;

	return com;
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
			
			btVector3 negCenterOfMass = compShape->getChildTransform(0).getOrigin();
			negCenterOfMass = btVector3(negCenterOfMass.getX() * scale.getX(),
				negCenterOfMass.getY() * scale.getY(),
				negCenterOfMass.getZ() * scale.getZ());
			
			compShape->setLocalScaling(scale);
			m_offsets[apeBodyName] = fromBullet(negCenterOfMass);
		}	
	}
	else
	{
		colShape->setLocalScaling(scale);
	}
}

// don't use this, it's not working
void ape::BulletPhysicsPlugin::setBoundingBoxForDebug(std::string apeBodyName, btCollisionShape* colShape)
{
	btVector3 aabbMin;
	btVector3 aabbMax;
	btTransform tr;

	tr.setIdentity();
	tr.setOrigin(btVector3(0, 0, 0));

	colShape->getAabb(tr, aabbMin, aabbMax);

	if (auto boundBox = m_boundingBoxes[apeBodyName].lock())
	{
		/*boundBox->setParameters(ape::Vector3(
			aabbMax.getX() - aabbMin.getX(),
			aabbMax.getY() - aabbMin.getY(),
			aabbMax.getZ() - aabbMin.getZ()));*/

		boundBox->setParameters(ape::Vector3(300, 300, 300));
	}
	else
	{
		if (auto boundBoxNode = mpSceneManager->createNode(apeBodyName + "boundBoxNode").lock())
		{
			if (auto bodyParentNode = m_parentNodes[apeBodyName].lock())
			{
				boundBoxNode->setParentNode(bodyParentNode);
			}


			// material for box
			std::shared_ptr<ape::IManualMaterial> boxMaterial;
			if (boxMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpSceneManager->createEntity(apeBodyName + "boxMaterial", ape::Entity::MATERIAL_MANUAL).lock()))
			{
				boxMaterial->setDiffuseColor(ape::Color(0.5f, 0.0f, 0.0f));
				boxMaterial->setSpecularColor(ape::Color(0.5f, 0.0f, 0.0f));
			}


			// oordinateSystemArrowXTubeNode->rotate(ape::Degree(-90.0f).toRadian(), ape::Vector3(0, 0, 1), ape::Node::TransformationSpace::WORLD);
			if (auto boundBox = std::static_pointer_cast<ape::IBoxGeometry>(mpSceneManager->createEntity(apeBodyName + "boundBox", ape::Entity::GEOMETRY_BOX).lock()))
			{
				/*boundBox->setParameters(ape::Vector3(
					aabbMax.getX() - aabbMin.getX(),
					aabbMax.getY() - aabbMin.getY(),
					aabbMax.getZ() - aabbMin.getZ()));*/
				boundBox->setParameters(ape::Vector3(300, 300, 300));
				boundBox->setParentNode(boundBoxNode);

				boundBox->setMaterial(boxMaterial);


				m_boundingBoxes[apeBodyName] = boundBox;
			}

		}
	}
	
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