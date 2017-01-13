/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_OGRECONVERSIONS_H
#define APE_OGRECONVERSIONS_H

#include "ApeVector3.h"
#include "ApeVector2.h"
#include "ApeVector4.h"
#include "ApeQuaternion.h"
#include "ApeColor.h"
#include "ApeILight.h"
#include "ApeIFileGeometry.h"
#include "OgreVector3.h"
#include "OgreVector2.h"
#include "OgreVector4.h"
#include "OgreQuaternion.h"
#include "OgreColourValue.h"
#include "OgreLight.h"

namespace Ape
{
	inline Ogre::Vector3 ConversionToOgre(const Ape::Vector3& p_vec)
	{
		return Ogre::Vector3(p_vec.x, p_vec.y, p_vec.z);
	}

	inline Ape::Vector3 ConversionFromOgre(const Ogre::Vector3& p_vec)
	{
		return Ape::Vector3(p_vec.x, p_vec.y, p_vec.z);
	}

	inline Ogre::Vector2 ConversionToOgre(const Ape::Vector2& p_vec)
	{
		return Ogre::Vector2(p_vec.x, p_vec.y);
	}

	inline Ape::Vector2 ConversionFromOgre(const Ogre::Vector2& p_vec)
	{
		return Ape::Vector2(p_vec.x, p_vec.y);
	}

	inline Ogre::Quaternion ConversionToOgre(const Ape::Quaternion& p_q)
	{
		return Ogre::Quaternion(p_q.w, p_q.x, p_q.y, p_q.z);
	}

	inline Ape::Quaternion ConversionFromOgre(const Ogre::Quaternion& p_q)
	{
		return Ape::Quaternion(p_q.w, p_q.x, p_q.y, p_q.z);
	}

	inline Ogre::Vector4 ConversionToOgre(const Ape::Vector4& p_vec)
	{
		return Ogre::Vector4(p_vec.x, p_vec.y, p_vec.z, p_vec.w);
	}

	inline Ape::Vector4 ConversionFromOgre(const Ogre::Vector4& p_v4)
	{
		return Ape::Vector4(p_v4.x, p_v4.y, p_v4.z, p_v4.w);
	}

	inline Ogre::ColourValue ConversionToOgre(const Ape::Color& p_colv)
	{
		return Ogre::ColourValue(p_colv.r, p_colv.g, p_colv.b, p_colv.a);
	}

	inline Ape::Color ConversionFromOgre(const Ogre::ColourValue& p_colv)
	{
		return Ape::Color(p_colv.r, p_colv.g, p_colv.b, p_colv.a);
	}

	inline std::vector<Ogre::ColourValue> ConversionToOgre(const Ape::ColorVector &cList)
	{
		std::vector<Ogre::ColourValue> vec;
		vec.reserve(cList.size());
		for(auto it : cList) {
			vec.push_back(ConversionToOgre(it));
		}
		return vec;
	}

	inline Ape::ColorVector ConversionFromOgre(const std::vector<Ogre::ColourValue> &cVec)
	{
		Ape::ColorVector cList;
		cList.reserve(cVec.size());
		for(auto it : cVec) {
			cList.push_back(ConversionFromOgre(it));
		}
		return cList;
	}

	inline Ogre::Degree ConversionToOgre(const Ape::Degree& mwDeg)
	{
		return Ogre::Degree(mwDeg.degree);
	}

	inline Ape::Degree ConversionFromOgre(const Ogre::Degree& oDeg)
	{
		return Ape::Degree(oDeg.valueDegrees());
	}

	inline Ogre::Radian ConversionToOgre(const Ape::Radian& mwRad)
	{
		return Ogre::Radian(mwRad.radian);
	}

	inline Ape::Radian ConversionFromOgre(const Ogre::Radian& oRad)
	{
		return Ape::Radian(oRad.valueRadians());
	}

	inline Ape::Light::Type ConversionFromOgre(Ogre::Light::LightTypes type)
	{
		switch (type)
		{
		case Ogre::Light::LT_POINT:
			return Ape::Light::Type::POINT;
		case Ogre::Light::LT_DIRECTIONAL:
			return Ape::Light::Type::DIRECTIONAL;
		case Ogre::Light::LT_SPOTLIGHT:
			return Ape::Light::Type::SPOT;
		default:
			return Ape::Light::Type::INVALID;
		}		
	}

	inline Ogre::Light::LightTypes ConversionToOgre(Ape::Light::Type type)
	{
		switch (type)
		{
		case Ape::Light::Type::SPOT:
			return Ogre::Light::LightTypes::LT_SPOTLIGHT;
		case Ape::Light::Type::DIRECTIONAL:
			return Ogre::Light::LightTypes::LT_DIRECTIONAL;
		case Ape::Light::Type::POINT:
			return Ogre::Light::LightTypes::LT_POINT;
		case Ape::Light::Type::INVALID:
		default:
			return Ogre::Light::LightTypes(-1);
		}
	}
}

#endif
