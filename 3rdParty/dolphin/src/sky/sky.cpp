/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "sky/sky.h"
#include "renderogre.h"
#include "Engine.h"
#include "ocean/ocean.h"
#include "scenecomponents.h"



/*
Dolphin::SkyXSettings Dolphin::Sky::mPresets[] = {
	// Sunset
	Dolphin::SkyXSettings(Ogre::Vector3(8.85f, 7.5f, 20.5f), -0.08f, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0022f, 0.000675f, 30, Ogre::Vector3(0.57f, 0.52f, 0.44f), -0.991f, 3, 4), false, true, 300, false, Ogre::Radian(270), Ogre::Vector3(0.63f, 0.63f, 0.7f), Ogre::Vector4(0.35, 0.2, 0.92, 0.1), Ogre::Vector4(0.4, 0.7, 0, 0), Ogre::Vector2(0.8, 1)),
	// Clear
	Dolphin::SkyXSettings(Ogre::Vector3(17.16f, 7.5f, 20.5f), 0, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0017f, 0.000675f, 30, Ogre::Vector3(0.57f, 0.54f, 0.44f), -0.991f, 2.5f, 4), false),
	// Thunderstorm 1
	Dolphin::SkyXSettings(Ogre::Vector3(12.23, 7.5f, 20.5f), 0, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.00545f, 0.000375f, 30, Ogre::Vector3(0.55f, 0.54f, 0.52f), -0.991f, 1, 4), false, true, 300, false, Ogre::Radian(0), Ogre::Vector3(0.63f, 0.63f, 0.7f), Ogre::Vector4(0.25, 0.4, 0.5, 0.1), Ogre::Vector4(0.45, 0.3, 0.6, 0.1), Ogre::Vector2(1, 1), true, 0.5, Ogre::Vector3(1, 0.976, 0.92), 2),
	// Thunderstorm 2
	Dolphin::SkyXSettings(Ogre::Vector3(10.23, 7.5f, 20.5f), 0, 0, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.00545f, 0.000375f, 30, Ogre::Vector3(0.55f, 0.54f, 0.52f), -0.991f, 0.5, 4), false, true, 300, false, Ogre::Radian(0), Ogre::Vector3(0.63f, 0.63f, 0.7f), Ogre::Vector4(0, 0.02, 0.34, 0.24), Ogre::Vector4(0.29, 0.3, 0.6, 1), Ogre::Vector2(1, 1), true, 0.5, Ogre::Vector3(0.95, 1, 1), 2),
	// Desert
	Dolphin::SkyXSettings(Ogre::Vector3(7.59f, 7.5f, 20.5f), 0, -0.8f, SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.0072f, 0.000925f, 30, Ogre::Vector3(0.71f, 0.59f, 0.53f), -0.997f, 2.5f, 1), true),
	// Night
	Dolphin::SkyXSettings(Ogre::Vector3(21.5f, 7.5, 20.5), 0.03, -0.25, SkyX::AtmosphereManager::Options(), true)
};



/*void Dolphin::Sky::setPreset(const SkyXSettings& preset)
{
	mSkyX->setTimeMultiplier(preset.timeMultiplier);
	skyxbasiccontroller->setTime(preset.time);
	skyxbasiccontroller->setMoonPhase(preset.moonPhase);
	mSkyX->getAtmosphereManager()->setOptions(preset.atmosphereOpt);

	// Layered clouds
	if (preset.layeredClouds)
	{
		// Create layer cloud
		if (mSkyX->getCloudsManager()->getCloudLayers().empty())
		{
			mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options());
		}
	}
	else
	{
		// Remove layer cloud
		if (!mSkyX->getCloudsManager()->getCloudLayers().empty())
		{
			mSkyX->getCloudsManager()->removeAll();
		}
	}

	mSkyX->getVCloudsManager()->setWindSpeed(preset.vcWindSpeed);
	mSkyX->getVCloudsManager()->setAutoupdate(preset.vcAutoupdate);

	SkyX::VClouds::VClouds* vclouds = mSkyX->getVCloudsManager()->getVClouds();

	vclouds->setWindDirection(preset.vcWindDir);
	vclouds->setAmbientColor(preset.vcAmbientColor);
	vclouds->setLightResponse(preset.vcLightResponse);
	vclouds->setAmbientFactors(preset.vcAmbientFactors);
	vclouds->setWheater(preset.vcWheater.x, preset.vcWheater.y, false);

	if (preset.volumetricClouds)
	{
		// Create VClouds
		if (!mSkyX->getVCloudsManager()->isCreated())
		{
			// SkyX::MeshManager::getSkydomeRadius(...) works for both finite and infinite(=0) camera far clip distances
			mSkyX->getVCloudsManager()->create(mSkyX->getMeshManager()->getSkydomeRadius(RenderOgre::getmCamera()));
		}
	}
	else
	{
		// Remove VClouds
		if (mSkyX->getVCloudsManager()->isCreated())
		{
			mSkyX->getVCloudsManager()->remove();
		}
	}

	vclouds->getLightningManager()->setEnabled(preset.vcLightnings);
	vclouds->getLightningManager()->setAverageLightningApparitionTime(preset.vcLightningsAT);
	vclouds->getLightningManager()->setLightningColor(preset.vcLightningsColor);
	vclouds->getLightningManager()->setLightningTimeMultiplier(preset.vcLightningsTM);

	

	// Reset camera position/orientation
	RenderOgre::getmCamera()->setPosition(0, 0, 0);
	RenderOgre::getmCamera()->setDirection(0, 0, 1);

	mSkyX->update(0);
}*/

