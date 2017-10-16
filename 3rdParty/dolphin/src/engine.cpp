/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "engine.h"
#include <functional>
#include "OgreWindowEventUtilities.h"
#include "OgreTimer.h"

#include "render.h"
#include "huddev.h"
#include "renderogre.h"
#include "scenecreator.h"
#include "scenecomponentbuilderogre.h"
#include "scenecomponents.h"
#include "physics.h"
#include "eventmanager.h"
#include "event.h"

#include "ocean/ocean.h"
#include "sky/sky.h"
#include "terrain.h"

std::vector<Dolphin::SceneComponent*> Dolphin::Engine::scenecomponents;
Dolphin::Ocean* Dolphin::Engine::ocean = 0;
Dolphin::Sky* Dolphin::Engine::sky = 0;

Dolphin::Engine::Engine()
{
}

Dolphin::Engine::~Engine()
{
	delete render;
	delete huddev;
	delete scenecreator;
	delete scenecomponentbuilderogre;
	delete ocean;
	delete terrain;
	scenecomponents.clear();
	delete physics;
}

void Dolphin::Engine::StepPhysicsSymulation()
{
	//physics->StepSimulation();
}

std::vector<Dolphin::SceneComponent*>* Dolphin::Engine::GetSceneComponents()
{
	return &scenecomponents;
}

Dolphin::Ocean* Dolphin::Engine::GetOcean()
{
	return ocean;
}

Dolphin::Sky* Dolphin::Engine::GetSky()
{
	return sky;
}



void Dolphin::Engine::go()
{
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(Dolphin::KEYPRESS, std::bind(&Engine::StepPhysicsSymulation, this));
	huddev = new HudDev();
	render = new Render();
	ocean = new Ocean();
	
	sky = new Sky();
	scenecreator = new SceneCreator();
	render->Initialize();
	
	
	scenecomponentbuilderogre = new SceneComponentBuilderOgre();
	scenecomponents = *scenecreator->CreateScene(scenecomponentbuilderogre);
	ocean->SetHydrax(static_cast<OceanSceneComponent*>(Engine::GetSceneComponents()->back())->GetHydrax());
	ocean->InitHydrax();
	sky->SetSkyX(static_cast<SkySceneComponent*>(Engine::GetSceneComponents()->at(Engine::GetSceneComponents()->size() - 2))->GetSkyX());  //todo better acces to scene elements : by name in map?
	sky->InitSkyX(static_cast<SkySceneComponent*>(Engine::GetSceneComponents()->at(Engine::GetSceneComponents()->size() - 2))->GetSkyXBasicController());
	
	terrain = new Terrain();
	terrain->setupContent();
	render->SetTerrain(terrain);

	physics = new Physics();
	physics->Initialize();
	
	for (auto const& value : scenecomponents)
	{
		if (value->GetPhysicsStatus())
		{
			btRigidBody* body = physics->GetPhysicsWorldImporter()->getRigidBodyByName(value->GetModel()->getName().c_str());
			body->setUserPointer(value->GetModel()->getParentSceneNode());
		}
	}
	terrain->SetPhysics(physics);
	render->SetPhysics(physics);

	/*RenderOgre::GetSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
	RenderOgre::GetSceneManager()->setShadowFarDistance(1000);*/
	
	//physics->CreateBasicRigidBody();
	//physics->CreateDynamicRigidBody();

	//physics->body->setUserPointer(scenecomponents[0]->GetModel()->getParentSceneNode());
	
	Ogre::Timer timer;
	unsigned long startTime = timer.getMicroseconds();

	double timeSinceLast = 1.0 / 60.0;

	while (!RenderOgre::IsShutDown())
	{
		Ogre::WindowEventUtilities::messagePump();
		physics->StepSimulation(static_cast<float>(timeSinceLast));
		RenderOgre::GetOgreRoot()->renderOneFrame(static_cast<float>(timeSinceLast));
		unsigned long endTime = timer.getMicroseconds();
		timeSinceLast = (endTime - startTime) / 1000000.0;
		timeSinceLast = std::min(1.0, timeSinceLast); //Prevent from going haywire.
		startTime = endTime;
	}

	/*
	float TimeStep = 1 / 60.0f;
	float TimeStepAccumulator = 0.0f;

	while (!RenderOgre::IsShutDown())
	{
		Ogre::Timer frameTimer;
		Ogre::WindowEventUtilities::messagePump();

		TimeStepAccumulator += timeSinceLastFrame;
		while (TimeStepAccumulator >= TimeStep)
		{
			physics->StepSimulation(TimeStep);
			TimeStepAccumulator -= TimeStep;
		}

		

		RenderOgre::GetOgreRoot()->renderOneFrame(timeSinceLastFrame);

		timeSinceLastFrame = frameTimer.getMicroseconds() / 1000000.0;;

		if (timeSinceLastFrame > 1.0f)
			timeSinceLastFrame = 1.0f;

		frameTimer.reset();
	}*/
	physics->Clean();
	// Clean up
	// destroyScene();
}