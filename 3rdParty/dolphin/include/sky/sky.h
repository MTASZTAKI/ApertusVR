/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef _SKY_H
#define _SKY_H

#include <vector>
#include "SkyX.h"

namespace Dolphin
{
	/*struct SkyXSettings
	{
		
		SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt,
			const bool& lc, const bool& vc, const Ogre::Real& vcws, const bool& vcauto, const Ogre::Radian& vcwd,
			const Ogre::Vector3& vcac, const Ogre::Vector4& vclr, const Ogre::Vector4& vcaf, const Ogre::Vector2& vcw,
			const bool& vcl, const Ogre::Real& vclat, const Ogre::Vector3& vclc, const Ogre::Real& vcltm)
			: time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(vc), vcWindSpeed(vcws)
			, vcAutoupdate(vcauto), vcWindDir(vcwd), vcAmbientColor(vcac), vcLightResponse(vclr), vcAmbientFactors(vcaf), vcWheater(vcw)
			, vcLightnings(vcl), vcLightningsAT(vclat), vcLightningsColor(vclc), vcLightningsTM(vcltm)
		{}

		
		SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt,
			const bool& lc, const bool& vc, const Ogre::Real& vcws, const bool& vcauto, const Ogre::Radian& vcwd,
			const Ogre::Vector3& vcac, const Ogre::Vector4& vclr, const Ogre::Vector4& vcaf, const Ogre::Vector2& vcw)
			: time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(vc), vcWindSpeed(vcws)
			, vcAutoupdate(vcauto), vcWindDir(vcwd), vcAmbientColor(vcac), vcLightResponse(vclr), vcAmbientFactors(vcaf), vcWheater(vcw), vcLightnings(false)
		{}

		
		SkyXSettings(const Ogre::Vector3 t, const Ogre::Real& tm, const Ogre::Real& mp, const SkyX::AtmosphereManager::Options& atmOpt, const bool& lc)
			: time(t), timeMultiplier(tm), moonPhase(mp), atmosphereOpt(atmOpt), layeredClouds(lc), volumetricClouds(false), vcLightnings(false)
		{}

		/// Time
		Ogre::Vector3 time;
		/// Time multiplier
		Ogre::Real timeMultiplier;
		/// Moon phase
		Ogre::Real moonPhase;
		/// Atmosphere options
		SkyX::AtmosphereManager::Options atmosphereOpt;
		/// Layered clouds?
		bool layeredClouds;
		/// Volumetric clouds?
		bool volumetricClouds;
		/// VClouds wind speed
		Ogre::Real vcWindSpeed;
		/// VClouds autoupdate
		bool vcAutoupdate;
		/// VClouds wind direction
		Ogre::Radian vcWindDir;
		/// VClouds ambient color
		Ogre::Vector3 vcAmbientColor;
		/// VClouds light response
		Ogre::Vector4 vcLightResponse;
		/// VClouds ambient factors
		Ogre::Vector4 vcAmbientFactors;
		/// VClouds wheater
		Ogre::Vector2 vcWheater;
		/// VClouds lightnings?
		bool vcLightnings;
		/// VClouds lightnings average aparition time
		Ogre::Real vcLightningsAT;
		/// VClouds lightnings color
		Ogre::Vector3 vcLightningsColor;
		/// VClouds lightnings time multiplier
		Ogre::Real vcLightningsTM;
	};*/

	class Sky
	{
	public:
		Sky();
		~Sky();
		void InitSkyX(SkyX::BasicController* skyxbasiccont);
		
		SkyX::SkyX* GetSkyX();
		void SetSkyX(SkyX::SkyX* skyx);
		void InitLights();
		void UpdateLights();
		Ogre::Vector3 GetSunLightDirection();
		Ogre::Light* GetSunLight();
		//void setPreset(const SkyXSettings& preset);
	
	private:
		SkyX::SkyX* mSkyX = 0;
		Ogre::Light* sunlight;
		Ogre::Light* skylight;
		SkyX::ColorGradient mWaterGradient,
			mSunGradient,
			mAmbientGradient;
		//static SkyXSettings mPresets[6];
		SkyX::BasicController* skyxbasiccontroller;
		
		
	};
}

#endif //_SKY_H