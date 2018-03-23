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
#include "ApeMatrix4.h"
#include "ApeQuaternion.h"
#include "ApeColor.h"
#include "ApeILight.h"
#include "ApeICamera.h"
#include "ApeIFileGeometry.h"
#include "OgreVector3.h"
#include "OgreVector2.h"
#include "OgreVector4.h"
#include "OgreQuaternion.h"
#include "OgreColourValue.h"
#include "OgreLight.h"
#include "OgreMatrix4.h"

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

	inline Ogre::Matrix4 ConversionToOgre(const Ape::Matrix4& p_mat4)
	{
		return Ogre::Matrix4(
			p_mat4.m[0][0], p_mat4.m[0][1], p_mat4.m[0][2], p_mat4.m[0][3],
			p_mat4.m[1][0], p_mat4.m[1][1], p_mat4.m[1][2], p_mat4.m[1][3],
			p_mat4.m[2][0], p_mat4.m[2][1], p_mat4.m[2][2], p_mat4.m[2][3],
			p_mat4.m[3][0], p_mat4.m[3][1], p_mat4.m[3][2], p_mat4.m[3][3]
			);
	}

	inline Ape::Matrix4 ConversionFromOgre(const Ogre::Matrix4& p_mat4)
	{
		return Ape::Matrix4(
			p_mat4[0][0], p_mat4[0][1], p_mat4[0][2], p_mat4[0][3],
			p_mat4[1][0], p_mat4[1][1], p_mat4[1][2], p_mat4[1][3],
			p_mat4[2][0], p_mat4[2][1], p_mat4[2][2], p_mat4[2][3],
			p_mat4[3][0], p_mat4[3][1], p_mat4[3][2], p_mat4[3][3]
			);
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

	inline Ogre::ProjectionType ConversionToOgre(const Ape::Camera::ProjectionType type)
	{
		if (type == Ape::Camera::ProjectionType::ORTHOGRAPHIC)
			return Ogre::ProjectionType::PT_ORTHOGRAPHIC;
		else if (type == Ape::Camera::ProjectionType::PERSPECTIVE)
			return Ogre::ProjectionType::PT_PERSPECTIVE;
	}

	inline Ape::Camera::ProjectionType ConversionFromOgre(const Ogre::ProjectionType type)
	{
		if (type == Ogre::ProjectionType::PT_ORTHOGRAPHIC)
			return Ape::Camera::ProjectionType::ORTHOGRAPHIC;
		else if (type == Ogre::ProjectionType::PT_PERSPECTIVE)
			return Ape::Camera::ProjectionType::PERSPECTIVE;
	}

	inline Ogre::SceneBlendType ConversionToOgre(const Ape::Pass::SceneBlendingType type)
	{
		if (type == Ape::Pass::SceneBlendingType::ADD)
			return Ogre::SceneBlendType::SBT_ADD;
		else if (type == Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA)
			return Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA;
		else if (type == Ape::Pass::SceneBlendingType::REPLACE)
			return Ogre::SceneBlendType::SBT_REPLACE;
	}

	inline Ape::Pass::SceneBlendingType ConversionFromOgre(const Ogre::SceneBlendType type)
	{
		if (type == Ogre::SceneBlendType::SBT_ADD)
			return Ape::Pass::SceneBlendingType::ADD;
		else if (type == Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA)
			return Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA;
		else if (type == Ogre::SceneBlendType::SBT_REPLACE)
			return Ape::Pass::SceneBlendingType::REPLACE;
	}

	inline Ogre::CullingMode ConversionToOgre(const Ape::Material::CullingMode type)
	{
		if (type == Ape::Material::CullingMode::NONE_CM)
			return Ogre::CullingMode::CULL_NONE;
		else if (type == Ape::Material::CullingMode::CLOCKWISE)
			return Ogre::CullingMode::CULL_CLOCKWISE;
		else if (type == Ape::Material::CullingMode::ANTICLOCKWISE)
			return Ogre::CullingMode::CULL_ANTICLOCKWISE;
	}

	inline Ape::Material::CullingMode ConversionFromOgre(const Ogre::CullingMode type)
	{
		if (type == Ogre::CullingMode::CULL_NONE)
			return Ape::Material::CullingMode::NONE_CM;
		else if (type == Ogre::CullingMode::CULL_CLOCKWISE)
			return Ape::Material::CullingMode::CLOCKWISE;
		else if (type == Ogre::CullingMode::CULL_ANTICLOCKWISE)
			return Ape::Material::CullingMode::ANTICLOCKWISE;
	}

	inline Ogre::TextureUnitState::TextureAddressingMode ConversionToOgre(const Ape::Texture::AddressingMode type)
	{
		if (type == Ape::Texture::AddressingMode::BORDER)
			return Ogre::TextureUnitState::TextureAddressingMode::TAM_BORDER;
		else if (type == Ape::Texture::AddressingMode::CLAMP)
			return Ogre::TextureUnitState::TextureAddressingMode::TAM_CLAMP;
		else if (type == Ape::Texture::AddressingMode::MIRROR)
			return Ogre::TextureUnitState::TextureAddressingMode::TAM_MIRROR;
		else if (type == Ape::Texture::AddressingMode::WRAP)
			return Ogre::TextureUnitState::TextureAddressingMode::TAM_WRAP;
	}

	inline Ape::Texture::AddressingMode ConversionFromOgre(const Ogre::TextureUnitState::TextureAddressingMode type)
	{
		if (type == Ogre::TextureUnitState::TextureAddressingMode::TAM_BORDER)
			return Ape::Texture::AddressingMode::BORDER;
		else if (type == Ogre::TextureUnitState::TextureAddressingMode::TAM_CLAMP)
			return Ape::Texture::AddressingMode::CLAMP;
		else if (type == Ogre::TextureUnitState::TextureAddressingMode::TAM_MIRROR)
			return Ape::Texture::AddressingMode::MIRROR;
		else if (type == Ogre::TextureUnitState::TextureAddressingMode::TAM_WRAP)
			return Ape::Texture::AddressingMode::WRAP;
	}

	inline Ogre::FilterOptions ConversionToOgre(const Ape::Texture::Filtering type)
	{
		if (type == Ape::Texture::Filtering::ANISOTROPIC)
			return Ogre::FilterOptions::FO_ANISOTROPIC;
		else if (type == Ape::Texture::Filtering::LINEAR)
			return Ogre::FilterOptions::FO_LINEAR;
		else if (type == Ape::Texture::Filtering::POINT)
			return Ogre::FilterOptions::FO_POINT;
	}

	inline Ape::Texture::Filtering ConversionFromOgre(const Ogre::FilterOptions type)
	{
		if (type == Ogre::FilterOptions::FO_ANISOTROPIC)
			return Ape::Texture::Filtering::ANISOTROPIC;
		else if (type == Ogre::FilterOptions::FO_LINEAR)
			return Ape::Texture::Filtering::LINEAR;
		else if (type == Ogre::FilterOptions::FO_POINT)
			return Ape::Texture::Filtering::POINT;
	}

	inline Ogre::PixelFormat ConversionToOgre(const Ape::Texture::PixelFormat type)
	{
		if (type == Ape::Texture::PixelFormat::A8R8G8B8)
			return Ogre::PixelFormat::PF_A8R8G8B8;
		else if (type == Ape::Texture::PixelFormat::R8G8B8)
			return Ogre::PixelFormat::PF_R8G8B8;
	}

	inline Ape::Texture::PixelFormat ConversionFromOgre(const Ogre::PixelFormat type)
	{
		if (type == Ogre::PixelFormat::PF_A8R8G8B8)
			return Ape::Texture::PixelFormat::A8R8G8B8;
		else if (type == Ogre::PixelFormat::PF_R8G8B8)
			return Ape::Texture::PixelFormat::R8G8B8;
	}

	inline Ogre::TextureUsage ConversionToOgre(const Ape::Texture::Usage type)
	{
		if (type == Ape::Texture::Usage::RENDERTARGET)
			return Ogre::TextureUsage::TU_RENDERTARGET;
		else if (type == Ape::Texture::Usage::DYNAMIC_WRITE_ONLY)
			return Ogre::TextureUsage::TU_DYNAMIC_WRITE_ONLY;
	}

	inline Ape::Texture::Usage ConversionFromOgre(const Ogre::TextureUsage type)
	{
		if (type == Ogre::TextureUsage::TU_RENDERTARGET)
			return Ape::Texture::Usage::RENDERTARGET;
		else if (type == Ogre::TextureUsage::TU_DYNAMIC_WRITE_ONLY)
			return Ape::Texture::Usage::DYNAMIC_WRITE_ONLY;
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
