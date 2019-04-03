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

#ifndef APE_PASS_H
#define APE_PASS_H

#include <string>
#include <vector>
#include "ape.h"
#include "datatypes/apeColor.h"
#include "datatypes/apeEntity.h"
#include "datatypes/apeTexture.h"
#include "datatypes/apeTexture.h"
#include "datatypes/apeVector3.h"

namespace ape
{
	struct PassGpuVector3Parameter
	{
		std::string name;
		ape::Vector3 value;

		PassGpuVector3Parameter()
		{
			this->value = ape::Vector3();
			this->name = std::string();
		}

		PassGpuVector3Parameter(std::string name, ape::Vector3 value)
		{
			this->value = value;
			this->name = name;
		}
	};

	typedef std::vector<PassGpuVector3Parameter> PassGpuParameters;

	class Pass : public Entity
	{
	public:
		enum SceneBlendingType
		{
			ADD,
			TRANSPARENT_ALPHA,
			REPLACE,
			INVALID
		};
	protected:
		Pass(std::string name, Entity::Type entityType) : Entity(name, entityType) {}

		virtual ~Pass() {};

		ape::Color mDiffuseColor;

		ape::Color mSpecularColor;

		ape::Color mAmbientColor;

		ape::Color mEmissiveColor;

		float mShininess;

		ape::TextureWeakPtr mTexture;

		std::string mTextureName;

		ape::PassGpuParameters mPassGpuParameters;

		ape::Pass::SceneBlendingType mSceneBlendingType;

	public:
		ape::Color getDiffuseColor() { return mDiffuseColor; };

		ape::Color getSpecularColor() { return mSpecularColor; };

		ape::Color getAmbientColor() { return mAmbientColor; };

		ape::Color getEmissiveColor() { return mEmissiveColor; };

		float getShininess() { return mShininess; };

		ape::Pass::SceneBlendingType getSceneBlendingType() { return mSceneBlendingType; };

		ape::TextureWeakPtr getTexture() { return mTexture; };

		ape::PassGpuParameters getPassGpuParameters() { return mPassGpuParameters; };
	};
}

#endif

