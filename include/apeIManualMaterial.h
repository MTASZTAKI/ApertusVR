/*MIT License

Copyright (c) 2018 MTA SZTAKI

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

#ifndef APE_IMANUALMATERIAL_H
#define APE_IMANUALMATERIAL_H

#include "apeMaterial.h"

namespace ape
{
	class IManualMaterial : public Material
	{
	protected:
		IManualMaterial(std::string name, bool replicate) : Material(name, replicate, Entity::MATERIAL_MANUAL) {}

		virtual ~IManualMaterial() {};

		ape::Color mDiffuseColor;

		ape::Color mSpecularColor;

		ape::Color mAmbientColor;

		ape::Color mEmissiveColor;

	public:
		virtual void setDiffuseColor(Color diffuse) = 0;

		virtual void setSpecularColor(Color specular) = 0;

		ape::Color getDiffuseColor() { return mDiffuseColor; };

		ape::Color getSpecularColor() { return mSpecularColor; };

		virtual void setAmbientColor(Color ambient) = 0;

		virtual void setEmissiveColor(Color emissive) = 0;

		ape::Color getAmbientColor() { return mAmbientColor; };

		ape::Color getEmissiveColor() { return mEmissiveColor; };

		virtual void setTexture(ape::TextureWeakPtr texture) = 0;

		virtual ape::TextureWeakPtr getTexture() = 0;

		virtual void setCullingMode(ape::Material::CullingMode cullingMode) = 0;

		virtual void setSceneBlending(ape::Material::SceneBlendingType sceneBlendingType) = 0;

		virtual void setDepthWriteEnabled(bool enable) = 0;

		virtual void setDepthCheckEnabled(bool enable) = 0;

		virtual void setLightingEnabled(bool enable) = 0;

		virtual void setManualCullingMode(ape::Material::ManualCullingMode manualcullingMode) = 0;

		virtual void setDepthBias(float constantBias, float slopeScaleBias) = 0;

		virtual void showOnOverlay(bool enable, int zOrder) = 0;

		virtual int getZOrder() = 0;
	};
}

#endif

