/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef _SCENECREATOR_H
#define _SCENECREATOR_H

#include <vector>
#include "ocean/ocean.h"
#include "sky/sky.h"

namespace Dolphin
{
	class SceneComponent;
	class SceneComponentBuilder;
	class SceneCreator
	{
	public:
		SceneCreator();
		~SceneCreator();
		std::vector<SceneComponent*>* CreateScene(SceneComponentBuilder* componentbuilder);
	private:
		std::vector<SceneComponent*> scenecomponents;
		Hydrax::Hydrax* hydrax;
		SkyX::SkyX* skyx;
		SkyX::BasicController* mBasicController;
		
	};
}

#endif //_SCENECREATOR_H