Dolphin::Sky::Sky()
{

}


Dolphin::Sky::~Sky()
{

}

void Dolphin::Sky::InitSkyX(SkyX::BasicController* skyxbasiccont)
{
	skyxbasiccontroller = skyxbasiccont;
	SkyX::CfgFileManager *cfg = new SkyX::CfgFileManager(mSkyX, skyxbasiccontroller, RenderOgre::getmCamera());
	cfg->load("SkyXDefault.skx");
	mSkyX->create();
	
	mSkyX->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(1, -1));
	RenderOgre::GetOgreRoot()->addFrameListener(mSkyX);
	RenderOgre::getmWindow()->addListener(mSkyX);
	
	mSkyX->getGPUManager()->addGroundPass(
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().
		getByName("Terrain"))->getTechnique(0)->createPass(), 250, Ogre::SBT_TRANSPARENT_COLOUR);
	

	//mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));

	//setPreset(mPresets[0]);

	InitLights();
	UpdateLights();
}

SkyX::SkyX* Dolphin::Sky::GetSkyX()
{
	return mSkyX;
}

void Dolphin::Sky::SetSkyX(SkyX::SkyX* skyx)
{
	mSkyX = skyx;
}


Ogre::Vector3 Dolphin::Sky::GetSunLightDirection()
{
	return mSkyX->getController()->getSunDirection();
}

Ogre::Light*  Dolphin::Sky::GetSunLight()
{
	return sunlight;
}

void Dolphin::Sky::InitLights()
{
	
	// Water
	mWaterGradient = SkyX::ColorGradient();
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209, 0.535822, 0.779105)*0.4, 1));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209, 0.535822, 0.729105)*0.3, 0.8));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209, 0.535822, 0.679105)*0.25, 0.6));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209, 0.535822, 0.679105)*0.2, 0.5));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209, 0.535822, 0.679105)*0.1, 0.45));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209, 0.535822, 0.679105)*0.025, 0));
	// Sun
	mSunGradient = SkyX::ColorGradient();
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8, 0.75, 0.55)*1.5, 1.0f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8, 0.75, 0.55)*1.4, 0.75f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8, 0.75, 0.55)*1.3, 0.5625f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.6, 0.5, 0.2)*1.5, 0.5f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5, 0.5, 0.5)*0.25, 0.45f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5, 0.5, 0.5)*0.01, 0.0f));
	// Ambient
	mAmbientGradient = SkyX::ColorGradient();
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1, 1, 1) * 1, 1.0f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1, 1, 1) * 1, 0.6f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1, 1, 1)*0.6, 0.5f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1, 1, 1)*0.3, 0.45f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1, 1, 1)*0.1, 0.35f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1, 1, 1)*0.05, 0.0f));
	
	RenderOgre::GetSceneManager()->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));

	skylight = RenderOgre::GetSceneManager()->createLight("skylight");
	skylight->setCastShadows(false);
	
	sunlight = RenderOgre::GetSceneManager()->createLight("sunlight");
	sunlight->setType(Ogre::Light::LT_DIRECTIONAL);
}

void Dolphin::Sky::UpdateLights()
{
	Ogre::Vector3 lightdir = -GetSunLightDirection();
	Ogre::Vector3 sunpos = RenderOgre::getmCamera()->getDerivedPosition() - lightdir*mSkyX->getMeshManager()->getSkydomeRadius(RenderOgre::getmCamera());
	// Calculate current color gradients point
	float point = (-lightdir.y + 1.0f) / 2.0f;

	Engine::GetOcean()->GetHydrax()->setWaterColor(mWaterGradient.getColor(point));

	Ogre::Vector3 suncolor = mSunGradient.getColor(point);
	skylight->setSpecularColour(Ogre::ColourValue(suncolor.x, suncolor.y, suncolor.z));
	
	Ogre::Vector3 ambientcolor = mAmbientGradient.getColor(point);
	sunlight->setDiffuseColour(Ogre::ColourValue(ambientcolor.x, ambientcolor.y, ambientcolor.z));

	skylight->setPosition(sunpos);
	sunlight->setDirection(lightdir);

	//sunlight->setPosition(100, 100, 100);

	SkyX::BasicController* skyxbasiccontroller;

	skyxbasiccontroller = static_cast<SkySceneComponent*>(Engine::GetSceneComponents()->at(Engine::GetSceneComponents()->size() - 2))->GetSkyXBasicController();

	if (skyxbasiccontroller->getTime().x > 12)
	{
		if (skyxbasiccontroller->getTime().x > skyxbasiccontroller->getTime().z)
		{
			skylight->setVisible(false);
		}
		else
		{
			skylight->setVisible(true);
		}
	}
	else
	{
		if (skyxbasiccontroller->getTime().x < skyxbasiccontroller->getTime().z)
		{
			skylight->setVisible(false);
		}
		else
		{
			skylight->setVisible(true);
		}
	}

	static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Terrain"))->getTechnique(0)->getPass(0)
		->getFragmentProgramParameters()->setNamedConstant("uLightY", skyxbasiccontroller->getSunDirection().y);
}