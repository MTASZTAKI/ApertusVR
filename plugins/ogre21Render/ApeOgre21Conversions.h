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

#ifndef APE_OGRE21CONVERSIONS_H
#define APE_OGRE21CONVERSIONS_H

#include "datatypes/apeVector3.h"
#include "datatypes/apeVector2.h"
#include "datatypes/apeVector4.h"
#include "datatypes/apeMatrix4.h"
#include "datatypes/apeQuaternion.h"
#include "datatypes/apeColor.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIFileGeometry.h"
#include "OgreVector3.h"
#include "OgreVector2.h"
#include "OgreVector4.h"
#include "OgreQuaternion.h"
#include "OgreColourValue.h"
#include "OgreLight.h"
#include "OgreMatrix4.h"

namespace ape
{
	inline Ogre::Vector3 ConversionToOgre21(const ape::Vector3& p_vec)
	{
		return Ogre::Vector3(p_vec.x, p_vec.y, p_vec.z);
	}

	inline ape::Vector3 ConversionFromOgre21(const Ogre::Vector3& p_vec)
	{
		return ape::Vector3(p_vec.x, p_vec.y, p_vec.z);
	}

	inline Ogre::Matrix4 ConversionToOgre21(const ape::Matrix4& p_mat4)
	{
		return Ogre::Matrix4(
			p_mat4.m[0][0], p_mat4.m[0][1], p_mat4.m[0][2], p_mat4.m[0][3],
			p_mat4.m[1][0], p_mat4.m[1][1], p_mat4.m[1][2], p_mat4.m[1][3],
			p_mat4.m[2][0], p_mat4.m[2][1], p_mat4.m[2][2], p_mat4.m[2][3],
			p_mat4.m[3][0], p_mat4.m[3][1], p_mat4.m[3][2], p_mat4.m[3][3]
		);
	}

	inline ape::Matrix4 ConversionFromOgre21(const Ogre::Matrix4& p_mat4)
	{
		return ape::Matrix4(
			p_mat4[0][0], p_mat4[0][1], p_mat4[0][2], p_mat4[0][3],
			p_mat4[1][0], p_mat4[1][1], p_mat4[1][2], p_mat4[1][3],
			p_mat4[2][0], p_mat4[2][1], p_mat4[2][2], p_mat4[2][3],
			p_mat4[3][0], p_mat4[3][1], p_mat4[3][2], p_mat4[3][3]
		);
	}

	inline Ogre::Vector2 ConversionToOgre21(const ape::Vector2& p_vec)
	{
		return Ogre::Vector2(p_vec.x, p_vec.y);
	}

	inline ape::Vector2 ConversionFromOgre21(const Ogre::Vector2& p_vec)
	{
		return ape::Vector2(p_vec.x, p_vec.y);
	}

	inline Ogre::Quaternion ConversionToOgre21(const ape::Quaternion& p_q)
	{
		return Ogre::Quaternion(p_q.w, p_q.x, p_q.y, p_q.z);
	}

	inline ape::Quaternion ConversionFromOgre21(const Ogre::Quaternion& p_q)
	{
		return ape::Quaternion(p_q.w, p_q.x, p_q.y, p_q.z);
	}

	inline Ogre::Vector4 ConversionToOgre21(const ape::Vector4& p_vec)
	{
		return Ogre::Vector4(p_vec.x, p_vec.y, p_vec.z, p_vec.w);
	}

	inline ape::Vector4 ConversionFromOgre21(const Ogre::Vector4& p_v4)
	{
		return ape::Vector4(p_v4.x, p_v4.y, p_v4.z, p_v4.w);
	}

	inline Ogre::ColourValue ConversionToOgre21(const ape::Color& p_colv)
	{
		return Ogre::ColourValue(p_colv.r, p_colv.g, p_colv.b, p_colv.a);
	}
	inline Ogre::Vector3 ConversionToOgre21_SCPECULAR(const ape::Color& p_colv)
	{
		return Ogre::Vector3(p_colv.r, p_colv.g, p_colv.b);
	}

	inline Ogre::Vector3 ConversionToOgre21_Alfaless(const ape::Color& p_colv)
	{
		return Ogre::Vector3(p_colv.r, p_colv.g, p_colv.b);
	}

