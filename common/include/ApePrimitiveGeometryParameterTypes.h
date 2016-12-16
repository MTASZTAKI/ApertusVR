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

#ifndef APE_PrimitiveGeometryTYPES_H
#define APE_PrimitiveGeometryTYPES_H

#include <string>
#include <vector>
#include "ApeVector2.h"
#include "ApeVector3.h"

namespace Ape
{	
	namespace PrimitiveGeometryParameter
	{
		enum Type
		{
			PLANE,
			BOX,
			SPHERE,
			TORUS,
			CYLINDER,
			CONE,
			TUBE,
			INVALID
		};
	}

	struct PrimitiveGeometryParameterBase
	{
		Ape::PrimitiveGeometryParameter::Type type;

		PrimitiveGeometryParameterBase()
		{
			this->type = Ape::PrimitiveGeometryParameter::INVALID;
		}

		PrimitiveGeometryParameterBase(
			Ape::PrimitiveGeometryParameter::Type type)
		{
			this->type = type;
		}

		virtual ~PrimitiveGeometryParameterBase()
		{

		}
	};

	struct PrimitiveGeometryParameterBox : Ape::PrimitiveGeometryParameterBase
	{
		Ape::Vector3 dimensions;

		PrimitiveGeometryParameterBox(
			Ape::Vector3 dimensions) : Ape::PrimitiveGeometryParameterBase(Ape::PrimitiveGeometryParameter::Type::BOX)
		{
			this->dimensions = dimensions;
		}
	};

	struct PrimitiveGeometryParameterPlane : Ape::PrimitiveGeometryParameterBase
	{
		Ape::Vector2 numSeg;
		Ape::Vector2 size;
		Ape::Vector2 tile;

		PrimitiveGeometryParameterPlane(
			Ape::Vector2 numSeg,
			Ape::Vector2 size,
			Ape::Vector2 tile) : Ape::PrimitiveGeometryParameterBase(Ape::PrimitiveGeometryParameter::Type::PLANE)
		{
			this->numSeg = numSeg;
			this->size = size;
			this->tile = tile;
		}
	};

	struct PrimitiveGeometryParameterSphere : Ape::PrimitiveGeometryParameterBase
	{
		float radius;
		Ape::Vector2 tile;

		PrimitiveGeometryParameterSphere(
			float radius,
			Ape::Vector2 tile) : Ape::PrimitiveGeometryParameterBase(Ape::PrimitiveGeometryParameter::Type::SPHERE)
		{
			this->radius = radius;
			this->tile = tile;
		}
	};

	struct PrimitiveGeometryParameterCylinder : Ape::PrimitiveGeometryParameterBase
	{
		float radius;
		float height;
		float tile;

		PrimitiveGeometryParameterCylinder(
			float radius,
			float height,
			float tile) : Ape::PrimitiveGeometryParameterBase(Ape::PrimitiveGeometryParameter::Type::CYLINDER)
		{
			this->radius = radius;
			this->height = height;
			this->tile = tile;
		}
	};

	struct PrimitiveGeometryParameterTorus : Ape::PrimitiveGeometryParameterBase
	{
		float radius;
		float sectionRadius;
		Ape::Vector2 tile;

		PrimitiveGeometryParameterTorus(
			float radius,
			float sectionRadius,
			Ape::Vector2 tile) : Ape::PrimitiveGeometryParameterBase(Ape::PrimitiveGeometryParameter::Type::TORUS)
		{
			this->radius = radius;
			this->sectionRadius = sectionRadius;
			this->tile = tile;
		}
	};

	struct PrimitiveGeometryParameterCone : Ape::PrimitiveGeometryParameterBase
	{
		float radius;
		float height;
		float tile;
		Ape::Vector2 numSeg;

		PrimitiveGeometryParameterCone(
			float radius,
			float height,
			float tile,
			Ape::Vector2 numSeg) : Ape::PrimitiveGeometryParameterBase(Ape::PrimitiveGeometryParameter::Type::CONE)
		{
			this->radius = radius;
			this->height = height;
			this->tile = tile;
			this->numSeg = numSeg;
		}
	};

	struct PrimitiveGeometryParameterTube : Ape::PrimitiveGeometryParameterBase
	{
		float height;
		float tile;

		PrimitiveGeometryParameterTube(
			float height,
			float tile) : Ape::PrimitiveGeometryParameterBase(Ape::PrimitiveGeometryParameter::Type::TUBE)
		{
			this->height = height;
			this->tile = tile;
		}
	};
}

#endif
