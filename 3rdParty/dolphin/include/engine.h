/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

namespace Dolphin
{
	class Render;
	class HudDev;
	class SceneCreator;
	class SceneComponent;
	class SceneComponentBuilderOgre;
	class Physics;
	class Ocean;
	class Sky;
	class Terrain;
	class Engine
	{
	public:
		Engine();
		virtual ~Engine();
		void go();
		static std::vector<SceneComponent*>* GetSceneComponents();
		static Ocean* GetOcean();
		static Sky* GetSky();
	private:
		Render* render;
		HudDev* huddev;
		static Ocean* ocean;
		static Sky* sky;
		Terrain* terrain;
		SceneCreator* scenecreator;
		Physics* physics;
		SceneComponentBuilderOgre* scenecomponentbuilderogre;
		static std::vector<SceneComponent*> scenecomponents;
		void Engine::StepPhysicsSymulation();
	};
}
#endif //ENGINE_H
