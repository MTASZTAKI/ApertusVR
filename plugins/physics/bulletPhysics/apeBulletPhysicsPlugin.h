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

#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"

#include "apeIBoxGeometry.h"
#include "apeIConeGeometry.h"
#include "apeICylinderGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIPlaneGeometry.h"
#include "apeIRayGeometry.h"
#include "apeISphereGeometry.h"
#include "apeITorusGeometry.h"
#include "apeITubeGeometry.h"
#include "apeICloneGeometry.h"
#include "apeIRigidBody.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeIManualMaterial.h"
#include "apeUserInputMacro.h"

//#define APE_DOUBLEQUEUE_UNIQUE
#include "apeDoubleQueue.h"

/// inlcude bullet
#include "btBulletDynamicsCommon.h"

#define THIS_PLUGINNAME "apeBulletPhysicsPlugin"

namespace ape
{

	struct userProps
	{
		std::string apeBodyName;
		btVector3 userPosition;
		ape::NodeWeakPtr userNode;
		bool userExists;
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

		ape::UserInputMacro* mpUserInputMacro;

		/// config variables

		float m_fixedTimeStep;

		int m_maxSubSteps;

		btScalar m_dtSec;

		btVector3 m_gravity;

		btVector3 m_waveDirection;

		btScalar m_waveFreq;

		btScalar m_waveDuration;

		btScalar m_forceScale;

		btScalar m_liquidHeight;

		btScalar m_liquidDensity;

		bool m_balanceInLiquid;

		bool mCollisionDetecionEnable;

		bool mCollisionDetecionSelf;

		/// member pointers for bullet

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

		std::map<std::string, btVector3> m_shapeScales;

		std::map<std::string, btVector3> m_shapeCenterOfMasses;

		std::map<std::string, bool> m_bouyancyEnabled;

		std::map<std::string, btScalar> m_volumes;

		std::map<std::string, ape::Vector2> m_dampings;

		std::map<std::string, std::string> m_geometryNameBodyNameMap;

		std::map<const btCollisionObject*, ape::RigidBodyWeakPtr> mApeRigidBodies;


		userProps m_userProps;

		ape::DoubleQueue<Event> m_eventDoubleQueue;

		/// conversion between bullet and ape

		ape::Vector3 fromBt(const btVector3& btVec);

		ape::Quaternion fromBt(const btQuaternion& btQuat);

		btVector3 fromApe(const ape::Vector3& apeVec);

		btQuaternion fromApe(const ape::Quaternion& apeQuat);

		btTransform fromApe(ape::NodeWeakPtr node);

		std::string toString(btVector3 vec);

		/// functions for cleaner code in the eventCallBack

		void processEventDoubleQueue();

		void setTransform(std::string apeBodyName, btQuaternion new_orientation, btVector3 new_position);

		void deleteCollisionObject(std::string apeBodyName);

		void setCollisionShape(std::string apeBodyName, btCollisionShape* colShape, btScalar mass);

		void createRigidBody(std::string apeBodyName, btTransform trans, btScalar mass, btCollisionShape* shape, ape::RigidBodyWeakPtr apeBody);

		void createCollisionObject(std::string apeBodyName, btTransform trans, btCollisionShape * shape);

		btVector3 calculateCenterOfMass(const ape::GeometryCoordinates& coordinates);

		void updateShapeScale(std::string apeBodyname);

		void updateBouyancy(std::string apeBodyName, btRigidBody* body, btTransform tr);

		ape::Vector3 Rotate(ape::Vector3 vec, ape::Quaternion quat);

		btVector3 Rotate(btVector3 vec, btQuaternion quat);

		void setOffsetVector(std::string apeBodyName, ape::Vector3 offsetVec);

		void createConvexHullShape(
			std::string apeBodyName,
			const ape::GeometryCoordinates& coordinates,
			btScalar mass
		);

		void createTriangleMeshShape(
			std::string apeBodyname,
			const ape::GeometryCoordinates& coordinates,
			const ape::GeometryIndices& indices,
			btScalar mass
		);

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
