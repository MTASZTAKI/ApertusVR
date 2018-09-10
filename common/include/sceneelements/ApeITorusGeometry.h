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

#ifndef APE_ITORUSGEOMETRY_H
#define APE_ITORUSGEOMETRY_H

#include <string>
#include <vector>
#include "datatypes/ApeGeometry.h"
#include "datatypes/ApeVector2.h"
#include "datatypes/ApeVector3.h"
#include "sceneelements/ApeINode.h"

namespace Ape
{
	struct GeometryTorusParameters
	{
		float radius;
		float sectionRadius;
		Ape::Vector2 tile;

		GeometryTorusParameters()
		{
			this->radius = 0.0f;
			this->sectionRadius = 0.0f;
			this->tile = Ape::Vector2();
		}

		GeometryTorusParameters(float radius, float sectionRadius, Ape::Vector2 tile)
		{
			this->radius = radius;
			this->sectionRadius = sectionRadius;
			this->tile = tile;
		}
	};

	class ITorusGeometry : public Ape::Geometry
	{
	protected:
		ITorusGeometry(std::string name) : Ape::Geometry(name, Ape::Entity::GEOMETRY_TORUS) {}

		virtual ~ITorusGeometry() {};

	public:
		virtual void setParameters(float radius, float sectionRadius, Ape::Vector2 tile) = 0;

		virtual Ape::GeometryTorusParameters getParameters() = 0;

		virtual void setParentNode(Ape::NodeWeakPtr parentNode) = 0;

		virtual void setMaterial(Ape::MaterialWeakPtr material) = 0;

		virtual Ape::MaterialWeakPtr getMaterial() = 0;
	};
}

#endif
