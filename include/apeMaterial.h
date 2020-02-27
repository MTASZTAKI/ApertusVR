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

#ifndef APE_MATERIAL_H
#define APE_MATERIAL_H

#include <string>
#include <vector>
#include "ape.h"
#include "apeColor.h"
#include "apeEntity.h"
#include "apeVector2.h"

namespace ape
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
		enum SceneBlendingType
		{
			NONE_SBT,
			ADD,
			TRANSPARENT_ALPHA,
			REPLACE,
			INVALID_SBT
		};

	protected:
		Material(std::string name, bool replicated,  Entity::Type entityType) : Entity(name, replicated, entityType) {}

		virtual ~Material() {};

		CullingMode mCullingMode;

		ManualCullingMode mManualCullingMode;

		SceneBlendingType mSceneBlendingType;

		bool mDepthWriteEnabled;

		bool mDepthCheckEnabled;

		bool mLightingEnabled;

		ape::Vector2 mDepthBias;

		bool mIsShowOnOverlay;

	public:

		CullingMode getCullingMode() { return mCullingMode; };

		ManualCullingMode getManualCullingMode() { return mManualCullingMode; };

		bool getDepthWriteEnabled() { return mDepthWriteEnabled; };

		bool getDepthCheckEnabled() { return mDepthCheckEnabled; };

		ape::Vector2 getDepthBias() { return mDepthBias; };

		bool getLightingEnabled() { return mLightingEnabled; };

		SceneBlendingType getSceneBlendingType() { return mSceneBlendingType; };

		bool isShowOnOverlay() { return mIsShowOnOverlay; };
	};
}

#endif

