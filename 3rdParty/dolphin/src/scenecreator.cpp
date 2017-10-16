/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "scenecreator.h"
#include "scenecomponentbuilderogre.h"
#include "scenecomponents.h"
#include "sceneloader.h"
#include "renderogre.h"

Dolphin::SceneCreator::SceneCreator()
{
}


Dolphin::SceneCreator::~SceneCreator()
{
	delete hydrax;
	delete skyx;
	delete mBasicController;
}

std::vector<Dolphin::SceneComponent*>* Dolphin::SceneCreator::CreateScene(SceneComponentBuilder* componentbuilder)
{
	Ogre::DotSceneLoader dotsceneloader;
	dotsceneloader.parseDotScene("test.scene", "General", RenderOgre::GetSceneManager());
	scenecomponents = *dotsceneloader.GetSceneComponents();
	hydrax = new Hydrax::Hydrax(RenderOgre::GetSceneManager(), RenderOgre::getmCamera(), RenderOgre::getmWindow()->getViewport(0));
	Dolphin::OceanSceneComponent* ocean = new Dolphin::OceanSceneComponent();
	//ToDo make "valid" scenecomponents... make scenecomponents vector to map for name?
	
	//ocean->SetNode(pParent);
	//ocean->SetModel(pEntity);
	
	ocean->SetHydrax(hydrax);
	ocean->SetPhysicsStatus(false);



	mBasicController = new SkyX::BasicController();
	skyx = new SkyX::SkyX(RenderOgre::GetSceneManager(),mBasicController);
	Dolphin::SkySceneComponent* sky = new Dolphin::SkySceneComponent();
	//ToDo make "valid" scenecomponents... make scenecomponents vector to map for name?
	sky->SetSkyX(skyx);
	sky->SetSkyXBasicController(mBasicController);
	sky->SetPhysicsStatus(false);

	scenecomponents.push_back(sky);
	scenecomponents.push_back(ocean);

	return &scenecomponents;
}