	inline float ConversionToOgre21_Alfa(const ape::Color& p_colv)
	{
		return p_colv.a;
	}

	inline ape::Color ConversionFromOgre21(const Ogre::ColourValue& p_colv)
	{
		return ape::Color(p_colv.r, p_colv.g, p_colv.b, p_colv.a);
	}

	inline std::vector<Ogre::ColourValue> ConversionToOgre21(const ape::ColorVector &cList)
	{
		std::vector<Ogre::ColourValue> vec;
		vec.reserve(cList.size());
		for (auto it : cList) {
			vec.push_back(ConversionToOgre21(it));
		}
		return vec;
	}



	inline ape::ColorVector ConversionFromOgre21(const std::vector<Ogre::ColourValue> &cVec)
	{
		ape::ColorVector cList;
		cList.reserve(cVec.size());
		for (auto it : cVec) {
			cList.push_back(ConversionFromOgre21(it));
		}
		return cList;
	}

	inline Ogre::Degree ConversionToOgre21(const ape::Degree& mwDeg)
	{
		return Ogre::Degree(mwDeg.degree);
	}

	inline ape::Degree ConversionFromOgre21(const Ogre::Degree& oDeg)
	{
		return ape::Degree(oDeg.valueDegrees());
	}

	inline Ogre::SceneBlendType ConversionToOgre21(const ape::Pass::SceneBlendingType type)
	{
		if (type == ape::Pass::SceneBlendingType::ADD)
			return Ogre::SceneBlendType::SBT_ADD;
		else if (type == ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA)
			return Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA;
		else if (type == ape::Pass::SceneBlendingType::REPLACE)
			return Ogre::SceneBlendType::SBT_REPLACE;
	}

	inline ape::Pass::SceneBlendingType ConversionFromOgre21(const Ogre::SceneBlendType type)
	{
		if (type == Ogre::SceneBlendType::SBT_ADD)
			return ape::Pass::SceneBlendingType::ADD;
		else if (type == Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA)
			return ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA;
		else if (type == Ogre::SceneBlendType::SBT_REPLACE)
			return ape::Pass::SceneBlendingType::REPLACE;
	}

	inline Ogre::CullingMode ConversionToOgre21(const ape::Material::CullingMode type)
	{
		if (type == ape::Material::CullingMode::NONE_CM)
			return Ogre::CullingMode::CULL_NONE;
		else if (type == ape::Material::CullingMode::CLOCKWISE)
			return Ogre::CullingMode::CULL_CLOCKWISE;
		else if (type == ape::Material::CullingMode::ANTICLOCKWISE)
			return Ogre::CullingMode::CULL_ANTICLOCKWISE;
	}

	inline ape::Material::CullingMode ConversionFromOgre21(const Ogre::CullingMode type)
	{
		if (type == Ogre::CullingMode::CULL_NONE)
			return ape::Material::CullingMode::NONE_CM;
		else if (type == Ogre::CullingMode::CULL_CLOCKWISE)
			return ape::Material::CullingMode::CLOCKWISE;
		else if (type == Ogre::CullingMode::CULL_ANTICLOCKWISE)
			return ape::Material::CullingMode::ANTICLOCKWISE;
	}

	inline Ogre::FilterOptions ConversionToOgre21(const ape::Texture::Filtering type)
	{
		if (type == ape::Texture::Filtering::ANISOTROPIC)
			return Ogre::FilterOptions::FO_ANISOTROPIC;
		else if (type == ape::Texture::Filtering::LINEAR)
			return Ogre::FilterOptions::FO_LINEAR;
		else if (type == ape::Texture::Filtering::POINT)
			return Ogre::FilterOptions::FO_POINT;
	}

	inline ape::Texture::Filtering ConversionFromOgre21(const Ogre::FilterOptions type)
	{
		if (type == Ogre::FilterOptions::FO_ANISOTROPIC)
			return ape::Texture::Filtering::ANISOTROPIC;
		else if (type == Ogre::FilterOptions::FO_LINEAR)
			return ape::Texture::Filtering::LINEAR;
		else if (type == Ogre::FilterOptions::FO_POINT)
			return ape::Texture::Filtering::POINT;
	}

