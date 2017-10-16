/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "scenecomponentbuilderogre.h"
#include "renderogre.h"

Dolphin::SceneComponentBuilderOgre::~SceneComponentBuilderOgre()
{
}

Dolphin::Node* Dolphin::SceneComponentBuilderOgre::CreateNode()
{
	Ogre::SceneNode* node = RenderOgre::GetSceneManager()->getRootSceneNode()->createChildSceneNode("node");
	return node;
}

Dolphin::Model* Dolphin::SceneComponentBuilderOgre::CreateModel()
{
	Ogre::Entity* entity = RenderOgre::GetSceneManager()->createEntity("Icosphere", "Icosphere.mesh");
	RenderOgre::GetSceneManager()->getSceneNode("node")->attachObject(entity);
	return entity;
}