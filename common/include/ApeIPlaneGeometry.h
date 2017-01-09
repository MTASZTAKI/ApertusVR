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

#ifndef APE_IPLANEGEOMETRY_H
#define APE_IPLANEGEOMETRY_H

#include <string>
#include <vector>
#include "ApeGeometry.h"
#include "ApeVector2.h"
#include "ApeVector3.h"
#include "ApeINode.h"

namespace Ape
{	
	struct GeometryPlaneParameters
	{
		Ape::Vector2 numSeg;
		Ape::Vector2 size;
		Ape::Vector2 tile;

		GeometryPlaneParameters()
		{
			this->numSeg = Ape::Vector2();
			this->size = Ape::Vector2();
			this->tile = Ape::Vector2();
		}

		GeometryPlaneParameters(
			Ape::Vector2 numSeg,
			Ape::Vector2 size,
			Ape::Vector2 tile)
		{
			this->numSeg = numSeg;
			this->size = size;
			this->tile = tile;
		}
	};

	class IPlaneGeometry : public Ape::Geometry
	{
	protected:
		IPlaneGeometry(std::string name) : Ape::Geometry(name, Ape::Entity::GEOMETRY_PLANE) {}
		
		virtual ~IPlaneGeometry() {};
		
	public:
		virtual void setParameters(Ape::Vector2 numSeg, Ape::Vector2 size, Ape::Vector2 tile) = 0;
		
		virtual Ape::GeometryPlaneParameters getParameters() = 0;

		virtual void setParentNode(Ape::NodeWeakPtr parentNode) = 0;

		virtual void setMaterial(Ape::MaterialWeakPtr material) = 0;

		virtual Ape::MaterialWeakPtr getMaterial() = 0;
	};
}

#endif
