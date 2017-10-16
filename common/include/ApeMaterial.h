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


#ifndef APE_MATERIAL_H
#define APE_MATERIAL_H

#include <string>
#include <vector>
#include "Ape.h"
#include "ApeVector2.h"
#include "ApeEntity.h"
#include "ApeColor.h"
#include "ApePass.h"

namespace Ape
{	
	class Material : public Entity
	{
	public:
		enum CullingMode
		{
			NONE_CM,
			CLOCKWISE,
			ANTICLOCKWISE,
			INVALID_CM
		};
		enum ManualCullingMode
		{
			NONE_MCM,
			BACK,
			FRONT,
			INVALID_MCM
		};

	protected:
	    Material(std::string name, Entity::Type entityType) : Entity(name,entityType) {}
		
		virtual ~Material() {};

		Ape::PassWeakPtr mPass;

		CullingMode mCullingMode;

		ManualCullingMode mManualCullingMode;

		Ape::Pass::SceneBlendingType mSceneBlendingType;

		bool mDepthWriteEnabled;

		bool mDepthCheckEnabled;

		bool mLightingEnabled;

		Ape::Vector2 mDepthBias;

		std::string mPassName;

	public:
		Ape::PassWeakPtr getPass() { return mPass; };

		CullingMode getCullingMode() { return mCullingMode; };

		ManualCullingMode getManualCullingMode() { return mManualCullingMode; };

		bool getDepthWriteEnabled() { return mDepthWriteEnabled; };

		bool getDepthCheckEnabled() { return mDepthCheckEnabled; };

		Ape::Vector2 getDepthBias() { return mDepthBias; };

		bool getLightingEnabled() { return mLightingEnabled; };

		Ape::Pass::SceneBlendingType getSceneBlendingType() { return mSceneBlendingType; };
	};
}

#endif

