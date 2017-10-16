/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "btBulletWorldImporter.h"
#include "btHeightfieldTerrainShape.h"
#include "debugdraw.h"

namespace Dolphin
{
	class Physics
	{
	public:
		Physics();
		virtual ~Physics();
		void Initialize();
		void Clean();
		void StepSimulation(float deltatime);
		void CreateBasicRigidBody();
		void CreateDynamicRigidBody();
		void GenerateTerrainPhysics(int size, float* data,
			const Ogre::Real& minHeight,
			const Ogre::Real& maxHeight,
			const Ogre::Vector3& position,
			const Ogre::Real& scale);
		btBulletWorldImporter* GetPhysicsWorldImporter();
		CDebugDraw* mDebugDrawer;
	private:
		
		btBulletWorldImporter* physicsworldimporter;
		bParse::btBulletFile* bulletfileloader;

		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		//keep track of the shapes, we release memory at exit.
		//make sure to re-use collision shapes among rigid bodies whenever possible!
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
		btCollisionShape* groundShape;
		float* mTerrainHeights;
		btHeightfieldTerrainShape* mTerrainShape;
		btRigidBody* mGroundBody;
	};
}
#endif //PHYSICS_H
