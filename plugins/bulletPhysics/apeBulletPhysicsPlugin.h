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

// for bullet
#include "btBulletDynamicsCommon.h"

#define THIS_PLUGINNAME "apeBulletPhysicsPlugin"

namespace ape
{
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



		// physics 
		btDefaultCollisionConfiguration* collisionConfiguration;

		btCollisionDispatcher* dispatcher;

		btBroadphaseInterface* overlappingPairCache;

		btSequentialImpulseConstraintSolver* solver;

		btDiscreteDynamicsWorld* dynamicsWorld;

		btAlignedObjectArray<btCollisionShape*> collisionShapes;
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
