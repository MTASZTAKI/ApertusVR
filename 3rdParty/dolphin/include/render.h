/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef RENDER_H
#define RENDER_H

#include "scenecomponents.h"
#include "terrain.h"
#include "physics.h"

namespace OIS
{
	class KeyEvent;
}

namespace Dolphin
{
	class RenderOgre;

	class Render
	{
	public:
		Render(void);
		virtual ~Render(void);
		void CycleTextureFilteringMode(const OIS::KeyEvent arg);
		void CyclePolygonRenderingMode(const OIS::KeyEvent arg);
		void RefreshAllTexture(const OIS::KeyEvent arg);
		void TakeScreeShot(const OIS::KeyEvent arg);
		void Initialize();
		void SetTerrain(Terrain* tr);
		void SetPhysics(Physics* ph);
	private:
		RenderOgre* renderogre;
	};
}
#endif //ENGINE_H
