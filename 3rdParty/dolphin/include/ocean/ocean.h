/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef _OCEAN_H
#define _OCEAN_H

#include <vector>
#include "Hydrax.h"
#include "Noise/Perlin/Perlin.h"
#include "Modules/ProjectedGrid/ProjectedGrid.h"

#include <Overlay/OgreTextAreaOverlayElement.h>

namespace Dolphin
{

	#define _def_SkyBoxNum 3


	class Ocean
	{
	public:
		Ocean();
		~Ocean();
		void InitHydrax();
		void changeSkyBox();
		Hydrax::Hydrax* GetHydrax();

		static int mCurrentSkyBox;
		void SetHydrax(Hydrax::Hydrax* hydrax);

		void SetSunPosition(Ogre::Vector3 position);

		void Update();

		static const Ogre::String mSkyBoxes[_def_SkyBoxNum];

		static const Ogre::Vector3 mSunPosition[_def_SkyBoxNum];

		static const Ogre::Vector3 mSunColor[_def_SkyBoxNum];
	private:
		Hydrax::Hydrax *mHydrax = 0;

		

		

		Ogre::TextAreaOverlayElement* mTextArea = 0;

		float seed_ = 801;

		float rnd_(const float& min, const float& max);
	};
}

#endif //_OCEAN_H