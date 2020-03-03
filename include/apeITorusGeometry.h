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
#include "apeGeometry.h"
#include "apeVector2.h"
#include "apeVector3.h"
#include "apeINode.h"

namespace ape
{
	struct GeometryTorusParameters
	{
		float radius;
		float sectionRadius;
		ape::Vector2 tile;

		GeometryTorusParameters()
		{
			this->radius = 0.0f;
			this->sectionRadius = 0.0f;
			this->tile = ape::Vector2();
		}

		GeometryTorusParameters(float radius, float sectionRadius, ape::Vector2 tile)
		{
			this->radius = radius;
			this->sectionRadius = sectionRadius;
			this->tile = tile;
		}
	};

	class ITorusGeometry : public ape::Geometry
	{
	protected:
		ITorusGeometry(std::string name, bool replicate, std::string ownerID) : ape::Geometry(name, ape::Entity::GEOMETRY_TORUS, replicate, ownerID) {}

		virtual ~ITorusGeometry() {};

	public:
		virtual void setParameters(float radius, float sectionRadius, ape::Vector2 tile) = 0;

		virtual ape::GeometryTorusParameters getParameters() = 0;

		virtual void setParentNode(ape::NodeWeakPtr parentNode) = 0;

		virtual void setMaterial(ape::MaterialWeakPtr material) = 0;

		virtual ape::MaterialWeakPtr getMaterial() = 0;
	};
}

#endif
