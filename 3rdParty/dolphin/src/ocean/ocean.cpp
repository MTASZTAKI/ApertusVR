/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "ocean/ocean.h"
#include "renderogre.h"
#include "scenecomponents.h"
#include "engine.h"

const Ogre::String Dolphin::Ocean::mSkyBoxes[_def_SkyBoxNum] = { "Sky/ClubTropicana", "Sky/EarlyMorning", "Sky/Clouds" };

const Ogre::Vector3 Dolphin::Ocean::mSunPosition[_def_SkyBoxNum] = { Ogre::Vector3(0, 10000, 0), Ogre::Vector3(0, 10000, 90000), Ogre::Vector3(0, 10000, 0) };

const Ogre::Vector3 Dolphin::Ocean::mSunColor[_def_SkyBoxNum] = { Ogre::Vector3(1, 0.9, 0.6), Ogre::Vector3(1, 0.6, 0.4), Ogre::Vector3(0.45, 0.45, 0.45) };

int Dolphin::Ocean::mCurrentSkyBox = 0;

Dolphin::Ocean::Ocean()
{

}


Dolphin::Ocean::~Ocean()
{

}

void Dolphin::Ocean::changeSkyBox()
{
	// Change skybox
	//RenderOgre::GetSceneManager()->setSkyBox(true, mSkyBoxes[mCurrentSkyBox], 99999 * 3, true);

	// Update Hydrax sun position and colour
	mHydrax->setSunPosition(mSunPosition[mCurrentSkyBox]);
	mHydrax->setSunColor(mSunColor[mCurrentSkyBox]);

	// Update light 0 light position and colour
	RenderOgre::GetSceneManager()->getLight("Light0")->setPosition(mSunPosition[mCurrentSkyBox]);
	RenderOgre::GetSceneManager()->getLight("Light0")->setSpecularColour(mSunColor[mCurrentSkyBox].x, mSunColor[mCurrentSkyBox].y, mSunColor[mCurrentSkyBox].z);

	// Update text area
	mTextArea->setCaption("Hydrax 0.5.1 demo application\nCurrent water preset: " + Ogre::StringUtil::split(mSkyBoxes[mCurrentSkyBox], "/")[1] + " (" + Ogre::StringConverter::toString(mCurrentSkyBox + 1) + "/3). Press 'm' to switch water presets.");

	// Log
	Ogre::LogManager::getSingleton().logMessage("Skybox " + mSkyBoxes[mCurrentSkyBox] + " selected. (" + Ogre::StringConverter::toString(mCurrentSkyBox + 1) + "/" + Ogre::StringConverter::toString(_def_SkyBoxNum) + ")");
}

float Dolphin::Ocean::rnd_(const float& min, const float& max)
{
	seed_ += Ogre::Math::PI*2.8574f + seed_*(0.3424f - 0.12434f + 0.452345f);
	if (seed_ > 10000000000) seed_ -= 10000000000;
	return ((max - min)*Ogre::Math::Abs(Ogre::Math::Sin(Ogre::Radian(seed_))) + min);
}

void Dolphin::Ocean::InitHydrax()
{
	Hydrax::Module::ProjectedGrid *mModule
		= new Hydrax::Module::ProjectedGrid(mHydrax,
											new Hydrax::Noise::Perlin(/*Generic one*/),
											Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),
											Hydrax::MaterialManager::NM_VERTEX,
											Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

	mHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

	mHydrax->loadCfg("HydraxDemo.hdx");

	mHydrax->create();

	mHydrax->getMaterialManager()->addDepthTechnique(
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Terrain"))
		->createTechnique());
}

Hydrax::Hydrax* Dolphin::Ocean::GetHydrax()
{
	return mHydrax;
}

void Dolphin::Ocean::SetHydrax(Hydrax::Hydrax* hydrax)
{
	mHydrax = hydrax;
}

void Dolphin::Ocean::SetSunPosition(Ogre::Vector3 position)
{
	mHydrax->setSunPosition(position);
}

void Dolphin::Ocean::Update()
{
	SetSunPosition(RenderOgre::GetSceneManager()->getLight("skylight")->getPosition());
}