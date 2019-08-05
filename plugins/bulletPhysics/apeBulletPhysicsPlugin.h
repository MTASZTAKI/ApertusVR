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

#ifndef APE_BULLETPHYSICSPLUGIN_H
#define APE_BULLETPHYSICSPLUGIN_H

#include <iostream>
#include <list>
#include <string>
#include <thread>

#include "plugin/apeIPlugin.h"
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"

#include "sceneelements/apeIBoxGeometry.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeICylinderGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeIRayGeometry.h"
#include "sceneelements/apeISphereGeometry.h"
#include "sceneelements/apeITorusGeometry.h"
#include "sceneelements/apeITubeGeometry.h"
#include "sceneelements/apeIRigidBody.h"
#include "sceneElements/apeIIndexedFaceSetGeometry.h"
#include "sceneElements/apeIManualMaterial.h"

#include "utils/apeDoubleQueue.h"

/// inlcude bullet
#include "btBulletDynamicsCommon.h"

#define THIS_PLUGINNAME "apeBulletPhysicsPlugin"

namespace ape
{
	struct aabbProps
	{
		btVector3 aabbMin;
		btVector3 aabbMax;
		btTransform tr;
	};

	class BulletPhysicsPlugin : public ape::IPlugin
	{
	public:
		BulletPhysicsPlugin();

		~BulletPhysicsPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;


	private:
		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::IEventManager* mpEventManager;

		void eventCallBack(const ape::Event& event);

		/// for config

		float m_fixedTimeStep;

		int m_maxSubSteps;

		btVector3 m_gravity;

		float plainHeight;

		bool showBoundingBox;

		/// member pointers for bullet3
		btDefaultCollisionConfiguration* m_collisionConfiguration;

		btCollisionDispatcher* m_dispatcher;

		btBroadphaseInterface* m_overlappingPairCache;

		btSequentialImpulseConstraintSolver* m_solver;

		btDiscreteDynamicsWorld* m_dynamicsWorld;

		/// maps for shapes, objects, bodies, nodes...,
		/// the std::string key is the geometryName of the geometry

		std::map<std::string, btCollisionShape*> m_collisionShapes;

		std::map<std::string, btCollisionObject*> m_collisionObjects;
		
		std::map<std::string, ape::NodeWeakPtr> m_parentNodes;

		std::map<std::string, ape::Vector3> m_offsets;

		std::map<std::string, ape::BoxGeometryWeakPtr> m_boundingBoxes;

		std::map<std::string, btVector3> m_shapeScales;

		std::map<std::string, btVector3> m_shapeCenterOfMasses;
		
		ape::DoubleQueue<Event> m_eventDoubleQueue;

		void processEventDoubleQueue();

		/// conversion between bullet3 and ape

		ape::Vector3 fromBullet(const btVector3& btVec);

		ape::Quaternion fromBullet(const btQuaternion& btQuat);

		btVector3 fromApe(const ape::Vector3& apeVec);

		btQuaternion fromApe(const ape::Quaternion& apeQuat);

		std::string toString(btVector3 vec);

		/// functions for cleaner code in the eventCallBack

		void setTransform(std::string apeBodyName, btQuaternion new_orientation, btVector3 new_position);

		void deleteCollisionObject(std::string apeBodyName);

		void setCollisionShape(std::string apeBodyName, btCollisionShape* colShape, btScalar mass);

		void createRigidBody(std::string apeBodyName, btTransform trans, btScalar mass, btCollisionShape* shape);

		void createCollisionObject(std::string apeBodyName, btTransform trans, btCollisionShape * shape);

		btVector3 calculateCenterOfMass(const ape::GeometryCoordinates& coordinates);

		void updateShapeScale(std::string apeBodyname);

		void setBoundingBoxForDebug(std::string apeBodyName,btCollisionShape* colShape);

	};
	
	APE_PLUGIN_FUNC ape::IPlugin* CreateBulletPhysicsPlugin()
	{
		return new ape::BulletPhysicsPlugin;
	}

	APE_PLUGIN_FUNC void DestroyBulletPhysicsPlugin(ape::IPlugin *plugin)
	{
		delete (ape::BulletPhysicsPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		apeRegisterPlugin(THIS_PLUGINNAME, CreateBulletPhysicsPlugin, DestroyBulletPhysicsPlugin);
		return 0;
	}
}

#endif
