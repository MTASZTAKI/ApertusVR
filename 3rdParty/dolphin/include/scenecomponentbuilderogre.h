/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef _SCENECOMPONENTBUILDEROGRE_H_
#define _SCENECOMPONENTBUILDEROGRE_H_

#include "scenecomponentbuilder.h"

namespace Dolphin
{
	class SceneComponentBuilderOgre : public SceneComponentBuilder
	{
	public:
		virtual ~SceneComponentBuilderOgre();
		Node* CreateNode();
		//Node* GetNode();
		Model* CreateModel();
		//Model* GetModel();
		//Light* CreateLight();
		//Light* GetLight();
		//Material* CreateMaterial();
		//Material* GetMaterial();
	private:
		Model* entity;
		Node* node;

	};
}

#endif //_SCENECOMPONENTBUILDEROGRE_H_