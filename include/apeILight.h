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

#ifndef APE_ILIGHT_H
#define APE_ILIGHT_H

#include "datatypes/apeColor.h"
#include "datatypes/apeDegree.h"
#include "datatypes/apeEntity.h"
#include "datatypes/apeRadian.h"
#include "datatypes/apeVector3.h"
#include "sceneelements/apeINode.h"

namespace ape
{
	namespace Light
	{
		enum Type
		{
			SPOT,
			DIRECTIONAL,
			POINT,
			INVALID
		};
	}

	struct LightSpotRange
	{
		Degree innerAngle;
		Degree outerAngle;
		float falloff;

		LightSpotRange() : innerAngle(Degree()), outerAngle(Degree()), falloff(0.0f)
		{}

		LightSpotRange(Degree innerAngleDeg, Degree outerAngleDeg, float falloff) :
			innerAngle(innerAngleDeg), outerAngle(outerAngleDeg), falloff(falloff)
		{}

		LightSpotRange(Radian innerAngleRad, Radian outerAngleRad, float falloff) :
			innerAngle(innerAngleRad.toDegree()), outerAngle(outerAngleRad.toDegree()), falloff(falloff)
		{}
	};

	struct LightAttenuation
	{
		float range;
		float constant;
		float linear;
		float quadratic;

		LightAttenuation() : range(0.0f), constant(0.0f), linear(0.0f), quadratic(0.0f)
		{}

		LightAttenuation(float _range, float _constant, float _linear, float _quadratic) :
			range(_range), constant(_constant), linear(_linear), quadratic(_quadratic)
		{}
	};

	class ILight : public Entity
	{
	protected:
		ILight(std::string name) : Entity(name, ape::Entity::LIGHT) {}

		virtual ~ILight() {};

	public:
		virtual ape::Light::Type getLightType() = 0;

		virtual Color getDiffuseColor() = 0;

		virtual Color getSpecularColor() = 0;

		virtual LightSpotRange getLightSpotRange() = 0;

		virtual LightAttenuation getLightAttenuation() = 0;

		virtual Vector3 getLightDirection() = 0;

		virtual void setLightType(ape::Light::Type lightType) = 0;

		virtual void setDiffuseColor(Color diffuseColor) = 0;

		virtual void setSpecularColor(Color specularColor) = 0;

		virtual void setLightSpotRange(LightSpotRange spotRange) = 0;

		virtual void setLightAttenuation(LightAttenuation lightAttenuation) = 0;

		virtual void setLightDirection(Vector3 lightDirection) = 0;

		virtual void setParentNode(ape::NodeWeakPtr parentNode) = 0;

		virtual ape::NodeWeakPtr getParentNode() = 0;

	};
}

#endif