	inline Ogre::PixelFormat ConversionToOgre21(const ape::Texture::PixelFormat type)
	{
		if (type == ape::Texture::PixelFormat::A8R8G8B8)
			return Ogre::PixelFormat::PF_A8R8G8B8;
		else if (type == ape::Texture::PixelFormat::R8G8B8)
			return Ogre::PixelFormat::PF_R8G8B8;
	}

	inline ape::Texture::PixelFormat ConversionFromOgre21(const Ogre::PixelFormat type)
	{
		if (type == Ogre::PixelFormat::PF_A8R8G8B8)
			return ape::Texture::PixelFormat::A8R8G8B8;
		else if (type == Ogre::PixelFormat::PF_R8G8B8)
			return ape::Texture::PixelFormat::R8G8B8;
	}

	inline Ogre::TextureUsage ConversionToOgre21(const ape::Texture::Usage type)
	{
		if (type == ape::Texture::Usage::RENDERTARGET)
			return Ogre::TextureUsage::TU_RENDERTARGET;
		else if (type == ape::Texture::Usage::DYNAMIC_WRITE_ONLY)
			return Ogre::TextureUsage::TU_DYNAMIC_WRITE_ONLY;
	}

	inline ape::Texture::Usage ConversionFromOgre21(const Ogre::TextureUsage type)
	{
		if (type == Ogre::TextureUsage::TU_RENDERTARGET)
			return ape::Texture::Usage::RENDERTARGET;
		else if (type == Ogre::TextureUsage::TU_DYNAMIC_WRITE_ONLY)
			return ape::Texture::Usage::DYNAMIC_WRITE_ONLY;
	}

	inline Ogre::Radian ConversionToOgre21(const ape::Radian& mwRad)
	{
		return Ogre::Radian(mwRad.radian);
	}

	inline ape::Radian ConversionFromOgre21(const Ogre::Radian& oRad)
	{
		return ape::Radian(oRad.valueRadians());
	}

	inline ape::Light::Type ConversionFromOgre21(Ogre::Light::LightTypes type)
	{
		switch (type)
		{
		case Ogre::Light::LT_POINT:
			return ape::Light::Type::POINT;
		case Ogre::Light::LT_DIRECTIONAL:
			return ape::Light::Type::DIRECTIONAL;
		case Ogre::Light::LT_SPOTLIGHT:
			return ape::Light::Type::SPOT;
		default:
			return ape::Light::Type::INVALID;
		}
	}

	inline Ogre::Light::LightTypes ConversionToOgre21(ape::Light::Type type)
	{
		switch (type)
		{
		case ape::Light::Type::SPOT:
			return Ogre::Light::LightTypes::LT_SPOTLIGHT;
		case ape::Light::Type::DIRECTIONAL:
			return Ogre::Light::LightTypes::LT_DIRECTIONAL;
		case ape::Light::Type::POINT:
			return Ogre::Light::LightTypes::LT_POINT;
		case ape::Light::Type::INVALID:
		default:
			return Ogre::Light::LightTypes(-1);
		}
	}
	inline ape::Camera::ProjectionType ConversionFromOgre21(const Ogre::ProjectionType type)
	{
		switch (type)
		{
			case Ogre::ProjectionType::PT_ORTHOGRAPHIC:
				return ape::Camera::ProjectionType::ORTHOGRAPHIC;
			case Ogre::ProjectionType::PT_PERSPECTIVE:
				return ape::Camera::ProjectionType::PERSPECTIVE;
			default:
				return ape::Camera::ProjectionType::INVALID;
		}
	}

	inline Ogre::ProjectionType ConversionToOgre21(const ape::Camera::ProjectionType type)
	{
		switch (type)
		{
		case ape::Camera::ProjectionType::ORTHOGRAPHIC:
			return Ogre::ProjectionType::PT_ORTHOGRAPHIC;
		case ape::Camera::ProjectionType::PERSPECTIVE:
			return Ogre::ProjectionType::PT_PERSPECTIVE;
		default:
			return Ogre::ProjectionType::PT_ORTHOGRAPHIC;
		}
	}
}

#endif