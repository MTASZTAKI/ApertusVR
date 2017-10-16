/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef _SCENECOMPONENTBUILDER_H_
#define _SCENECOMPONENTBUILDER_H_

#include <OgreCamera.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include "scenecomponents.h"

#include "define.h"

namespace Dolphin
{
	class Light;
	class SceneComponentBuilder
	{
	public:
		SceneComponentBuilder();
		virtual ~SceneComponentBuilder();
		virtual Node* CreateNode() { return 0; };
		//virtual	Node* GetNode();
		virtual Model* CreateModel() { return 0; };
		//virtual Model* GetModel();
		//virtual Light* CreateLight();
		//virtual Light* GetLight();
		//virtual Material* CreateMaterial();
		//virtual Material* GetMaterial();
	protected:
	};
}

#endif //_SCENECOMPONENTBUILDER_H_
